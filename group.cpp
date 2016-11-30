#include "group.hpp"
using namespace std;

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
      /* Looking for table */
      return this->lf_table();
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
      cerr << "/* POLIVKA */" << endl;
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
      cerr << "/* HLAVNI CHOD */" << endl;
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
      cerr << "/* ODCHAZI */" << endl;
      Leave(*this->table, this->size);
      break;
    default:
      break;
  }
  return true;
}

bool Group::lf_table(){
  this->zone = find_zone_with_table(false);
  /* When zone with suitable table was found, the group owns the thread
     so they can aquire the table exclusivly
  */
  bool force = false;
  if (!this->zone){
      // TODO: odchazi nebo se rozdeluji
      cout << "/* Group didn't get free table*/" << endl;
      this->zone = find_zone_with_table(this->size);
      if (!this->zone){
        cout << "/* Group couldn't join any table*/" << endl;
        return false;
      }
      force = true;
      cout << "/* Group joined taken table*/" << endl;
  }
  this->table = this->zone->find_table(this->size, force);
  Enter(*this->table, this->size);
  return true;
}

Zone * Group::find_zone_with_table(bool force){
  vector<Zone *> zones_g(zones); // Create copy of zone pointers
  random_shuffle(zones_g.begin(), zones_g.end()); //Walk in random order
  for (Zone * z : zones_g){
    Wait(TIME_TO_CHANGE_ZONE);
    Store * s = z->find_table(this->size, force);
    if(s) return z;
  }
  return NULL;
}
