#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <vector>
#include <cmath>

#include <iostream>
#include <cstdlib>

#include "terrain_generation.h"
#include "world_generation.h"

bool item::operator==(const item &i) const {
  return i.item_ID == item_ID;
}

int battle::determine_battle() {

  int temp = rand() % 100;

  if(temp >= 90) {
    return 1;
  }

  return 0;
}

void battle::engage_battle_wild(std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, std::vector<pokemon_types> pkmn_typ, int manhattan_x, int manhattan_y) {

  in_game_pokemon to_spawn = generate_pokemon(pkmn_list, pkmn_st, pkmn_mv, mv, pkmn_typ, manhattan_x, manhattan_y);

  clear();
  char buffer1[100];
  sprintf(buffer1, "POKEMON: %s", to_spawn.get_name().c_str());
  char buffer2[100];
  sprintf(buffer2, "LEVEL: %d", to_spawn.get_level());
  char buffer3[100];
  sprintf(buffer3, "HP: %d", to_spawn.get_HP());
  char buffer4[100];
  sprintf(buffer4, "ATTACK: %d", to_spawn.get_attack());
  char buffer5[100];
  sprintf(buffer5, "DEFENSE: %d", to_spawn.get_defense());
  char buffer6[100];
  sprintf(buffer6, "SPECIAL ATTACK: %d", to_spawn.get_special_attack());
  char buffer7[100];
  sprintf(buffer7, "SPECIAL DEFENSE: %d", to_spawn.get_special_defense());
  char buffer8[100];
  sprintf(buffer8, "SPEED: %d", to_spawn.get_speed());
  char buffer9[100];
  sprintf(buffer9, "MOVE 1: %s", to_spawn.get_move_1().c_str());
  char buffer10[100];
  sprintf(buffer10, "MOVE 2: %s", to_spawn.get_move_2().c_str());
  char buffer11[100];
  sprintf(buffer11, "SHINY: %s", to_spawn.get_shiny().c_str());
  char buffer12[100];
  sprintf(buffer12, "GENDER: %s", to_spawn.get_gender().c_str());
  mvaddstr(5, 20, buffer1);
  mvaddstr(7, 20, buffer2);
  mvaddstr(8, 20, buffer3);
  mvaddstr(9, 20, buffer4);
  mvaddstr(10, 20, buffer5);
  mvaddstr(11, 20, buffer6);
  mvaddstr(12, 20, buffer7);
  mvaddstr(13, 20, buffer8);
  mvaddstr(15, 20, buffer9);
  mvaddstr(16, 20, buffer10);
  mvaddstr(10, 50, buffer11);
  mvaddstr(11, 50, buffer12);
  refresh();

  int stay = 1;

  while(stay) {

    if(getchar() == 27) {
      if(getchar() == 27) {
	stay = 0;
      }
    }
  }
  
}

void battle::get_types(in_game_pokemon &pkmn, std::vector<pokemon_types> pkmn_typ) {

  for(int i = 0; i < pkmn_typ.size(); i++) {
    if(pkmn.get_id() == stoi(pkmn_typ[i].pokemon_id)) {
      pkmn.add_type(stoi(pkmn_typ[i].type_id));
    }						    
  }
  
}

in_game_pokemon battle::generate_pokemon(std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, std::vector<pokemon_types> pkmn_typ, int manhattan_x, int manhattan_y) {

  in_game_pokemon to_spawn;
  choose_random_pokemon(to_spawn, pkmn_list);
  get_pokemon_stats(to_spawn, pkmn_st);
  assign_ivs(to_spawn);
  level_up(to_spawn, manhattan_x, manhattan_y);
  get_pokemon_moves(to_spawn, pkmn_mv);
  resolve_pokemon_move_names(to_spawn, mv);
  get_types(to_spawn, pkmn_typ);

  int random = rand() % 8192;

  if(random == 0) {
    to_spawn.set_shiny("Shiny");
  }

  random = rand() % 2;

  if(random == 0) {
    to_spawn.set_gender("Male");
  }
  else if(random == 1) {
    to_spawn.set_gender("Female");
  }

  return to_spawn;
  
}

void battle::choose_random_pokemon(in_game_pokemon &pkmn, std::vector<pokemon> pkmn_list) {

  int chosen_index = rand() % pkmn_list.size();
  pokemon selected_pkmn = pkmn_list[chosen_index];

  pkmn.set_id(stoi(selected_pkmn.id));
  pkmn.set_name(selected_pkmn.identifier);
  
}

