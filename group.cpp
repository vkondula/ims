#include "group.hpp"

void Group::Behavior(){
  for(int i = 0; i < END_ENUM; i++){
    this->timestamps.push_back(Time); //Save Beginning
    if(!this->set_phase((tPhase)i)) break;
    this->timestamps.push_back(Time); //Save End
  }
}

bool Group::set_phase(tPhase p){
  switch (p) {
    case ARRIVE:
      break;
    case LF_TABLE:
      this->zone = find_zone_with_table(this->size);
      /* When zone with suitable table was found, the group owns the thread
         so they can aquire the table exclusivly
      */
      if (!this->zone){
          // TODO: odchazi nebo se rozdeluji
          std::cout << "/* Group didn't get table */" << std::endl;
          return false;
      }
      this->table = this->zone->find_table(this->size);
      Enter(*this->table, this->size);
      break;
    case SITING:
      /* Taking a seat */
      Wait(Uniform(TIME_TO_SIT_L, TIME_TO_SIT_H));
      break;
    case WF_ORDER:
      /* Waiting for soup and order */
      this->zone->q.Insert(this);
      Passivate();
      Wait(TIME_TO_ORDER_GENERAL + Uniform(TIME_TO_ORDER_L, TIME_TO_ORDER_H) * this->size);
      this->curr_waiter->Activate();
      break;
    case SOUP:
      /* Eating soup */
      std::cerr << "/* POLIVKA */" << std::endl;
      Wait(Uniform(TIME_TO_EAT_SOUP_L,TIME_TO_EAT_SOUP_H));
      break;
    case WF_CLEAN:
      /* Wating for waiter to take away dishes (soup) */
      this->zone->q.Insert(this);
      Passivate();
      Wait(TIME_TO_CLEAN * this->size);
      this->curr_waiter->Activate();
      break;
    case WF_MEAL:
      /* Waiting for meal */
      this->zone->q.Insert(this);
      Passivate();
      Wait(TIME_TO_SERVER_MEAL * this->size);
      this->curr_waiter->Activate();
      break;
    case MEAL:
      /* Eating meal */
      std::cerr << "/* HLAVNI CHOD */" << std::endl;
      Wait(Uniform(TIME_TO_EAT_MEAL_L,TIME_TO_EAT_MEAL_H));
      break;
    case WF_PAYMENT:
      /* Wating for waiter to take away dishes (meal) and pay */
      this->zone->q.Insert(this);
      Passivate();
      break;
    case PAYING:
      /* Paying and taking away the dishes */
      Wait(Uniform(TIME_TO_PAY_L, TIME_TO_PAY_H));
      Wait(TIME_TO_CLEAN * this->size);
      this->curr_waiter->Activate();
      break;
    case LEAVING:
      /* Leaving the restaurant */
      std::cerr << "/* ODCHAZI */" << std::endl;
      Leave(*this->table, this->size);
      break;
    default:
      break;
  }
  return true;
}
