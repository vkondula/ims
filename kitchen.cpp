#include "kitchen.hpp"
using namespace std;

Kitchen::Kitchen(int cooks){
  for (int i = 0; i<cooks; i++){
    this->cook.push_back(new Cook());
  }
  this->meals.assign(MENUES, 0);
  this->meal_max = STACK_MEALS;
  this->soup_max = STACK_SOUPS;
}

bool Kitchen::get_meals(Group * group){
  vector<int>* order = group->get_order();
  bool enough = true;
  for (unsigned int i =0; i < order->size(); i++){
    if (this->meals[i] < (*order)[i]) enough = false;
  }
  if (enough){
    for (unsigned int i =0; i < order->size(); i++){
      this->meals[i] -= (*order)[i];
    }
    return true;
  }
  this->q.Insert(group);
  return false;
}

bool Kitchen::get_soups(Group * group){
  int count = group->get_group_size();
  if (this->soups >= count){
    this->soups -= count;
    return true;
  }
  this->q.Insert(group);
  return false;
}

bool Kitchen::get_order_wrapper(Group * group){
  if (group->get_phase() == WF_ORDER)
    return this->get_soups(group);
  else if (group->get_phase() == WF_MEAL)
    return this->get_meals(group);
  return true;
}

void Kitchen::activate_cooks(){
  for (Cook * cook : this->cook){
    cook->Activate();
  }
}

void Cook::Behavior(){
  while (true){
    if (!kitchen->q.Empty()){
      /* Some order was not satisfied, has priority */
      stat->add_kitchen_took_order(1);
      Group * group = (Group *)kitchen->q.GetFirst();
      if (group->get_phase() == WF_ORDER){
        /* Need more soup */
        int soups = kitchen->soups;
        kitchen->soups = 0;
        for(int i = soups; i <= group->get_group_size(); i++){
          this->prepare_soup();
          kitchen->soups--;
        }
        group->set_phase(PRIORITY_SOUP);
      } else {
        /* Need more meals */
        vector<int>* order = group->get_order();
        int missing = 0;
        for (unsigned int i = 0; i < order->size(); i++){
          kitchen->meals[i] -= (*order)[i];
          if (kitchen->meals[i] < 0){
            missing += - kitchen->meals[i];
            kitchen->meals[i] = 0;
          }
        }
        for (int i = 0; i<missing; i++){
          this->prepare_meal(0);
          kitchen->meals[0]--;
        }
        group->set_phase(PRIORITY_MEAL);
      }
      group->get_zone()->priority_q.Insert(group);
    } else if (kitchen->soups < kitchen->soup_max) {
      /* Stack for soups is not full */
      this->prepare_soup();
    } else {
      this->prepare_meal_wrapper();
    }
  }
}

void Cook::prepare_meal_wrapper(){
  vector<int>* meals = &(kitchen->meals);
  int min_index = -1;
  int min_value = kitchen->meal_max;
  for (unsigned int i = 0; i < meals->size(); i++){
    if ((*meals)[i] < min_value){
      min_index = i;
      min_value = (*meals)[i];
    }
  }
  if (min_index >= 0) this->prepare_meal(min_index);
  else {
    double time_wait = 10;
    if (Time < TIME_END)
      stat->add_cook_no_queue(time_wait);
    Wait(time_wait); // Wait for next order
  }
}

void Cook::prepare_meal(int ptr){
  Wait(TIME_TO_PREPARE_MEAL);
  kitchen->meals[ptr]++;
}

void Cook::prepare_soup(){
  Wait(TIME_TO_PREPARE_SOUP);
  kitchen->soups++;
}

int Kitchen::get_cook_count(){
  return this->cook.size();
}
