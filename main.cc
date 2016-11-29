/*
Simulation model of a restaurant during a lunch time

Authors:
  xkrajn02 - Krajnak Martin
  xkondu00 - Kondula Vaclav
*/
#include "main.h"

using namespace std;
vector<Zone *> zones;

int main() {
  SetOutput("output.out");
  Init(TIME_BEGIN, TIME_END + 3 * HOUR);
  init_zones();
  (new Generator)->Activate();
  Run();
  return 0;
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

void Group::Behavior(){
    this->timestamps.push_back(Time); // Save arrival
    // Store * table = find_table();
    // if (!table){
    //     // TODO: odchazi nebo se rozdeluji
    //     std::cout << "/* Group didn't get table */" << std::endl;
    //     return;
    // }
    // Seize(TABLE, this->size);
    // Wait(TIME_TO_SIT);
    // Seize();


}
void Group::next_phase(){/*TODO*/}
