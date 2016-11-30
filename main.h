#ifndef MAIN_H
#define MAIN_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <time.h>
#include "simlib.h"

/* WORKING TIME DEFINITION */
const int MINUTE = 60;
const int HOUR = 60 * MINUTE;
const int TIME_BEGIN = 11 * HOUR;
const int TIME_END = 14 * HOUR;

/* OPERATIONS DURATIONS */
// TODO
const int TIME_TO_SIT_L = 5;
const int TIME_TO_SIT_H = 15;
const int TIME_TO_SEARCH_ZONE_L = 10;
const int TIME_TO_SEARCH_ZONE_H = 15;

const int TIME_TO_ORDER_GENERAL = 8;
const int TIME_TO_ORDER_L = 2;
const int TIME_TO_ORDER_H = 7;

const int TIME_TO_CLEAN = 3;
const int TIME_TO_SERVER_MEAL = 4;

const int TIME_TO_EAT_MEAL_L = 5 * MINUTE;
const int TIME_TO_EAT_MEAL_H = 10 * MINUTE;
const int TIME_TO_EAT_SOUP_L = 3 * MINUTE;
const int TIME_TO_EAT_SOUP_H = 7 * MINUTE;

const int TIME_TO_PAY_L = 10;
const int TIME_TO_PAY_H = 20;


/* ENVIRONMENT DESCRIPTION */

// TODO

enum tPhase : int {
    ARRIVE,
    LF_TABLE,
    WF_ORDER,
    WF_DRINK,
    WF_MEAL,
    WF_PAYMENT,
    LEAVING,
};
class Zone;

class Waiter : public Process {
  Zone * zone;
  void Behavior();
public:
  Waiter(){ Activate(); }
  void set_zone(Zone * z);
};

class Zone {
  std::vector<Waiter *> waiter;
  std::vector<Store *> table;
public:
  Zone(int waiter_count, std::vector<int> tables_sizes){
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
  Store * find_table(int min_size);
  Queue q;
};

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
    std::cout << "Group arrived at: " << Time << "\n";
  }
  Waiter * curr_waiter;
  Store find_table();
  Group split_group();
};

class Generator : public Event {
    void Behavior(){
        if(Time < TIME_END){
            Activate(Time + Exponential(2 * MINUTE)); //CHANGE!!
            new Group(3); //CHANGE!!
        }
    }
};

void init_zones();
Zone * find_zone_with_table(int size);

#endif // MAIN_H
