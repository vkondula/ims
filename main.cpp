/*
Simulation model of a restaurant during a lunch time

Authors:
  xkrajn02 - Krajnak Martin
  xkondu00 - Kondula Vaclav
*/
#include "main.hpp"

using namespace std;
using json = nlohmann::json;

vector<Zone *> zones;
Kitchen * kitchen;
Statistics * stat;

int main(int argc, char* argv[]) {
  /* seed - get from current time  */
  RandomSeed(time(NULL));
  srand(time(NULL));
  /* parse argument */
  json args = parse_arguments(argc, argv);
  /* set output file */
  try{
    const string output_str = args["output_file"];
    SetOutput(output_str.c_str());
  }
  catch (domain_error){
    SetOutput("output.out");
  }
  stat = new Statistics();
  Init(TIME_BEGIN, TIME_END + 3 * HOUR);
  /* setup zones and files based on input json */
  init_zones(args);
  (new Generator)->Activate();
  Run();
  stat->Output();
  dealoc_global();
  return 0;
}

void Generator::Behavior(){
  if(Time < TIME_END){

    if (Time > PEAK_TIME_START && Time < PEAK_TIME_END){ // more people comming
      Activate(Time + Exponential(MINUTE + 40));
    }else{
      Activate(Time + Exponential(2 * MINUTE + 40));
    }
    double pick_group = Random();//TODO: change weights
    int member_count = 0;

    if (pick_group > 0 && pick_group <= 0.1)            member_count=1;
    if (pick_group > 0.1 && pick_group <= 0.4)          member_count=2;
    if (pick_group > 0.4 && pick_group <= 0.5)          member_count=3;
    if (pick_group > 0.5 && pick_group <= 0.7)          member_count=4;
    if (pick_group > 0.7 && pick_group <= 0.8)          member_count=5;
    if (pick_group > 0.8 && pick_group <= 0.85)         member_count=6;
    if (pick_group > 0.85 && pick_group <= 0.9)         member_count=7;
    if (pick_group > 0.90 && pick_group <= 1)           member_count=8;
    Group * new_group = new Group(member_count);
    new_group->Activate();
  }
}

void init_zones(json args){
  try{
    int cooks = args["cooks"];
    // iterate the zones
    for (json::iterator it = args["zones"].begin(); it != args["zones"].end(); ++it) {
      Zone * zone = new Zone((*it)["waiters"], (*it)["tables"]);
      zones.push_back(zone);
    }
    kitchen = new Kitchen(cooks);
  }
  catch (domain_error){
    cerr << "Invalid JSON data" << '\n';
    exit(1);
  }
  kitchen->activate_cooks();
  return;
}


json parse_arguments(int argc, char* argv[]){
  if(argc == 2 && !strcmp(argv[1], "--help")){
      cout << "Simulation model of a restaurant" << endl;
      cout << "./ims [INPUTFILE, [OUTPUTFILE]]" << endl;
      cout << "Usage:" << endl;
      cout << "  No parameters: implicit number of waiters, tables, zones and cooks" << endl;
      cout << "  INPUTFILE: set json file, that specifies environment (see examples)" << endl;
      cout << "  OUTPUTFILE: set output file for simulation data" << endl;
      exit(0);
  }
  json args = {
    {"cooks", 2},
    {"output_file", "output.out"},
    {"zones", {
        {
          {"tables", {2,2,4,8,8}},
          {"waiters", 2}
        },
        {
          {"tables", {4,6,6}},
          {"waiters", 1}
        }
      }
    }
  };
  if (argc > 1){
    ifstream input_file (argv[1]);
    if (!input_file.is_open()){
      cerr << "Couldn't open input file" << endl;
      exit(1);
    }
    try{
      args = json::parse(input_file);
    }
    catch (invalid_argument){
      cerr << "Invalid JSON format" << '\n';
      exit(1);
    }
  }
  if (argc == 3){
    args["output_file"] = argv[2];
  }
  return args;
}

Store * Zone::find_table(int size, bool force){
  vector<Store *> table_g(this->table); // Create copy of table pointers
  random_shuffle(table_g.begin(), table_g.end()); // Validate tables in random order
  vector<Store *> table_acceptable; // Acceptable tables (>= then group)
  for(Store * t : table_g){
      if ((t->Free() >= (unsigned int)size) && (force || t->Empty())){
        table_acceptable.push_back(t);
      }
  }
  unsigned int min_size = 9000;
  Store * table = NULL;
  for(Store * t : table_acceptable){ //return smallest acceptable table for group
      if (t->Capacity() < min_size){
        min_size = t->Capacity();
        table = t;
      }
  }
  return table;
}

int ZONE_COUNTER=1;

Zone::Zone(int waiter_count, vector<int> tables_sizes){
  this->id=ZONE_COUNTER++;
  this->waiter.reserve(waiter_count);
  this->table.reserve(tables_sizes.size());
  for (int i : tables_sizes){
    Store * t = new Store();
    t->SetCapacity(i);
    this->table.push_back(t);
  }
  for (int i = 0; i<waiter_count; i++){
    Waiter * w = new Waiter();
    this->waiter.push_back(w);
    w->set_zone(this);
  }
}

Zone::~Zone(){
  for (Waiter * w : this->waiter) { delete w; }
  for (Store * s : this->table) { delete s; }
}

void Zone::waiter_no_queue(double duration){
  this->waiter_waiting(duration);
}

int Zone::get_id(){
  return this->id;
}

int Zone::get_waiter_count(){
  return this->waiter.size();
}

void dealoc_global(){
  for (Zone * z : zones){ delete z; }
  delete kitchen;
  delete stat;
}
