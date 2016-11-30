#include "group.hpp"

void Group::Behavior(){
  this->timestamps.push_back(Time); // Save arrival
  Zone * zone = find_zone_with_table(this->size);
  /* When zone with suitable table was found, the group owns the thread
     so they can aquire the table exclusivly
  */
  if (!zone){
      // TODO: odchazi nebo se rozdeluji
      std::cout << "/* Group didn't get table */" << std::endl;
      return;
  }
  Store * table = zone->find_table(this->size);
  Enter(*table, this->size);
  /* Taking a seat */
  Wait(Uniform(TIME_TO_SIT_L, TIME_TO_SIT_H));
  /* Waiting for soup and order */
  zone->q.Insert(this);
  Passivate();
  Wait(TIME_TO_ORDER_GENERAL + Uniform(TIME_TO_ORDER_L, TIME_TO_ORDER_H) * this->size);
  this->curr_waiter->Activate();
  /* Eating soup */
  std::cerr << "/* POLIVKA */" << std::endl;
  Wait(Uniform(TIME_TO_EAT_SOUP_L,TIME_TO_EAT_SOUP_H));
  /* Wating for waiter to take away dishes (soup) */
  zone->q.Insert(this);
  Passivate();
  Wait(TIME_TO_CLEAN * this->size);
  this->curr_waiter->Activate();
  /* Waiting for meal */
  zone->q.Insert(this);
  Passivate();
  Wait(TIME_TO_SERVER_MEAL * this->size);
  this->curr_waiter->Activate();
  /* Eating meal */
  std::cerr << "/* HLAVNI CHOD */" << std::endl;
  Wait(Uniform(TIME_TO_EAT_MEAL_L,TIME_TO_EAT_MEAL_H));
  /* Wating for waiter to take away dishes (meal) and pay */
  zone->q.Insert(this);
  Passivate();
  Wait(Uniform(TIME_TO_PAY_L, TIME_TO_PAY_H));
  Wait(TIME_TO_CLEAN * this->size);
  this->curr_waiter->Activate();
  /* Leaving the restaurant */
  std::cerr << "/* ODCHAZI */" << std::endl;
  Leave(*table, this->size);
}