void battle::get_pokemon_stats(in_game_pokemon &pkmn, std::vector<pokemon_stats> pkmn_st) {
  std::vector<pokemon_stats> selected_stats;
   
  for(int i = 0; i < pkmn_st.size(); i++) {
    if(pkmn.get_id() == stoi(pkmn_st[i].pokemon_id)) {
      selected_stats.push_back(pkmn_st[i]);
    }
  }

  // We should be parsing the stats in this order
  pkmn.set_HP(stoi(selected_stats[0].base_stat));
  pkmn.set_attack(stoi(selected_stats[1].base_stat));
  pkmn.set_defense(stoi(selected_stats[2].base_stat));
  pkmn.set_special_attack(stoi(selected_stats[3].base_stat));
  pkmn.set_special_defense(stoi(selected_stats[4].base_stat));
  pkmn.set_speed(stoi(selected_stats[5].base_stat));
}

void battle::get_pokemon_moves(in_game_pokemon &pkmn, std::vector<pokemon_moves> pkmn_mv) {
  std::vector<pokemon_moves> selected_moves;

  // Look among the most common version_ids to find moves. There's still a chance we won't find some, but looking among the most common version_ids,
  // You really should

  int chosen_version_id = 15;

  while(selected_moves.size() == 0
	&& chosen_version_id < 21) {
    
    for(int i = 0; i < pkmn_mv.size(); i++) {
      
      if(stoi(pkmn_mv[i].version_group_id) == chosen_version_id
	 && stoi(pkmn_mv[i].pokemon_id) == pkmn.get_id()
	 && pkmn.get_level() >= stoi(pkmn_mv[i].level)
	 && stoi(pkmn_mv[i].pokemon_move_method_id) == 1) {
	selected_moves.push_back(pkmn_mv[i]);
      }
      
    }
    
    chosen_version_id++;
  }

  int moves_to_learn = selected_moves.size();

  if(moves_to_learn == 0) {

    // Instead of crashing, let's just not assign any moves if we somehow don't find any
  }
  else if(moves_to_learn == 1) {
    pkmn.set_move_id_1(stoi(selected_moves[0].move_id));
  }

  else if(moves_to_learn >= 2) {
    int rand_move_1;
    int rand_move_2;

    rand_move_1 = rand() % selected_moves.size();
    pkmn.set_move_id_1(stoi(selected_moves[rand_move_1].move_id));

    
    rand_move_2 = rand() % selected_moves.size();

    // I'm only selecting from one moveset ID, so just checking if we generated the same number should be fine
    while(rand_move_2 == rand_move_1) {
      rand_move_2 = rand() % selected_moves.size();
    }
    
    pkmn.set_move_id_2(stoi(selected_moves[rand_move_2].move_id));
  }

    
}

void battle::assign_ivs(in_game_pokemon &pkmn) {

  int rand_iv;

  rand_iv = rand() % 15;
  pkmn.set_HP_iv(rand_iv);
  rand_iv = rand() % 15;
  pkmn.set_attack_iv(rand_iv);
  rand_iv = rand() % 15;
  pkmn.set_defense_iv(rand_iv);
  rand_iv = rand() % 15;
  pkmn.set_special_attack_iv(rand_iv);
  rand_iv = rand() % 15;
  pkmn.set_special_defense_iv(rand_iv);
  rand_iv = rand() % 15;
  pkmn.set_speed_iv(rand_iv);
  
}

int battle::generate_HP_lv_up(int base_HP, int HP_iv, int level) {

  int calc_1 = (base_HP + HP_iv) * 2 * level;
  double calc_2 = calc_1 / 100.0;
  calc_2 = floor(calc_2);
  int calc_3 = ((int) calc_2) + level + 10;
  

  return calc_3;
  
}

int battle::generate_otherstat_lv_up(int base_stat, int base_iv, int level) {

  int calc_1 = (base_stat + base_iv) * 2 * level;
  double calc_2 = calc_1 / 100.0;
  calc_2 = floor(calc_2);
  int calc_3 = ((int) calc_2) + 5;
  

  return calc_3;
  
}


