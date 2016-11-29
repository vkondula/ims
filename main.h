#ifndef MAIN_H
#define MAIN_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include "simlib.h"

/* WORKING TIME DEFINITION */
const int MINUTE = 60;
const int HOUR = 60 * MINUTE;
const int TIME_BEGIN = 11 * HOUR;
const int TIME_END = 14 * HOUR;

/* OPERATIONS DURATIONS */
// TODO

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

class Zone {
    int waiter_count;
    Facility * waiter;
    std::vector<Store *> table;
    Store find_table(int min_size);
    public: Zone(int waiter_count, std::vector<int> tables_sizes){
        this->waiter_count = waiter_count;
        this->table.reserve(tables_sizes.size());
        this->waiter= new Facility[waiter_count];
        for (int i : tables_sizes){
            Store * t = new Store();
            t->SetCapacity(i);
            this->table.push_back(t);
        }
    }
};

class Group : public Process {
    int size;
    tPhase phase;
    Store table;
    std::vector<int> timestamps;
    Store find_table();
    Group split_group();
    void Behavior();
    void next_phase();
    public: Group(int size){
      this->size = size;
      this->timestamps.reserve(8);
      this->Activate();
      std::cout << "Group arrived at: " << Time << "\n";
    }
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

#endif // MAIN_H
