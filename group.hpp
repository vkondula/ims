#ifndef GROUP_H
#define GROUP_H

#include "main.hpp"

enum tPhase : int {
  ARRIVE,
  LF_TABLE,
  WF_ORDER,
  WF_DRINK,
  WF_MEAL,
  WF_PAYMENT,
  LEAVING,
};

class Waiter;

class Group : public Process {
  int size;
  tPhase phase;
  Store table;
  std::vector<int> timestamps;
  void Behavior();
public:
  Group(int size){
    this->size = size;
    this->timestamps.reserve(8);
    this->Activate();
    std::cout << "Group with "<< size <<" members arrived at: " << Time << "\n";
  }
  Waiter * curr_waiter;
  Store find_table();
  Group split_group();
};

#endif // GROUP_H
