#ifndef STATS_H
#define STATS_H

#include "main.hpp"
using namespace std;

class Statistics {
  Stat group_count; // All groups that arrived
  Stat group_no_own_table; // Failed to get own table
  Stat group_joined; // Joined taken table
  Stat group_join_failed; // Attemp to join table failed
  Stat group_splited; // Splited succesfully
  Stat group_split_failed;
  Stat kitchen_took_order; // Order was not satisfied
  Stat waiting; // waiting for waiter (not eating or anything)
  Stat time_in_system;
  Stat waiter_no_queue; // COUNT ONLY WHILE GROUPS ARE COMING!
public:
  Statistics() : waiting("Čas čekání"), time_in_system("Celkový čas v restauraci"){}
  void Output();
  /* group numbers */
  void add_group_count(int group_size);
  void add_group_no_own_table(int group_size);
  void add_group_joined(int group_size);
  void add_group_join_failed(int group_size);
  void add_group_splited(int group_size);
  void add_group_split_failed(int group_size);
  void add_kitchen_took_order(int group_order);
  /* time info */
  void add_time_in_system(double duration);
  void add_waiting(double duration);
  void add_waiter_no_queue(double duration);
};

int minutes(int value);
int seconds(int value);
#endif // STATS_H
