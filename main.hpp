#ifndef MAIN_H
#define MAIN_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <time.h>
#include <math.h>
#include <fstream>
#include "simlib.h"
#include "waiter.hpp"
#include "group.hpp"
#include "kitchen.hpp"
#include "stats.hpp"
#include "external_lib/json.hpp"
using namespace std;

/* WORKING TIME DEFINITION */
const int MINUTE = 60;
const int HOUR = 60 * MINUTE;
const int TIME_BEGIN = 11 * HOUR;
const int TIME_END = 14 * HOUR;
const int PEAK_TIME_START = 12 * HOUR;
const int PEAK_TIME_END = 12 * HOUR + 30 * MINUTE;

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

const int TIME_TO_KITCHEN = 14;
const int TIME_TO_TABLE = 7;
const int TIME_TO_IN_KITCHEN_PER_SERVING = 5;

const int TIME_TO_PREPARE_MEAL = 23;
const int TIME_TO_PREPARE_SOUP = 8;

const double CHANCE_TO_LEAVE = 0.4;
const double CHANCE_TO_JOIN = 0.8;
const double CHANCE_TO_ORDER_DRINK = 0.15;

/* ENVIRONMENT DESCRIPTION */
const int MENUES = 4;
const int STACK_MEALS = 3;
const int STACK_SOUPS = 10;

class Zone;
class Group;
class Waiter;

class Zone {
  vector<Waiter *> waiter;
  vector<Store *> table;
  int id;
public:
  Zone(int waiter_count, vector<int> tables_sizes);
  ~Zone();
  Store * find_table(int min_size, bool force);
  Queue q;
  Queue priority_q;
  Stat waiter_waiting;
  void move();
  void waiter_no_queue(double duration);
  int get_id();
  int get_waiter_count();
};

class Generator : public Event {
    void Behavior();
};

nlohmann::json parse_arguments(int argc, char* argv[]);
void init_zones(nlohmann::json args);
void dealoc_global();

#endif // MAIN_H
