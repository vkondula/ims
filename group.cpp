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
  this->phase = p;
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
  if (this->get_table()) return true;
  this->find_zone_with_table(false);
  /* When zone with suitable table was found, the group owns the thread
     so they can aquire the table exclusivly
  */
  bool force = false;
  if (!this->get_zone()){
    /* Group didn't get free table, tries to join someone */
    force = true;
    cout << "/* Group didn't get free table*/" << endl;
    this->find_zone_with_table(true);
    if (!this->get_zone()){
      cout << "/* Group couldn't join any table*/" << endl;
      if (this->size < 5) return false; // Small group leaves right away
      if (Random() <= CHANCE_TO_LEAVE) return false; // Big group might leave
      /* Group spliting up and searches for tables */
      Group * second_g = this->group_split();
      this->find_zone_with_table(true);
      if(this->get_zone()) {
        // First group got table, aquires it
        this->set_table_in_zone(force);
        Enter(*this->get_table(), this->size);
        second_g->find_zone_with_table(true);
        if (second_g->get_zone()){
          // Second group got table as well, aquires it
          second_g->set_table_in_zone(force);
          Enter(*second_g->get_table(), second_g->size);
          second_g->Activate();
          cerr << "/* Second group OK */" << endl;
          return true;
        } else {
          // Second group did't get table, so first group leaves as well
          cerr << "/* Second group FAIL */" << endl;
          Leave(*this->get_table(), this->size);
          delete second_g;
          return false;
        }
      } else {
        // First group didn't get table so both groups leaves
        cerr << "/* FIRST group FAIL */" << endl;
        delete second_g;
        return false;
      }
    }
    cout << "/* Group joined taken table*/" << endl;
  }
  this->set_table_in_zone(force);
  Enter(*this->get_table(), this->size);
  return true;
}

void Group::find_zone_with_table(bool force){
  vector<Zone *> zones_g(zones); // Create copy of zone pointers
  random_shuffle(zones_g.begin(), zones_g.end()); //Walk in random order
  for (Zone * z : zones_g){
    if (this->timed)
      Wait(TIME_TO_CHANGE_ZONE);
    Store * s = z->find_table(this->size, force);
    if(s) {
      this->set_zone(z);
      return;
    }
  }
  this->set_zone(NULL);
  return;
}

void Group::quick_sit(bool t){
  this->timed = !t;
}

tPhase Group::get_phase(){
  return this->phase;
}

void Group::set_dependent_group(Group * g){
  this->dependent_group = g;
}

Group * Group::get_dependent_group(){
  return this->dependent_group;
}

Group * Group::group_split(){
  int original_size = this->size;
  int second_group_size = original_size/2;
  this->size = original_size/2 + original_size%2;
  Group * second_g = new Group(second_group_size);
  this->quick_sit(true);
  second_g->quick_sit(true);
  this->set_dependent_group(second_g);
  second_g->set_dependent_group(this);
  return second_g;
}

Store * Group::get_table(){
  return this->table;
}

Zone * Group::get_zone(){
  return this->zone;
}

void Group::set_zone(Zone * zone){
  this->zone = zone;
}

void Group::set_table_in_zone(bool force){
  this->table = this->get_zone()->find_table(this->size, force);
}
