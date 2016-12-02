/*
Simulation model of a restaurant during a lunch time

Authors:
  xkrajn02 - Krajnak Martin
  xkondu00 - Kondula Vaclav
*/
#include "main.hpp"

using namespace std;
vector<Zone *> zones;

int main() {
  RandomSeed(time(NULL));
  srand(time(NULL));
  SetOutput("output.out");
  Init(TIME_BEGIN, TIME_END + 3 * HOUR);
  init_zones();
  (new Generator)->Activate();
  Run();
  return 0;
}

void Generator::Behavior(){
  if(Time < TIME_END){

    if (Time > PEAK_TIME_START && Time < PEAK_TIME_END){ // more people comming
        Activate(Time + Exponential(MINUTE));
    }else{
      Activate(Time + Exponential(2 * MINUTE));
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


void init_zones(){
  //CHANGE!!
  vector<int> t1 = {4,4,4,4,6};
  vector<int> t2 = {2,2,2,4,8};
  Zone * zone1 = new Zone(1, t1);
  Zone * zone2 = new Zone(2, t2);
  zones.reserve(2);
  zones.push_back(zone1);
  zones.push_back(zone2);
  return;
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

Zone::Zone(int waiter_count, vector<int> tables_sizes){
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
