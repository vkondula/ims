#include "waiter.hpp"
using namespace std;

void Waiter::Behavior(){
  while (true){
      Queue * q = NULL;
      if (!this->get_zone()->priority_q.Empty()) q = &(this->get_zone()->priority_q);
      else if (!this->get_zone()->q.Empty()) q = &(this->get_zone()->q);
      else {
        /* Wait for next request */
        Wait(10);
        continue;
      }
      Group * group = (Group *) q->GetFirst();
      this->handle_request(group);
  }
}

void Waiter::set_zone(Zone * z){
  this->zone = z;
}

Zone * Waiter::get_zone(){
  return this->zone;
}


bool Waiter::is_in_zone(){
  return this->in_zone;
}

void Waiter::move(bool kitchen, bool slow=false){
  int duration = (kitchen ? TIME_TO_KITCHEN : TIME_TO_TABLE);
  if (slow) duration = (int)(duration * 1.5);
  if (kitchen) this->in_zone = !(this->in_zone);
  Wait(duration);
}

void Waiter::handle_request(Group * group){
  tPhase phase = group->get_phase();
  if (phase == WF_CLEAN && group->is_wf_drink()){
    /* Merge of bringing drink and cleaning plates in one */
    phase=END_ENUM;
  }
  switch (phase) {
    case WF_ORDER:
    case WF_MEAL:
    case SOUP:
    case END_ENUM:
      /* Need to bring something from kitchen (food / dring)*/
      if (this->is_in_zone()) this->move(true); // Go to kitchen (fast)
      this->move(true, true); // Go back with plates (slow)
      this->move(false, true); // Go to the table with plates (slow)
      break;
    case WF_CLEAN:
    case WF_PAYMENT:
      /* Requires just the waiter */
      if (!this->is_in_zone()) this->move(true); // Go to zone (fast)
      this->move(false); // Go to the table (fast)
      break;
    default:
      break;
  }
  if (group->is_wf_drink()){
    //only asynchronous action, no need for group co-operation
    group->set_wf_drink(false);
    Wait(TIME_TO_SERVE_MEAL);
    std::cerr << "/* DRINK SERVED */"<< phase << std::endl;
    return;
  } else {
    // These actions are handled by Group
    group->set_curr_waiter(this);
    group->Activate();
    Passivate();
  }
  switch (phase) {
    case WF_CLEAN:
    case WF_PAYMENT:
    case END_ENUM:
      /* Need to take away something to kitchen (dishes) */
      this->move(true, true);
      break;
    default:
      /* Leave table */
      Wait(2);
      break;
  }
}
