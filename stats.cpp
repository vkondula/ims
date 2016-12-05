#include "stats.hpp"
using namespace std;

void Statistics::Output(){
  if (this->group_count.Number() == 0) return; // division by zero
  Print("+------------------------------------------------------------------+\n");
  Print("| Zákaznící                                                        |\n");
  Print("+------------------------------------------------------------------+\n");
  int gc, gn; // can not be as Print argument
  int group_sum =  this->group_count.Sum();
  int group_num =  this->group_count.Number();
  Print("Celkový počet zákazníků: %d (skupin: %d)\n", group_sum, group_num);
  /* own table */
  int own_table_cust = group_sum - this->group_no_own_table.Sum();
  int own_table_groups = group_num - this->group_no_own_table.Number();
  double own_table_perc = (double)own_table_cust / group_sum * 100;
  /* combined */
  int served_cust = own_table_cust + this->group_joined.Sum() + this->group_splited.Sum();
  int served_groups = own_table_groups + this->group_joined.Number() + this->group_splited.Number();
  double served_perc = (double)served_cust / group_sum * 100;
  /* joined and split percentage */
  double joined_perc = (double)this->group_joined.Sum() / group_sum * 100;
  double split_perc= (double)this->group_splited.Sum() / group_sum * 100;
  Print("  Počet obsloužených zákazníků: %d (skupin: %d) (%.2f %%)\n", served_cust, served_groups, served_perc);
  Print("    Vlastní stůl: %d (skupin: %d) (%.2f %%)\n", own_table_cust, own_table_groups, own_table_perc);
  gc = this->group_joined.Sum();
  gn =  this->group_joined.Number();
  Print("    Přisedli si: %d (skupin: %d) (%.2f %%)\n", gc, gn, joined_perc);
  gc = this->group_splited.Sum();
  gn = this->group_splited.Number();
  Print("    Rozdělili se: %d (skupin: %d) (%.2f %%)\n\n", gc, gn, split_perc);
  /* Failed to join */
  int failed_join_cust = this->group_join_failed.Sum() - this->group_splited.Sum() - this->group_split_failed.Sum();
  int failed_join_groups = this->group_join_failed.Number() - this->group_splited.Number() - this->group_split_failed.Number();
  double failed_join_perc = (double)failed_join_cust / group_sum * 100;
  /* Failed to get own */
  int failed_own_cust = this->group_no_own_table.Sum() - this->group_joined.Sum() - this->group_join_failed.Sum();
  int failed_own_groups = this->group_no_own_table.Number() - this->group_joined.Number() - this->group_join_failed.Number();
  double failed_own_perc = (double)failed_own_cust / group_sum * 100;
  /* fail to split */
  double failed_to_split_perc = (double)this->group_split_failed.Sum() / group_sum * 100;
  /* combined */
  int not_served_cust = failed_join_cust + failed_own_cust + this->group_split_failed.Sum();
  int not_served_groups = failed_join_groups + failed_own_groups + this->group_split_failed.Number();
  double not_served_perc = (double)not_served_cust / group_sum * 100;
  Print("  Počet neobsloužených zákazníků: %d (skupin: %d) (%.2f %%)\n", not_served_cust, not_served_groups, not_served_perc);
  Print("    Nedostali vlastní stůl: %d (skupin: %d) (%.2f %%)\n", failed_own_cust, failed_own_groups, failed_own_perc);
  Print("    Nemohli si přisednout: %d (skupin: %d) (%.2f %%)\n", failed_join_cust, failed_join_groups, failed_join_perc);
  gc = this->group_split_failed.Sum();
  gn = this->group_split_failed.Number();
  Print("    Nemohli si přisendout ani kdyby se rozdělili: %d (skupin: %d) (%.2f %%)\n\n", gc, gn, failed_to_split_perc);
  Print("+------------------------------------------------------------------+\n");
  Print("| Časové záznamy                                                   |\n");
  Print("+------------------------------------------------------------------+\n");
  int min_s = seconds((int)this->time_in_system.Min());
  int min_m = minutes((int)this->time_in_system.Min());
  int max_s = seconds((int)this->time_in_system.Max());
  int max_m = minutes((int)this->time_in_system.Max());
  int avg = this->time_in_system.Sum() / this->time_in_system.Number();
  int avg_s = seconds(avg);
  int avg_m = minutes(avg);
  Print("Čas ztrávený v restauraci\n");
  Print("  Minimální %d:%02dmin\n", min_m, min_s);
  Print("  Maximální %d:%02dmin\n", max_m, max_s);
  Print("  Průměrně %d:%02dmin\n", avg_m, avg_s);
  min_s = seconds((int)this->waiting.Min());
  min_m = minutes((int)this->waiting.Min());
  max_s = seconds((int)this->waiting.Max());
  max_m = minutes((int)this->waiting.Max());
  avg = this->waiting.Sum() / this->waiting.Number();
  avg_s = seconds(avg);
  avg_m = minutes(avg);
  Print("Čas čekání na číšníka\n");
  Print("  Minimální %d:%02dmin\n", min_m, min_s);
  Print("  Maximální %d:%02dmin\n", max_m, max_s);
  Print("  Průměrně %d:%02dmin\n\n", avg_m, avg_s);
  Print("+------------------------------------------------------------------+\n");
  Print("| Práce zaměstnanců                                                |\n");
  Print("+------------------------------------------------------------------+\n");
  gn = this->kitchen_took_order.Number();
  Print("Kuchyně\n");
  Print("  Počet kuchařů: %d\n", (int)kitchen->get_cook_count());
  Print("  Objednávky nepřipravené na čas: %d\n", gn);
  avg = this->cook_no_queue.Sum();
  avg_s = seconds(avg);
  avg_m = minutes(avg);
  Print("  Celkový čas volna: %d:%02dmin\n", avg_m, avg_s);
  avg = this->cook_no_queue.Sum() / (int)kitchen->get_cook_count();
  avg_s = seconds(avg);
  avg_m = minutes(avg);
  Print("  Čas volna na kuchaře: %d:%02dmin\n", avg_m, avg_s);
  for (Zone * z : zones){
    avg = z->waiter_waiting.Sum() / z->get_waiter_count();
    avg_s = seconds(avg);
    avg_m = minutes(avg);
    Print("Zóna číslo: %d\n", z->get_id());
    Print("  Počet číšníků: %d\n", z->get_waiter_count());
    Print("  Celkový čas volna: %d:%02dmin\n", avg_m, avg_s);
    Print("  Čas volna na číšníka: %d:%02dmin\n", avg_m, avg_s);
  }

}

void Statistics::add_group_count(int group_size){
  this->group_count(group_size);
}

void Statistics::add_group_no_own_table(int group_size){
  this->group_no_own_table(group_size);
}

void Statistics::add_group_joined(int group_size){
  this->group_joined(group_size);
}

void Statistics::add_group_join_failed(int group_size){
  this->group_join_failed(group_size);
}

void Statistics::add_group_splited(int group_size){
  this->group_splited(group_size);
}

void Statistics::add_group_split_failed(int group_size){
  this->group_split_failed(group_size);
}

void Statistics::add_kitchen_took_order(int group_order){
  this->kitchen_took_order(group_order);
}

void Statistics::add_time_in_system(double duration){
  this->time_in_system(duration);
}

void Statistics::add_waiting(double duration){
  this->waiting(duration);
}

void Statistics::add_cook_no_queue(double duration){
  this->cook_no_queue(duration);
}

void Statistics::add_waiter_no_queue(Zone * z, double duration){
  z->waiter_no_queue(duration);
}

int minutes(int value){
  return value / 60;
}

int seconds(int value){
  return value % 60;
}
