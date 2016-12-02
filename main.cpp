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

void Waiter::Behavior(){
  while (true){
      Queue * q = NULL;
      if (!this->get_zone()->priority_q.Empty()) q = &(this->get_zone()->priority_q);
      else if (!this->get_zone()->q.Empty()) q = &(this->get_zone()->q);
      else {
        /* Wait for next request */
        Wait(10);
        continue;
      }
      Group * group = (Group *) q->GetFirst();
      this->handle_request(group);
  }
}

void Waiter::set_zone(Zone * z){
  this->zone = z;
}

Zone * Waiter::get_zone(){
  return this->zone;
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

bool Waiter::is_in_zone(){
  return this->in_zone;
}

void Waiter::move(bool kitchen, bool slow=false){
  int duration = (kitchen ? TIME_TO_KITCHEN : TIME_TO_TABLE);
  if (slow) duration = (int)(duration * 1.5);
  if (kitchen) this->in_zone = !(this->in_zone);
  Wait(duration);
}

void Waiter::handle_request(Group * group){
  tPhase phase = group->get_phase();
  if (phase == WF_CLEAN && group->is_wf_drink()){
    /* Merge of bringing drink and cleaning plates in one */
    phase=END_ENUM;
    group->set_skip_clean();
  }
  if (phase == WF_CLEAN && group->get_skip_clean()){
    /* No need to clean dishes for the second time*/
    /* Dishes were taken away when drink arrived */
    return;
  }
  switch (phase) {
    case WF_ORDER:
    case WF_MEAL:
    case SOUP:
    case END_ENUM:
      /* Need to bring something from kitchen (food / dring)*/
      if (this->is_in_zone()) this->move(true); // Go to kitchen (fast)
      this->move(true, true); // Go back with plates (slow)
      this->move(false, true); // Go to the table with plates (slow)
      break;
    case WF_CLEAN:
    case WF_PAYMENT:
      /* Requires just the waiter */
      if (!this->is_in_zone()) this->move(true); // Go to zone (fast)
      this->move(false); // Go to the table (fast)
      break;
    default:
      break;
  }
  if (group->is_wf_drink()){
    //only asynchronous action, no need for group co-operation
    group->set_wf_drink(false);
    Wait(TIME_TO_SERVE_MEAL);
    std::cerr << "/* DRINK SERVED */"<< phase << std::endl;
    return;
  } else {
    // These actions are handled by Group
    group->set_curr_waiter(this);
    group->Activate();
    Passivate();
  }
  switch (phase) {
    case WF_CLEAN:
    case WF_PAYMENT:
    case END_ENUM:
      /* Need to take away something to kitchen (dishes) */
      this->move(true, true);
      break;
    default:
      /* Leave table */
      Wait(2);
      break;
  }
}
