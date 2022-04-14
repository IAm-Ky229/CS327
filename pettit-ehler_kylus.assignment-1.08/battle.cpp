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
  return i.item_UID == item_UID;
}

int battle::determine_battle() {

  int temp = rand() % 100;

  if(temp <= 90) {
    return 1;
  }

  return 0;
}

void battle::engage_battle_wild(PC_state &PC_s, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, std::vector<pokemon_types> pkmn_typ, int manhattan_x, int manhattan_y) {

  clear();

  battle wild_pokemon_battle;

  int PC_total_pokemon = PC_s.getPokemon().size();

  in_game_pokemon generated = wild_pokemon_battle.generate_pokemon(pkmn_list, pkmn_st, pkmn_mv, mv, pkmn_typ, manhattan_x, manhattan_y);

  char buffer1[50];
  char buffer2[50];
  char buffer3[50];
  char buffer4[50];

  // Used to stay in the battle while loop
  int stay = 1;
  char pressed_key;

  // ID for doing nothing
  int PC_move_id = -1;

  // Start with the head of the PC pokemon array
  int PC_active_pokemon = 0;

  // Be sure we aren't starting with a pokemon that has 0 HP
  int iter = 0;
  int PC_fainted = 0;
  while(iter < PC_s.getPokemon().size()) {
    if(PC_s.getPokemon()[iter].get_curr_HP() != 0) {
      PC_active_pokemon = iter;
      break;
    }
    iter++;
  }

  iter = 0;
  while(iter < PC_s.getPokemon().size()) {
    if(PC_s.getPokemon()[iter].get_curr_HP() == 0) {
      PC_fainted++;
    }
    iter++;
  }

  if (PC_fainted == PC_s.getPokemon().size()) {
    clear();
    mvaddstr(10, 45, "PC has no pokemon to battle!");
    refresh();
    sleep(1);
    return;
  }

  int attempts_to_run = 1;

  while(stay) {

    PC_move_id = -1;

    clear();
    
    sprintf(buffer1, "NPC Pokemon: %s", generated.get_name().c_str());
    sprintf(buffer2, "Your Pokemon: %s", PC_s.getPokemon()[PC_active_pokemon].get_name().c_str());
    sprintf(buffer3, "HP: %d / %d", generated.get_curr_HP(), generated.get_HP());
    sprintf(buffer4, "HP: %d / %d", PC_s.getPokemon()[PC_active_pokemon].get_curr_HP(), PC_s.getPokemon()[PC_active_pokemon].get_HP());

    mvaddstr(2, 15, buffer1);
    mvaddstr(3, 15, buffer3);
    mvaddstr(17, 40, buffer2);
    mvaddstr(18, 40, buffer4);
    mvaddstr(17, 15, "1: FIGHT");
    mvaddstr(18, 15, "2: BAG");
    mvaddstr(19, 15, "3: POKEMON");
    mvaddstr(20, 15, "4: RUN");
  
    refresh();
    pressed_key = getch();

    switch (pressed_key) {

    case '1':

      PC_move_id = wild_pokemon_battle.process_move(PC_s, PC_active_pokemon);
      break;

    case '2':
      
      PC_move_id = wild_pokemon_battle.enter_bag(PC_s, generated, 1, &PC_fainted);
      break;

    case '3':
      
      PC_move_id = wild_pokemon_battle.view_pokemon_in_battle(PC_s, &PC_active_pokemon, 0);
      break;

    case '4':

      PC_move_id = wild_pokemon_battle.run(PC_s, PC_active_pokemon, generated, &attempts_to_run);
      break;
      
    }

    // Flee battle move code
    if(PC_move_id == -999) {
      break;
    }

    clear();
    
    sprintf(buffer1, "NPC Pokemon: %s", generated.get_name().c_str());
    sprintf(buffer2, "Your Pokemon: %s", PC_s.getPokemon()[PC_active_pokemon].get_name().c_str());
    sprintf(buffer3, "HP: %d / %d", generated.get_curr_HP(), generated.get_HP());
    sprintf(buffer4, "HP: %d / %d", PC_s.getPokemon()[PC_active_pokemon].get_curr_HP(), PC_s.getPokemon()[PC_active_pokemon].get_HP());

    mvaddstr(2, 15, buffer1);
    mvaddstr(3, 15, buffer3);
    mvaddstr(17, 40, buffer2);
    mvaddstr(18, 40, buffer4);
    mvaddstr(17, 15, "1: FIGHT");
    mvaddstr(18, 15, "2: BAG");
    mvaddstr(19, 15, "3: POKEMON");
    mvaddstr(20, 15, "4: RUN");
  
    refresh();

    if(PC_move_id != -1) {
      
      wild_pokemon_battle.process_attacks(PC_s, generated, mv, PC_move_id, PC_active_pokemon);

    }

    if(generated.get_curr_HP() <= 0) {
      
      char buffer[100];
      clear();

      sprintf(buffer, "Wild %s fainted!", generated.get_name().c_str());
      mvaddstr(10, 45, buffer);
      stay = 0;
      refresh();
      sleep(1);
      stay = 0;
    }
    else if(PC_s.getPokemon()[PC_active_pokemon].get_curr_HP() <= 0) {

      PC_fainted++;
      if(PC_fainted == PC_total_pokemon) {
	clear();
	mvaddstr(10, 45, "All PC pokemon fainted!");
	stay = 0;
	refresh();
	sleep(1);
	stay = 0;
      }
      else {
        wild_pokemon_battle.view_pokemon_in_battle(PC_s, &PC_active_pokemon, 1);
      }
    }
    
  }
  
}

