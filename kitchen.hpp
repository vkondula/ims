#ifndef KITCHEN_H
#define KITCHEN_H

#include "main.hpp"
using namespace std;

class Group;
class Kitchen;
class Statistics;
extern Statistics * stat;

extern Kitchen * kitchen;

class Cook : public Process {
  void prepare_meal(int ptr);
  void prepare_meal_wrapper();
  void prepare_soup();
  void Behavior();
public:
  Cook(){ }
};

class Kitchen {
  friend class Cook;
  vector<Cook *> cook;
  vector<int> meals;
  Queue q;
  int soups = 0;
  int soup_max;
  int meal_max;
  bool get_meals(Group * group);
  bool get_soups(Group * group);
public:
  Kitchen(int cooks);
  bool get_order_wrapper(Group * group);
  void activate_cooks();
};

#endif // KITCHEN_H
