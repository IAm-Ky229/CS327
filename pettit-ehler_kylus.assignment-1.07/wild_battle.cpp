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

int wild_pokemon_battle::determine_battle() {

  int temp = rand() % 100;

  if(temp >= 90) {
    return 1;
  }

  return 0;
}

void wild_pokemon_battle::engage_battle_wild(std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, int manhattan_x, int manhattan_y) {

  in_game_pokemon to_spawn = generate_pokemon(pkmn_list, pkmn_st, pkmn_mv, mv, manhattan_x, manhattan_y);

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

in_game_pokemon wild_pokemon_battle::generate_pokemon(std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, int manhattan_x, int manhattan_y) {

  in_game_pokemon to_spawn;
  choose_random_pokemon(to_spawn, pkmn_list);
  get_pokemon_stats(to_spawn, pkmn_st);
  assign_ivs(to_spawn);
  level_up(to_spawn, manhattan_x, manhattan_y);
  get_pokemon_moves(to_spawn, pkmn_mv);
  resolve_pokemon_move_names(to_spawn, mv);

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

void wild_pokemon_battle::choose_random_pokemon(in_game_pokemon &pkmn, std::vector<pokemon> pkmn_list) {

  int chosen_index = rand() % pkmn_list.size();
  pokemon selected_pkmn = pkmn_list[chosen_index];

  pkmn.set_id(stoi(selected_pkmn.id));
  pkmn.set_name(selected_pkmn.identifier);
  
}

void wild_pokemon_battle::get_pokemon_stats(in_game_pokemon &pkmn, std::vector<pokemon_stats> pkmn_st) {
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

void wild_pokemon_battle::get_pokemon_moves(in_game_pokemon &pkmn, std::vector<pokemon_moves> pkmn_mv) {
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

void wild_pokemon_battle::assign_ivs(in_game_pokemon &pkmn) {

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

int wild_pokemon_battle::generate_HP_lv_up(int base_HP, int HP_iv, int level) {

  int calc_1 = (base_HP + HP_iv) * 2 * level;
  double calc_2 = calc_1 / 100.0;
  calc_2 = floor(calc_2);
  int calc_3 = ((int) calc_2) + level + 10;
  

  return calc_3;
  
}

int wild_pokemon_battle::generate_otherstat_lv_up(int base_stat, int base_iv, int level) {

  int calc_1 = (base_stat + base_iv) * 2 * level;
  double calc_2 = calc_1 / 100.0;
  calc_2 = floor(calc_2);
  int calc_3 = ((int) calc_2) + 5;
  

  return calc_3;
  
}


void wild_pokemon_battle::level_up(in_game_pokemon &pkmn, int manhattan_x, int manhattan_y) {

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

void wild_pokemon_battle::resolve_pokemon_move_names(in_game_pokemon &pkmn, std::vector<moves> mv) {

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
