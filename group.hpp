#ifndef GROUP_H
#define GROUP_H

#include "main.hpp"

enum tPhase : int {
  ARRIVE,
  LF_TABLE,
  SITING,
  WF_ORDER,
  SOUP,
  // WF_DRINK,
  WF_CLEAN,
  WF_MEAL,
  MEAL,
  WF_PAYMENT,
  PAYING,
  LEAVING,
  END_ENUM,
};

class Waiter;
class Zone;

class Group : public Process {
  int size;
  tPhase phase;
  std::vector<int> timestamps;
  Zone * zone;
  Store * table;
  void Behavior();
  bool set_phase(tPhase p);
public:
  Group(int size){
    this->size = size;
    this->timestamps.reserve(END_ENUM * 2);
    this->Activate();
    std::cout << "Group with "<< size <<" members arrived at: " << Time << "\n";
  }
  Waiter * curr_waiter;
  Store find_table();
  Group split_group();
};

#endif // GROUP_H
