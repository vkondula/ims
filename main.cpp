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
    new Group(member_count);
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
      if (!this->zone->q.Empty()){
        Group * group = (Group *) this->zone->q.GetFirst();
        // TODO: cesta kuchyn -> zona
        group->curr_waiter = this;
        group->Activate();
        Passivate();
        // TODO: cesta z5
      } else {
        /* Wait for next request */
        Wait(10);
      }
  }
}

void Waiter::set_zone(Zone * z){
  this->zone = z;
}

Zone * find_zone_with_table(int size){
  // TODO: zmenit poradit zon pro kazdou zkupinu
  for (Zone * z : zones){
    Store * s = z->find_table(size);
    if(s) return z;
  }
  return NULL;
}

Store * Zone::find_table(int size){
  for(Store * t : this->table){
    if (t->Free() >= (unsigned int)size ) return t;
  }
  return NULL;
}