int battle::run(PC_state &PC_s, int PC_active_pokemon, in_game_pokemon opposing_pokemon, int *attempts_to_run) {

  int calc1 = PC_s.getPokemon()[PC_active_pokemon].get_speed() * 32;
  double calc2 = (opposing_pokemon.get_speed() / 4) % 256;

  double calc3 = calc1 / calc2;
  int calc4 = (calc3) + (30 * (*attempts_to_run));
  
  mvaddstr(13, 30, "Attempting to run");
  refresh();
  sleep(1);

  if(rand() % 256 < calc4) {
    mvaddstr(14, 30, "Fled from the battle");
    refresh();
    sleep(1);
    return -999;
  }
  else {
    *attempts_to_run += 1;
    mvaddstr(14, 30, "Could not escape");
    refresh();
    sleep(1);
    return -99;
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
  pkmn.set_base_speed(stoi(selected_stats[5].base_stat));
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
  std::vector<in_game_pokemon> NPC_generated;

  int PC_total_pokemon = PC_s.getPokemon().size();

  // Match PC number of pokemon
  for(int i = 0; i < PC_total_pokemon; i++) {
    in_game_pokemon generated = NPC_pokemon_battle.generate_pokemon(pkmn_list, pkmn_st, pkmn_mv, mv, pkmn_typ, manhattan_x, manhattan_y);
    NPC_generated.push_back(generated);
  }

  // Series 60% chance of another pokemon
  int rand_gen = rand() % 100;
  int stack = 60;
  while(rand_gen < stack && NPC_generated.size() < 6) {
    in_game_pokemon generated = NPC_pokemon_battle.generate_pokemon(pkmn_list, pkmn_st, pkmn_mv, mv, pkmn_typ, manhattan_x, manhattan_y);
    NPC_generated.push_back(generated);

    stack *= .6;
    rand_gen = rand() % 100;
  }

  int NPC_total_pokemon = NPC_generated.size();

  char buffer1[50];
  char buffer2[50];
  char buffer3[50];
  char buffer4[50];

  // Used to stay in the battle while loop
  int stay = 1;
  char pressed_key;

  // ID for doing nothing
  int PC_move_id = -1;

  // Start with the head of the PC pokemon array
  int PC_active_pokemon = 0;

  // We have to be sure we start the battle with a non-fainted pokemon
  int iter = 0;
  int PC_fainted = 0;
  while(iter < PC_s.getPokemon().size()) {
    if(PC_s.getPokemon()[iter].get_curr_HP() != 0) {
      PC_active_pokemon = iter;
      break;
    }
    iter++;
  }

  // Manually check PC fainted pokemon, IDK what the issue was with updating PC fainted dynamically
  iter = 0;
  while(iter < PC_s.getPokemon().size()) {
    if(PC_s.getPokemon()[iter].get_curr_HP() == 0) {
      PC_fainted++;
    }
    iter++;
  }

  if(PC_fainted  == PC_s.getPokemon().size()) {
    clear();
    mvaddstr(10, 45, "PC has no pokemon to battle!");
    refresh();
    sleep(1);
    return;
  }
  
  int NPC_active_pokemon = 0;
  int NPC_fainted_pokemon = 0;

  while(stay) {

    PC_move_id = -1;

    clear();
    
    sprintf(buffer1, "NPC Pokemon: %s", NPC_generated[NPC_active_pokemon].get_name().c_str());
    sprintf(buffer2, "Your Pokemon: %s", PC_s.getPokemon()[PC_active_pokemon].get_name().c_str());
    sprintf(buffer3, "HP: %d / %d", NPC_generated[NPC_active_pokemon].get_curr_HP(), NPC_generated[NPC_active_pokemon].get_HP());
    sprintf(buffer4, "HP: %d / %d", PC_s.getPokemon()[PC_active_pokemon].get_curr_HP(), PC_s.getPokemon()[PC_active_pokemon].get_HP());

    mvaddstr(2, 15, buffer1);
    mvaddstr(3, 15, buffer3);
    mvaddstr(17, 40, buffer2);
    mvaddstr(18, 40, buffer4);
    mvaddstr(17, 15, "1: FIGHT");
    mvaddstr(18, 15, "2: BAG");
    mvaddstr(19, 15, "3: POKEMON");
  
    refresh();
    pressed_key = getch();

    switch (pressed_key) {

    case '1':

      PC_move_id = NPC_pokemon_battle.process_move(PC_s, PC_active_pokemon);
      break;

    case '2':
      
      PC_move_id = NPC_pokemon_battle.enter_bag(PC_s, NPC_generated[NPC_active_pokemon], 0, &PC_fainted);
      break;

    case '3':
      
      PC_move_id = NPC_pokemon_battle.view_pokemon_in_battle(PC_s, &PC_active_pokemon, 0);
      break;
      
    }

    clear();
    
    sprintf(buffer1, "NPC Pokemon: %s", NPC_generated[NPC_active_pokemon].get_name().c_str());
    sprintf(buffer2, "Your Pokemon: %s", PC_s.getPokemon()[PC_active_pokemon].get_name().c_str());
    sprintf(buffer3, "HP: %d / %d", NPC_generated[NPC_active_pokemon].get_curr_HP(), NPC_generated[NPC_active_pokemon].get_HP());
    sprintf(buffer4, "HP: %d / %d", PC_s.getPokemon()[PC_active_pokemon].get_curr_HP(), PC_s.getPokemon()[PC_active_pokemon].get_HP());

    mvaddstr(2, 15, buffer1);
    mvaddstr(3, 15, buffer3);
    mvaddstr(17, 40, buffer2);
    mvaddstr(18, 40, buffer4);
    mvaddstr(17, 15, "1: FIGHT");
    mvaddstr(18, 15, "2: BAG");
    mvaddstr(19, 15, "3: POKEMON");

    if(PC_move_id != -1) {
      
      NPC_pokemon_battle.process_attacks(PC_s, NPC_generated[NPC_active_pokemon], mv, PC_move_id, PC_active_pokemon);

    }

    if(NPC_generated[NPC_active_pokemon].get_curr_HP() <= 0) {

      NPC_fainted_pokemon++;
      if(NPC_fainted_pokemon == NPC_total_pokemon) {
	clear();
	mvaddstr(10, 45, "NPC loses!");
	refresh();
	sleep(1);
	break;
      }
      // The AI for trainers is pretty bad so we can just increment the active pokemon counter
      else {
	NPC_active_pokemon++;
      }
    }
    else if(PC_s.getPokemon()[PC_active_pokemon].get_curr_HP() <= 0) {

      PC_fainted++;
      if(PC_fainted == PC_total_pokemon) {
	clear();
	mvaddstr(10, 45, "PC loses!");
	refresh();
	sleep(1);
	break;
      }
      else {
        NPC_pokemon_battle.view_pokemon_in_battle(PC_s, &PC_active_pokemon, 1);
      }
    }
    
  }
}

int battle::enter_bag(PC_state &PC_s, in_game_pokemon opposing_pokemon, int pokeballs_usable, int *PC_fainted) {

  char pressed_key;
  int position = 0;
  
  while(1) {
    clear();
    print_bag(PC_s.get_items(), 1, position / 10);
    mvaddstr((position % 10) + 3, 8, ">");
    refresh();
    
    pressed_key = '0';
    pressed_key = getch();

    // press esc to exit
    if (pressed_key == 27) {
      return -1;
    }

    if (pressed_key == '2') {
      if(position + 1 < PC_s.get_items().size()) {
	position++;
      }
    }

    if (pressed_key == '8') {
      if(position - 1 >= 0) {
	position--;
      }
    }

    // Keeps from using pokeballs in NPC battles
    if (pressed_key == '+' &&
	PC_s.get_items()[position].get_item_ID() != 3) {

      item itm = PC_s.get_items()[position];
      
      int position2 = 0;

      while(pressed_key != 27) {

	clear();
	print_pokemon(PC_s);
	mvaddstr((position2 % 10) + 3, 8, ">");
	refresh();

	pressed_key = '0';
	pressed_key = getch();

	if (pressed_key == '2') {
	  if(position2 + 1 < PC_s.get_items().size()) {
	    position2++;
	  }
	}

	if (pressed_key == '8') {
	  if(position2 - 1 >= 0) {
	    position2--;
	  }
	}

	if(pressed_key == '+') {

	  switch(itm.get_item_ID()) {
	    
	  case 1:
	    
	    use_potion(PC_s, position2, itm);
	    return -99;
	    
	  case 2:
	    
	    use_revive(PC_s, position2, itm, PC_fainted);
	    return -99;
	    
	  }
	  
	}
	
      }
      
    }
    // Allows pokeball use in wild battles
    else if (pressed_key == '+' &&
	     PC_s.get_items()[position].get_item_ID() == 3 &&
	     pokeballs_usable) {
      item itm = PC_s.get_items()[position];
      return use_pokeball(PC_s, opposing_pokemon, itm);
    }
    
  }
  
}

void battle::use_potion(PC_state &PC_s, int pokemon_rec, item itm) {

  char buffer[100];

  sprintf(buffer, "Used a potion on %s", PC_s.getPokemon()[pokemon_rec].get_name().c_str());
  mvaddstr(20, 40, buffer);
  refresh();
  sleep(1);

  if(!(PC_s.getPokemon()[pokemon_rec].get_curr_HP() <= 0)) {
    PC_s.getPokemon()[pokemon_rec].set_curr_HP(PC_s.getPokemon()[pokemon_rec].get_curr_HP() + 20);
  }

  if(PC_s.getPokemon()[pokemon_rec].get_curr_HP() > PC_s.getPokemon()[pokemon_rec].get_HP()) {
    PC_s.getPokemon()[pokemon_rec].set_curr_HP(PC_s.getPokemon()[pokemon_rec].get_HP());
  }

  PC_s.removeItem(itm);
}

void battle::use_revive(PC_state &PC_s, int pokemon_rec, item itm, int *PC_fainted) {

  char buffer[100];
  
  sprintf(buffer, "Used a revive on %s", PC_s.getPokemon()[pokemon_rec].get_name().c_str());
  mvaddstr(20, 40, buffer);
  refresh();
  sleep(1);

  if(PC_s.getPokemon()[pokemon_rec].get_curr_HP() <= 0) {
    PC_s.getPokemon()[pokemon_rec].set_curr_HP(PC_s.getPokemon()[pokemon_rec].get_HP() / 2);
    *PC_fainted -= 1;
  }

  PC_s.removeItem(itm);
  
}

int battle::use_pokeball(PC_state &PC_s, in_game_pokemon wild_pkmn, item itm) {

  char buffer[100];

  sprintf(buffer, "Using a pokeball");
  mvaddstr(5, 40, buffer);
  refresh();

  sleep(1);

  // For this assignment, using a pokeball is going to end the battle either way
  // So both cases return -999. Otherwise, pokeball failure would result in
  // Wild pokemon attacking
  if(PC_s.getPokemon().size() < 6) {
    PC_s.getPokemon().push_back(wild_pkmn);

    clear();
    sprintf(buffer, "Wild %s was caught", wild_pkmn.get_name().c_str());
    mvaddstr(10, 40, buffer);
    refresh();
    sleep(1);
    PC_s.removeItem(itm);
    return -999;
  }
  else {

    clear();

    sprintf(buffer, "Wild %s got away", wild_pkmn.get_name().c_str());
    mvaddstr(10, 40, buffer);
    refresh();
    sleep(1);
    PC_s.removeItem(itm);
    return -999;
    
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

int battle::view_pokemon_in_battle(PC_state &PC_s, int *PC_active_pokemon, int must_choose) {

  char pressed_key;

  int position = 0;
  
  while(1) {
    
    clear();
    print_pokemon(PC_s);
    mvaddstr((position % 10) + 3, 8, ">");
    refresh();

    pressed_key = getch();

    if(pressed_key == 27) {
      if(!must_choose){
	return -1;
      }
    }

    if (pressed_key == '2') {
      if(position + 1 < PC_s.getPokemon().size()) {
	position++;
      }
    }

    if(pressed_key == '+') {
      if(PC_s.getPokemon()[position].get_curr_HP() != 0) {
	char buffer[100];
	sprintf(buffer, "Switching out to %s", PC_s.getPokemon()[position].get_name().c_str());
	mvaddstr(20, 55, buffer);
	refresh();
	sleep(1);
	
	*PC_active_pokemon = position;
	return -99;
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

int battle::process_move(PC_state &PC_s, int PC_active_pokemon) {

  char pressed_key;

  char buffer1[50];
  char buffer2[50];

  sprintf(buffer1, "MOVE 1: %s", PC_s.getPokemon()[PC_active_pokemon].get_move_1().c_str());
  sprintf(buffer2, "MOVE 2: %s", PC_s.getPokemon()[PC_active_pokemon].get_move_2().c_str());

  mvaddstr(14, 20, buffer1);
  mvaddstr(15, 20, buffer2);

  refresh();
  
  while(1) {

    pressed_key = getch();

    if(pressed_key == 27) {
      return -1;
    }

    if(pressed_key == '1') {
      return PC_s.getPokemon()[PC_active_pokemon].get_move_id_1() - 1;
    }

    if(pressed_key == '2') {
      // All pokemon should have at least 1 move
      // But we shouldn't be able to choose an unassigned move
      if(PC_s.getPokemon()[PC_active_pokemon].get_move_id_2() != -1) {
	return PC_s.getPokemon()[PC_active_pokemon].get_move_id_2() - 1;
      }
    }
    
    refresh();
  }
  
}

void battle::process_attacks(PC_state &PC_s, in_game_pokemon &NPC_s, std::vector<moves> mv, int PC_move_id, int PC_active_pokemon) {

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

  // -99 means PC did something other than attack
  if( PC_move_id != -99) {
  
    int NPC_attacking_damage = calculate_damage(NPC_s, PC_s.getPokemon()[PC_active_pokemon], NPC_move_id, mv);
    int PC_attacking_damage = calculate_damage(PC_s.getPokemon()[PC_active_pokemon], NPC_s, PC_move_id, mv);

    // PC is faster
    if(((PC_s.getPokemon()[PC_active_pokemon].get_speed() > NPC_s.get_speed())
	&& !(stoi(mv[PC_move_id].priority) < stoi(mv[NPC_move_id].priority)))
       || (stoi(mv[PC_move_id].priority) > stoi(mv[NPC_move_id].priority))) {

      char buffer1[100];
      char buffer2[100];
      char buffer3[100];
      char buffer4[100];

      sprintf(buffer1, "%s used %s", PC_s.getPokemon()[PC_active_pokemon].get_name().c_str(), mv[PC_move_id].identifier.c_str());
      mvaddstr(6, 30, buffer1);
      refresh();

      sleep(1);
      if(rand() % 100 > stoi(mv[PC_move_id].accuracy) && stoi(mv[PC_move_id].power) > 0) {
	sprintf(buffer3, "%s's move missed!", PC_s.getPokemon()[PC_active_pokemon].get_name().c_str());
	mvaddstr(7, 30, buffer3);
      } else {
	sprintf(buffer3, "%d damage!", PC_attacking_damage);
	mvaddstr(7, 30, buffer3);
	NPC_s.set_curr_HP(NPC_s.get_curr_HP() - PC_attacking_damage);
	if(NPC_s.get_curr_HP() < 0) {
	  NPC_s.set_curr_HP(0);
	}
      }
      refresh();
      sleep(1);

      if (NPC_s.get_curr_HP() > 0) {
	sprintf(buffer2, "%s used %s", NPC_s.get_name().c_str(), mv[NPC_move_id].identifier.c_str());
	mvaddstr(8, 30, buffer2);
	refresh();

	sleep(1);
	if(rand() % 100 > stoi(mv[NPC_move_id].accuracy) && stoi(mv[NPC_move_id].power) > 0) {
	  sprintf(buffer4, "%s's move missed!", NPC_s.get_name().c_str());
	  mvaddstr(9, 30, buffer4);
	} else {
	  sprintf(buffer4, "%d damage!", NPC_attacking_damage);
	  mvaddstr(9, 30, buffer4);
	  PC_s.getPokemon()[PC_active_pokemon]
	    .set_curr_HP(PC_s.getPokemon()[PC_active_pokemon].get_curr_HP() - NPC_attacking_damage);
	  if(PC_s.getPokemon()[PC_active_pokemon].get_curr_HP() < 0) {
	    PC_s.getPokemon()[PC_active_pokemon].set_curr_HP(0);
	  }
	}

	refresh();
	sleep(1);
      }
    
    }
    // if NPC is faster
    else {

      char buffer1[100];
      char buffer2[100];
      char buffer3[100];
      char buffer4[100];

      sprintf(buffer2, "%s used %s", NPC_s.get_name().c_str(), mv[NPC_move_id].identifier.c_str());
      mvaddstr(8, 30, buffer2);
      refresh();

      sleep(1);
      if(rand() % 100 > stoi(mv[NPC_move_id].accuracy) && stoi(mv[NPC_move_id].power) > 0) {
	sprintf(buffer4, "%s's move missed!", NPC_s.get_name().c_str());
	mvaddstr(9, 30, buffer4);
      } else {
	sprintf(buffer4, "%d damage!", NPC_attacking_damage);
	mvaddstr(9, 30, buffer4);
	PC_s.getPokemon()[PC_active_pokemon]
	  .set_curr_HP(PC_s.getPokemon()[PC_active_pokemon].get_curr_HP() - NPC_attacking_damage);
	if(PC_s.getPokemon()[PC_active_pokemon].get_curr_HP() < 0) {
	  PC_s.getPokemon()[PC_active_pokemon].set_curr_HP(0);
	}
      }
      refresh();
      sleep(1);

      if(PC_s.getPokemon()[PC_active_pokemon].get_curr_HP() > 0) {
	sprintf(buffer1, "%s used %s", PC_s.getPokemon()[PC_active_pokemon].get_name().c_str(), mv[PC_move_id].identifier.c_str());
	mvaddstr(6, 30, buffer1);
	refresh();

	sleep(1);
	if(rand() % 100 > stoi(mv[PC_move_id].accuracy) && stoi(mv[PC_move_id].power) > 0) {
	  sprintf(buffer3, "%s's move missed!", PC_s.getPokemon()[PC_active_pokemon].get_name().c_str());
	  mvaddstr(7, 30, buffer3);
	} else {
	  sprintf(buffer3, "%d damage!", PC_attacking_damage);
	  mvaddstr(7, 30, buffer3);
	  NPC_s.set_curr_HP(NPC_s.get_curr_HP() - PC_attacking_damage);
	  if(NPC_s.get_curr_HP() < 0) {
	    NPC_s.set_curr_HP(0);
	  }
	}
	refresh();
	sleep(1);
      }
    
    }

  }
  // PC did something like switch pokemon / use potion, so NPC attacks
  else {

      char buffer2[100];
      char buffer4[100];

      int NPC_attacking_damage = calculate_damage(NPC_s, PC_s.getPokemon()[PC_active_pokemon], NPC_move_id, mv);

      sprintf(buffer2, "%s used %s", NPC_s.get_name().c_str(), mv[NPC_move_id].identifier.c_str());
      mvaddstr(8, 45, buffer2);
      refresh();

      sleep(1);
      if(rand() % 100 > stoi(mv[NPC_move_id].accuracy) && stoi(mv[NPC_move_id].power) > 0) {
	sprintf(buffer4, "%s's move missed!", NPC_s.get_name().c_str());
	mvaddstr(9, 45, buffer4);
      } else {
	sprintf(buffer4, "%d damage!", NPC_attacking_damage);
	mvaddstr(9, 45, buffer4);
	PC_s.getPokemon()[PC_active_pokemon].set_curr_HP(PC_s.getPokemon()[PC_active_pokemon].get_curr_HP() - NPC_attacking_damage);
	if(PC_s.getPokemon()[PC_active_pokemon].get_curr_HP() < 0) {
	  PC_s.getPokemon()[PC_active_pokemon].set_curr_HP(0);
	}
      }
      refresh();
      sleep(1);
  }

  
  
}

int battle::calculate_damage(in_game_pokemon attacking, in_game_pokemon defending, int move_id, std::vector<moves> mv) {

  double calc1 = (2 * attacking.get_level()) / 5;
  calc1 += 2;

  double calc2 = 0;

  if(stoi(mv[move_id].damage_class_id) == 2) {
    calc2 = stoi(mv[move_id].power) * (attacking.get_attack() / defending.get_defense());
  }
  else if(stoi(mv[move_id].damage_class_id) == 3) {
    calc2 = stoi(mv[move_id].power) * (attacking.get_special_attack() / defending.get_special_defense());
  }
  else {
    calc2 = stoi(mv[move_id].power) * (attacking.get_attack() / defending.get_defense());
  }

  double calc3 = calc1 * calc2;

  calc3 /= 50;
  calc3 += 2;

  double crit = 1;

  if(rand() % 256 < (attacking.get_base_speed() / 2)) {
    crit = 1.5;
  }

  // Don't know what this random is supposed to do, but I'm not doing it right
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
