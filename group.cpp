#include "group.hpp"
using namespace std;

void Group::Behavior(){
  for(int i = 0; i < END_ENUM; i++){
    if(!this->do_phase((tPhase)i)) break;
  }
}

bool Group::do_phase(tPhase p){
  this->set_phase(p);
  switch (p) {
    case ARRIVE:
      this->timestamps.push_back(Time); //Save Beginning
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
      this->get_zone()->q.Insert(this);
      this->timestamps.push_back(Time);
      Passivate();
      this->timestamps.push_back(Time);
      Wait(TIME_TO_ORDER_GENERAL + Uniform(TIME_TO_ORDER_L, TIME_TO_ORDER_H) * this->size);
      this->generate_order();
      /* Every person has 15% chance to order drink, event happens when at least one orders*/
      if (pow(1-CHANCE_TO_ORDER_DRINK, this->size) <= Random()){
        /* Asynchronously waiting for drink */
        this->set_wf_drink(true);
        this->get_zone()->q.Insert(this);
      }
      this->curr_waiter->Activate();
      break;
    case SOUP:
      /* Eating soup */
      Wait(Uniform(TIME_TO_EAT_SOUP_L,TIME_TO_EAT_SOUP_H));
      break;
    case WF_CLEAN:
      /* Wating for waiter to take away dishes (soup) */
      if (!this->is_wf_drink()) // Group is already once in queue
        this->get_zone()->q.Insert(this);
      this->timestamps.push_back(Time);
      Passivate();
      this->timestamps.push_back(Time);
      Wait(TIME_TO_CLEAN * this->size);
      this->curr_waiter->Activate();
      break;
    case WF_MEAL:
      /* Waiting for meal */
      this->get_zone()->q.Insert(this);
      this->timestamps.push_back(Time);
      Passivate();
      this->timestamps.push_back(Time);
      Wait(TIME_TO_SERVE_MEAL * this->size);
      this->curr_waiter->Activate();
      break;
    case MEAL:
      /* Eating meal */
      Wait(Uniform(TIME_TO_EAT_MEAL_L,TIME_TO_EAT_MEAL_H));
      break;
    case WF_PAYMENT:
      /* Wating for waiter to take away dishes (meal) and pay */
      this->get_zone()->priority_q.Insert(this);
      this->timestamps.push_back(Time);
      Passivate();
      this->timestamps.push_back(Time);
      break;
    case PAYING:
      /* Paying and taking away the dishes */
      Wait(Uniform(TIME_TO_PAY_L, TIME_TO_PAY_H));
      Wait(TIME_TO_CLEAN * this->size);
      this->curr_waiter->Activate();
      break;
    case LEAVING:
      /* Leaving the restaurant */
      this->timestamps.push_back(Time);
      this->calculate_time();
      if (this->get_dependent_group()){
        if (this->get_dependent_group()->get_phase() == LEAVING){
          /* Ping second group so they can leave together */
          this->get_dependent_group()->Activate();
        } else {
          /* Wait for second group to finish lunch */
          Passivate();
        }
      }
      Leave(*this->get_table(), this->size);
      break;
    default:
      break;
  }
  return true;
}

bool Group::lf_table(){
  if (this->get_table()) return true;
  stat->add_group_count(this->size);
  this->find_zone_with_table(false);
  /* When zone with suitable table was found, the group owns the thread
     so they can aquire the table exclusivly
  */
  bool force = false;
  if (!this->get_zone()){
    /* Group didn't get free table, tries to join someone */
    stat->add_group_no_own_table(this->size);
    if (Random() >= CHANCE_TO_JOIN) return false; // Group leaves immediately
    force = true;
    this->find_zone_with_table(true);
    if (!this->get_zone()){
      /* Group couldn't join any table*/
      stat->add_group_join_failed(this->size);
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
          stat->add_group_splited(this->size);
          second_g->set_table_in_zone(force);
          Enter(*second_g->get_table(), second_g->size);
          second_g->Activate();
          return true;
        } else {
          // Second group did't get table, so first group leaves as well
          stat->add_group_split_failed(this->size);
          Leave(*this->get_table(), this->size);
          delete second_g;
          return false;
        }
      } else {
        // First group didn't get table so both groups leaves
        stat->add_group_split_failed(this->size);
        delete second_g;
        return false;
      }
    }
    /* Group joined taken table*/
    stat->add_group_joined(this->size);
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

void Group::set_wf_drink(bool waits_for_drink){
  this->wf_drink = waits_for_drink;
}

void Group::set_curr_waiter(Waiter * w){
  this->curr_waiter = w;
}

bool Group::is_wf_drink(){
  return this->wf_drink;
}

int Group::get_group_size(){
  return this->size;
}

void Group::generate_order(){
  this->order.assign(MENUES, 0);
  for (int i = 0; i < this->size; i++){
    this->order[(int)(Random() * MENUES)]++;
  }
}

vector<int> * Group::get_order(){
  return &(this->order);
}

void Group::set_phase(tPhase p){
  this->phase = p;
}

void Group::calculate_time(){
  double time_wait = 0;
  for (int i = 1; i < 9; i=i+2){
    time_wait += this->timestamps[i+1] - this->timestamps[i];
  }
  double time_total = this->timestamps[9] - this->timestamps[0];
  stat->add_time_in_system(time_total);
  stat->add_waiting(time_wait);
}
