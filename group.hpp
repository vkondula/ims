#ifndef GROUP_H
#define GROUP_H

#include <algorithm>
#include "main.hpp"
using namespace std;


enum tPhase : int {
  ARRIVE,
  LF_TABLE,
  SITING,
  WF_ORDER,
  SOUP,
  WF_CLEAN,
  WF_MEAL,
  MEAL,
  WF_PAYMENT,
  PAYING,
  LEAVING,
  END_ENUM,
  PRIORITY_SOUP,
  PRIORITY_MEAL,
};

class Waiter;
class Zone;
class Group;
class Statistics;
extern vector<Zone *> zones;
extern Statistics * stat;

class Group : public Process {
  int size;
  bool timed;
  bool wf_drink = false;
  vector<double> timestamps;
  vector<int> order;
  tPhase phase;
  Zone * zone = NULL;
  Store * table = NULL;
  Group * dependent_group = NULL;
  void Behavior();
  void generate_order();
  void calculate_time();
  bool do_phase(tPhase p);
  bool lf_table();
  Group * group_split();
public:
  Group(int size){
    this->size = size;
    this->timestamps.reserve(END_ENUM * 2);
    this->quick_sit(false);
  }
  Waiter * curr_waiter;
  void quick_sit(bool t);
  void set_dependent_group(Group * g);
  void set_zone(Zone * z);
  void set_table_in_zone(bool force);
  void set_wf_drink(bool waits_for_drink);
  void set_curr_waiter(Waiter * w);
  void find_zone_with_table(bool force);
  void set_phase(tPhase p);
  bool is_wf_drink();
  int get_group_size();
  vector<int> * get_order();
  Store find_table();
  Group split_group();
  tPhase get_phase();
  Group * get_dependent_group();
  Zone * get_zone();
  Store * get_table();
};

#endif // GROUP_H