void battle::level_up(in_game_pokemon &pkmn, int manhattan_x, int manhattan_y) {

  int abs_man_x = abs(manhattan_x);
  int abs_man_y = abs(manhattan_y);
  int distance = abs_man_x + abs_man_y;

  if((distance) <= 200) {
    int min = 1;
    int max;
    if(distance == 0) {
      max = 1;
    }
    else if(distance != 0) {
      max = (int) distance / 2;
    }
    pkmn.set_level((rand() % (max - min + 1)) + min);
  }
  else if((distance) > 200) {
    int min = (int) (distance - 200) / 2;
    int max = 100;
    pkmn.set_level(((rand() % (max - min + 1)) + min));
  }

  int to_assign;

  to_assign = generate_HP_lv_up(pkmn.get_HP(), pkmn.get_HP_iv(), pkmn.get_level());
  pkmn.set_HP(to_assign);
  pkmn.set_curr_HP(to_assign);

  to_assign = generate_otherstat_lv_up(pkmn.get_attack(), pkmn.get_attack_iv(), pkmn.get_level());
  pkmn.set_attack(to_assign);
  
  to_assign = generate_otherstat_lv_up(pkmn.get_defense(), pkmn.get_defense_iv(), pkmn.get_level());
  pkmn.set_defense(to_assign);
  
  to_assign = generate_otherstat_lv_up(pkmn.get_special_attack(), pkmn.get_special_attack_iv(), pkmn.get_level());
  pkmn.set_special_attack(to_assign);
  
  to_assign = generate_otherstat_lv_up(pkmn.get_special_defense(), pkmn.get_special_defense_iv(), pkmn.get_level());
  pkmn.set_special_defense(to_assign);
  
  to_assign = generate_otherstat_lv_up(pkmn.get_speed(), pkmn.get_speed_iv(), pkmn.get_level());
  pkmn.set_speed(to_assign);
}

void battle::resolve_pokemon_move_names(in_game_pokemon &pkmn, std::vector<moves> mv) {

  if(pkmn.get_move_id_1() != -1) {
    for(int i = 0; i < mv.size(); i++) {
      if(pkmn.get_move_id_1() == stoi(mv[i].id)) {
	pkmn.set_move_1(mv[i].identifier);
      }
    }
  }

  if(pkmn.get_move_id_2() != -1) {
    for(int i = 0; i < mv.size(); i++) {
      if(pkmn.get_move_id_2() == stoi(mv[i].id)) {
	pkmn.set_move_2(mv[i].identifier);
      }
    }
  }
  
}

void characterLogic::engage_battle(PC_state &PC_s, int manhattan_x, int manhattan_y, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, std::vector<pokemon_types> pkmn_typ) {

  clear();

  battle NPC_pokemon_battle;

  in_game_pokemon generated = NPC_pokemon_battle.generate_pokemon(pkmn_list, pkmn_st, pkmn_mv, mv, pkmn_typ, manhattan_x, manhattan_y);

  int current_PC_HP = PC_s.getPokemon()[0].get_curr_HP();
  int current_NPC_HP = generated.get_curr_HP();

  char buffer1[50];
  char buffer2[50];
  char buffer3[50];
  char buffer4[50];

  int stay = 1;
  char pressed_key;

  int PC_move_id = -1;

  while(stay) {

    PC_move_id = -1;

    clear();
    
    sprintf(buffer1, "NPC Pokemon: %s", generated.get_name().c_str());
    sprintf(buffer2, "Your Pokemon: %s", PC_s.getPokemon()[0].get_name().c_str());
    sprintf(buffer3, "HP: %d / %d", current_NPC_HP, generated.get_curr_HP());
    sprintf(buffer4, "HP: %d / %d", current_PC_HP, PC_s.getPokemon()[0].get_curr_HP());

    mvaddstr(2, 15, buffer1);
    mvaddstr(3, 15, buffer3);
    mvaddstr(17, 50, buffer2);
    mvaddstr(18, 50, buffer4);
    mvaddstr(17, 15, "1: FIGHT");
    mvaddstr(18, 15, "2: BAG");
    mvaddstr(19, 15, "3: POKEMON");
  
    refresh();
    pressed_key = getch();

    switch (pressed_key) {

    case '1':

      PC_move_id = NPC_pokemon_battle.process_move(PC_s);
      break;

    case '2':
      
      PC_move_id = NPC_pokemon_battle.enter_bag_NPC_battle(PC_s);
      break;

    case '3':
      
      PC_move_id = NPC_pokemon_battle.view_pokemon_in_battle(PC_s);
      break;
      
    }

    if(PC_move_id != -1) {
      
      NPC_pokemon_battle.process_attacks(PC_s, generated, mv, PC_move_id, &current_PC_HP, &current_NPC_HP);

    }

    if(current_NPC_HP <= 0) {
      clear();
      mvaddstr(10, 45, "NPC loses!");
      stay = 0;
      refresh();
      sleep(1);
    }
    else if(current_PC_HP <= 0) {
      clear();
      mvaddstr(10, 45, "PC loses!");
      stay = 0;
      refresh();
      sleep(1);
    }
    
  }
}

