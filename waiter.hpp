#ifndef WAITER_H
#define WAITER_H

#include "main.hpp"
using namespace std;

class Zone;
class Waiter;
class Group;
class Kitchen;
class Statistics;

extern Kitchen * kitchen;
extern Statistics * stat;

class Waiter : public Process {
  bool in_zone = false;
  Zone * zone;
  void Behavior();
  void move(bool to_kitchen, bool slow);
  void prepare(int size);
  bool is_in_zone();
public:
  Waiter(){ Activate(); }
  void set_zone(Zone * z);
  void handle_request(Group * group);
  Zone * get_zone();
};

#endif // WAITER_H
