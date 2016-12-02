#ifndef MAIN_H
#define MAIN_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <time.h>
#include <math.h>
#include "simlib.h"
#include "waiter.hpp"
#include "group.hpp"
using namespace std;

/* WORKING TIME DEFINITION */
const int MINUTE = 60;
const int HOUR = 60 * MINUTE;
const int TIME_BEGIN = 11 * HOUR;
const int TIME_END = 14 * HOUR;
const int PEAK_TIME_START = 12 * HOUR;
const int PEAK_TIME_END = 13 * HOUR;

/* OPERATIONS DURATIONS */

const int TIME_TO_CHANGE_ZONE = 10;

const int TIME_TO_SIT_L = 5;
const int TIME_TO_SIT_H = 15;
const int TIME_TO_SEARCH_ZONE_L = 10;
const int TIME_TO_SEARCH_ZONE_H = 15;

const int TIME_TO_ORDER_GENERAL = 8;
const int TIME_TO_ORDER_L = 2;
const int TIME_TO_ORDER_H = 7;

const int TIME_TO_CLEAN = 3;
const int TIME_TO_SERVE_MEAL = 4;

const int TIME_TO_EAT_MEAL_L = 5 * MINUTE;
const int TIME_TO_EAT_MEAL_H = 10 * MINUTE;
const int TIME_TO_EAT_SOUP_L = 3 * MINUTE;
const int TIME_TO_EAT_SOUP_H = 7 * MINUTE;

const int TIME_TO_PAY_L = 10;
const int TIME_TO_PAY_H = 20;

const int TIME_TO_KITCHEN = 10;
const int TIME_TO_TABLE = 5;

const double CHANCE_TO_LEAVE = 0.4;
const double CHANCE_TO_JOIN = 0.8;
const double CHANCE_TO_ORDER_DRINK = 0.15;



/* ENVIRONMENT DESCRIPTION */
// TODO

class Zone;
class Group;
class Waiter;

class Zone {
  vector<Waiter *> waiter;
  vector<Store *> table;
public:
  Zone(int waiter_count, vector<int> tables_sizes);
  void move();
  Store * find_table(int min_size, bool force);
  Queue q;
  Queue priority_q;
};

class Generator : public Event {
    void Behavior();
};

void init_zones();

#endif // MAIN_H