int battle::enter_bag_NPC_battle(PC_state &PC_s) {

  std::vector<item> bag_copy;

  for(int i = 0; i < PC_s.get_items().size(); i++) {

    if(PC_s.get_items()[i].get_item_ID() != 3) {
      bag_copy.push_back(PC_s.get_items()[i]);
    }
    
  }

  char pressed_key;
  int position = 0;
  
  while(1) {
    clear();
    print_bag(bag_copy, 1, position / 10);
    mvaddstr((position % 10) + 3, 8, ">");
    refresh();

    pressed_key = getch();

    if (pressed_key == '2') {
      if(position + 1 < bag_copy.size()) {
	position++;
      }
    }

    if (pressed_key == '8') {
      if(position - 1 >= 0) {
	position--;
      }
    }

    if (pressed_key == 27) {
      return -1;
    }
    
  }
  
}

void battle::print_bag(std::vector<item> bag_copy, int in_battle, int window) {

  int to_print_final;
  int pages;
  int total;
  
  to_print_final = bag_copy.size() % 10;
  pages = bag_copy.size() / 10;
  total = pages * 10 + to_print_final;
  
  int position;
  int i;
  
  char buffer[100];
  int iterate = 3;

  clear();
  
  if(window == pages) {
    position = 10 * window;
    
    for(i = position; i < to_print_final + pages * 10; i++) {

      sprintf(buffer, "ITEM: %s", bag_copy[i].get_item_name().c_str());
      mvaddstr(iterate, 10, buffer);
      
      iterate += 1;
      
    }
  }
  
  else {
    
    position = window * 10;
    
    for(i = position; i < window * 10 + 10; i++) {

      sprintf(buffer, "ITEM: %s", bag_copy[i].get_item_name().c_str());
      mvaddstr(iterate, 10, buffer);
      
      iterate += 1;
    }

    refresh();
    
  }
}

int battle::view_pokemon_in_battle(PC_state &PC_s) {

  char pressed_key;

  int position = 0;
  
  while(1) {
    
    clear();
    print_pokemon(PC_s);
    mvaddstr((position % 10) + 3, 8, ">");
    refresh();

    pressed_key = getch();

    if(pressed_key == 27) {
      return -1;
    }

    if (pressed_key == '2') {
      if(position + 1 < PC_s.getPokemon().size()) {
	position++;
      }
    }

    if (pressed_key == '8') {
      if(position - 1 >= 0) {
	position--;
      }
    }
    
  }
  
}

void battle::print_pokemon(PC_state &PC_s) {
  
  int i;
  
  char buffer[100];
  int iterate = 3;
 
  for(i = 0; i < PC_s.getPokemon().size(); i++) {
    
    sprintf(buffer, "POKEMON: %s HP: %d / %d", PC_s.getPokemon()[i].get_name().c_str(), PC_s.getPokemon()[i].get_curr_HP(), PC_s.getPokemon()[i].get_HP());
    mvaddstr(iterate, 10, buffer);
    
    iterate += 1;
      
  }

  refresh();
    
}

int battle::process_move(PC_state &PC_s) {

  char pressed_key;

  char buffer1[50];
  char buffer2[50];

  sprintf(buffer1, "MOVE 1: %s", PC_s.getPokemon()[0].get_move_1().c_str());
  sprintf(buffer2, "MOVE 2: %s", PC_s.getPokemon()[0].get_move_2().c_str());

  mvaddstr(17, 30, buffer1);
  mvaddstr(18, 30, buffer2);

  refresh();
  
  while(1) {

    pressed_key = getch();

    if(pressed_key == 27) {
      return -1;
    }

    if(pressed_key == '1') {
      return PC_s.getPokemon()[0].get_move_id_1() - 1;
    }

    if(pressed_key == '2') {
      return PC_s.getPokemon()[0].get_move_id_2() - 1;
    }
    
    refresh();
  }
  
}

void battle::process_attacks(PC_state &PC_s, in_game_pokemon &NPC_s, std::vector<moves> mv, int PC_move_id, int *PC_hp, int *NPC_hp) {

  int rand_NPC_attack = rand() % 2;
  int NPC_move_id = -1;

  if(NPC_s.get_move_id_2() == -1) {
    NPC_move_id = NPC_s.get_move_id_1();
  }
  else {
    if(rand_NPC_attack == 0) {

      NPC_move_id = NPC_s.get_move_id_1() - 1;
    
    }
    else if(rand_NPC_attack == 1) {

      NPC_move_id = NPC_s.get_move_id_2() - 1;
    
    }
  }

  int NPC_attacking_damage = calculate_damage(NPC_s, PC_s.getPokemon()[0], NPC_move_id, mv);
  int PC_attacking_damage = calculate_damage(PC_s.getPokemon()[0], NPC_s, PC_move_id, mv);

  if(PC_s.getPokemon()[0].get_speed() > NPC_s.get_speed()) {

    char buffer1[100];
    char buffer2[100];
    char buffer3[100];
    char buffer4[100];

    sprintf(buffer1, "%s used %s", PC_s.getPokemon()[0].get_name().c_str(), mv[PC_move_id].identifier.c_str());
    mvaddstr(4, 45, buffer1);
    refresh();

    sleep(1);
    if(rand() % 100 > stoi(mv[PC_move_id].accuracy) && stoi(mv[PC_move_id].power) > 0) {
      sprintf(buffer3, "%s's move missed!", PC_s.getPokemon()[0].get_name().c_str());
      mvaddstr(5, 45, buffer3);
      refresh();
      sleep(1);
    } else {
    *NPC_hp -= PC_attacking_damage;
    }

    sprintf(buffer2, "%s used %s", NPC_s.get_name().c_str(), mv[NPC_move_id].identifier.c_str());
    mvaddstr(6, 45, buffer2);
    refresh();

    sleep(1);
    if(rand() % 100 > stoi(mv[NPC_move_id].accuracy) && stoi(mv[NPC_move_id].power) > 0) {
      sprintf(buffer4, "%s's move missed!", NPC_s.get_name().c_str());
      mvaddstr(7, 45, buffer4);
      refresh();
      sleep(1);
    } else {
    *PC_hp -= PC_attacking_damage;
    }
    
  }
  else {

    char buffer1[100];
    char buffer2[100];
    char buffer3[100];
    char buffer4[100];

    sprintf(buffer2, "%s used %s", NPC_s.get_name().c_str(), mv[NPC_move_id].identifier.c_str());
    mvaddstr(6, 45, buffer2);
    refresh();

    sleep(1);
    if(rand() % 100 > stoi(mv[NPC_move_id].accuracy) && stoi(mv[NPC_move_id].power) > 0) {
      sprintf(buffer4, "%s's move missed!", NPC_s.get_name().c_str());
      mvaddstr(7, 45, buffer4);
      refresh();
      sleep(1);
    } else {
    *PC_hp -= PC_attacking_damage;
    }

    sprintf(buffer1, "%s used %s", PC_s.getPokemon()[0].get_name().c_str(), mv[PC_move_id].identifier.c_str());
    mvaddstr(4, 45, buffer1);
    refresh();

    sleep(1);
    if(rand() % 100 > stoi(mv[PC_move_id].accuracy) && stoi(mv[PC_move_id].power) > 0) {
      sprintf(buffer3, "%s's move missed!", PC_s.getPokemon()[0].get_name().c_str());
      mvaddstr(5, 45, buffer3);
      refresh();
      sleep(1);
    } else {
    *NPC_hp -= PC_attacking_damage;
    }
    
  }

  
  
}

int battle::calculate_damage(in_game_pokemon attacking, in_game_pokemon defending, int move_id, std::vector<moves> mv) {

  double calc1 = (2 * attacking.get_level()) / 5;
  calc1 += 2;

  double calc2 = stoi(mv[move_id].power) * (attacking.get_attack() / defending.get_defense());

  double calc3 = calc1 * calc2;

  calc3 /= 50;
  calc3 += 2;

  double crit = 1;

  if((rand() % 256) == 0) {
    crit = 1.5;
  }

  //int random = (rand() % (100 - 85 + 1)) + 85;

  double STAB = 1;

  for(int i = 0; i < attacking.get_type_ids().size(); i++) {
    if(stoi(mv[move_id].type_id) == attacking.get_type_ids()[i]) {
      STAB = 1.5;
    }
  }

  // Type advantages not implemented in this assignment
  double type = 1;

  int calc4 = calc3 * crit * STAB * type;

  return calc4;

}
