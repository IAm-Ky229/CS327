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

int movementCosts::determine_cost_rival(generatedMap *map_data, int x_dim, int y_dim) {

  switch(map_data -> generate_map[x_dim][y_dim]) {

  case path:
    return 10;
  case tall_grass:
    return 20;
  case clearing:
    return 10;
  case nothing:
    return 10;
  case pokemon_center:
    return INT_MAX;
  case pokemon_mart:
    return INT_MAX;
  case boulder:
    return INT_MAX;
  case tree:
    return INT_MAX;
  case stationary_occupied:
    return INT_MAX;
  default:
    return 0;
  }
}

int movementCosts::determine_cost_PC(generatedMap *map_data, int x_dim, int y_dim) {

  switch(map_data -> generate_map[x_dim][y_dim]) {

  case path:
    return 10;
  case tall_grass:
    return 20;
  case clearing:
    return 10;
  case nothing:
    return 10;
  case pokemon_center:
    return 10;
  case pokemon_mart:
    return 10;
  case boulder:
    return INT_MAX;
  case tree:
    return INT_MAX;
  case stationary_occupied:
    return INT_MAX;
  default:
    return 0;
  }
}

int movementCosts::determine_cost_hiker(generatedMap *map_data, int x_dim, int y_dim) {

  switch(map_data -> generate_map[x_dim][y_dim]) {

  case path:
    return 10;
  case tall_grass:
    return 15;
  case clearing:
    return 10;
  case nothing:
    return 10;
  case pokemon_center:
    return INT_MAX;
  case pokemon_mart:
    return INT_MAX;
  case boulder:
    return INT_MAX;
  case tree:
    return INT_MAX;
  case stationary_occupied:
    return INT_MAX;
  default:
    return 0;
  }
}

void characterLogic::place_characters(generatedMap *m, heap_t *h, movementCosts distance_hiker[HORIZONTAL][VERTICAL], movementCosts distance_rival[HORIZONTAL][VERTICAL], int numtrainers) {

  movementCosts pathfind;

  int number_trainers = numtrainers;
  
  int placed_characters = 0;
  int rand_x;
  int rand_y;

  int min_x_next;
  int min_y_next;
  int cost_to_move;

  int i;
  enum char_type characters_to_place[number_trainers];

  // Default to 10
  if(number_trainers < 0) {
    number_trainers = 10;
  }

  if(number_trainers == 1) { 
    characters_to_place[0] = hiker;
    i = 1;
  }
  else {
    characters_to_place[0] = hiker;
    characters_to_place[1] = rival;

    i = 2;
  }

  // I thought about adding a check for a max number of trainers but
  // I'm assuming the intention isn't to break the game
  
  int choose_character;

  for (i; i < number_trainers; i++) {
    choose_character = rand() % 6;

    switch (choose_character) {
    case 0:
      characters_to_place[i] = hiker;
      break;
    case 1:
      characters_to_place[i] = rival;
      break;
    case 2:
      characters_to_place[i] = pacer;
      break;
    case 3:
      characters_to_place[i] = wanderer;
      break;
    case 4:
      characters_to_place[i] = stationary;
      break;
    case 5:
      characters_to_place[i] = random_walker;
      break;
    }
    
  }

  // Place all the characters, and determine their first moves
  // I brute forced a lot of this and there's some replicated code
  // Sorry if this hurts to look at
  i = 0;
  while (placed_characters < number_trainers) {

    rand_x = (rand() % (78 - 2 + 1)) + 2;
    rand_y = (rand() % (19 - 2 + 1)) + 2;

    if(m -> generate_map[rand_x][rand_y] != boulder &&
       m -> generate_map[rand_x][rand_y] != tree &&
       m -> generate_map[rand_x][rand_y] != pokemon_mart &&
       m -> generate_map[rand_x][rand_y] != pokemon_center &&
       m -> generate_map[rand_x][rand_y] != path &&
       m -> character_positions[rand_x][rand_y] == NULL) {
     
      m -> character_positions[rand_x][rand_y] = (NPC_char*) malloc(sizeof(NPC_char));
      m -> character_positions[rand_x][rand_y] -> player_type = characters_to_place[placed_characters];
      m -> character_positions[rand_x][rand_y] -> cost_to_move = 0;
      m -> character_positions[rand_x][rand_y] -> battled = 0;
      m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
      m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
      
      switch(m -> character_positions[rand_x][rand_y] -> player_type) {
      case wanderer:
	m -> character_positions[rand_x][rand_y] -> terrain_type = m -> generate_map[rand_x][rand_y];
	
	if(m -> generate_map[rand_x][rand_y + 1] != tree &&
	   m -> generate_map[rand_x][rand_y + 1] != boulder &&
	   m -> character_positions[rand_x][rand_y] -> terrain_type == m -> generate_map[rand_x][rand_y + 1]) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = down;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x, rand_y + 1);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y + 1;
	}
	else if(m -> generate_map[rand_x][rand_y - 1] != tree &&
		m -> generate_map[rand_x][rand_y - 1] != boulder &&
		m -> character_positions[rand_x][rand_y] -> terrain_type == m -> generate_map[rand_x][rand_y - 1]) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = up;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x, rand_y - 1);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y - 1;
	}
	else if(m -> generate_map[rand_x + 1][rand_y] != tree &&
		m -> generate_map[rand_x + 1][rand_y] != boulder &&
		m -> character_positions[rand_x][rand_y] -> terrain_type == m -> generate_map[rand_x + 1][rand_y]) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = right;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x + 1, rand_y);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x + 1;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y;
	}
	else if(m -> generate_map[rand_x - 1][rand_y] != tree &&
		m -> generate_map[rand_x - 1][rand_y] != boulder &&
		m -> character_positions[rand_x][rand_y] -> terrain_type == m -> generate_map[rand_x - 1][rand_y]) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = left;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x - 1, rand_y);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x - 1;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y;
	}
	break;
      case random_walker:
	if(m -> generate_map[rand_x][rand_y + 1] != tree &&
	   m -> generate_map[rand_x][rand_y + 1] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = down;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x, rand_y + 1);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y + 1;
	}
	else if(m -> generate_map[rand_x][rand_y - 1] != tree &&
		m -> generate_map[rand_x][rand_y - 1] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = up;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x, rand_y - 1);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y - 1;
	}
	else if(m -> generate_map[rand_x + 1][rand_y] != tree &&
		m -> generate_map[rand_x + 1][rand_y] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = right;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x + 1, rand_y);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x + 1;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y;
	}
	else if(m -> generate_map[rand_x - 1][rand_y] != tree &&
		m -> generate_map[rand_x - 1][rand_y] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = left;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x - 1, rand_y);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x - 1;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y;
	}
	break;
      case stationary:
	m -> generate_map[rand_x][rand_y] = stationary_occupied;
	m -> character_positions[rand_x][rand_y] -> cost_to_move = 2000000000;
	break;
      case pacer:
	if(m -> generate_map[rand_x][rand_y + 1] != tree &&
	   m -> generate_map[rand_x][rand_y + 1] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = down;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x, rand_y + 1);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y + 1;
	}
	else if(m -> generate_map[rand_x][rand_y - 1] != tree &&
		m -> generate_map[rand_x][rand_y - 1] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = up;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x, rand_y - 1);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y - 1;
	}
	else if(m -> generate_map[rand_x + 1][rand_y] != tree &&
		m -> generate_map[rand_x + 1][rand_y] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = right;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x + 1, rand_y);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x + 1;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y;
	}
	else if(m -> generate_map[rand_x - 1][rand_y] != tree &&
		m -> generate_map[rand_x - 1][rand_y] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> cur_direction = left;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    pathfind.determine_cost_rival(m, rand_x - 1, rand_y);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x - 1;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y;
	}
	break;
	
      case rival:
	
	cost_to_move = 9999;
	min_x_next = 0;
	min_y_next = 0;
	
	if(distance_rival[rand_x + 1][rand_y].cost < cost_to_move &&
	   distance_rival[rand_x + 1][rand_y].cost >= 0) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y;
	  
	  cost_to_move = distance_rival[rand_x + 1][rand_y].cost;
	}
	
	if(distance_rival[rand_x][rand_y + 1].cost < cost_to_move &&
	   distance_rival[rand_x][rand_y + 1].cost >= 0) {
	  min_x_next = rand_x;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_rival[rand_x][rand_y + 1].cost;
	}
	
	if(distance_rival[rand_x - 1][rand_y].cost < cost_to_move &&
	   distance_rival[rand_x - 1][rand_y].cost >= 0) {
	  min_x_next = rand_x - 1;
	  min_y_next = rand_y;
	  
	  cost_to_move = distance_rival[rand_x - 1][rand_y].cost;
	}
	
	if(distance_rival[rand_x][rand_y - 1].cost < cost_to_move &&
	   distance_rival[rand_x][rand_y - 1].cost >= 0) {
	  min_x_next = rand_x;
	  min_y_next = rand_y - 1;
	  
	  cost_to_move = distance_rival[rand_x][rand_y - 1].cost;
	}
	
	if(distance_rival[rand_x + 1][rand_y + 1].cost < cost_to_move &&
	   distance_rival[rand_x + 1][rand_y + 1].cost >= 0) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_rival[rand_x + 1][rand_y + 1].cost;
	}
	
	if(distance_rival[rand_x + 1][rand_y - 1].cost < cost_to_move &&
	   distance_rival[rand_x + 1][rand_y - 1].cost >= 0) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y - 1;
	  
	  cost_to_move = distance_rival[rand_x + 1][rand_y - 1].cost;
	}
	
	if(distance_rival[rand_x - 1][rand_y + 1].cost < cost_to_move &&
	   distance_rival[rand_x - 1][rand_y + 1].cost >= 0) {
	  min_x_next = rand_x - 1;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_rival[rand_x - 1][rand_y + 1].cost;
	}
	
	if(distance_rival[rand_x - 1][rand_y - 1].cost < cost_to_move &&
	   distance_rival[rand_x - 1][rand_y - 1].cost >= 0) {
	  min_x_next = rand_x - 1;
	  min_y_next = rand_y - 1;
	  
	  cost_to_move = distance_rival[rand_x - 1][rand_y - 1].cost;
	}
	
	m -> character_positions[rand_x][rand_y] -> cost_to_move = distance_rival[rand_x][rand_y].cost - cost_to_move;
	m -> character_positions[rand_x][rand_y] -> next_x = min_x_next;
	m -> character_positions[rand_x][rand_y] -> next_y = min_y_next;
	
	break;
	
      case hiker:
	
	cost_to_move = 9999;
	min_x_next = 0;
	min_y_next = 0;
	
	if(distance_hiker[rand_x + 1][rand_y].cost < cost_to_move &&
	   distance_hiker[rand_x + 1][rand_y].cost >= 0) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y;
	  
	  cost_to_move = distance_hiker[rand_x + 1][rand_y].cost;
	}
	
	if(distance_hiker[rand_x][rand_y + 1].cost < cost_to_move &&
	   distance_hiker[rand_x][rand_y + 1].cost >= 0) {
	  min_x_next = rand_x;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_hiker[rand_x][rand_y + 1].cost;
	}
	
	if(distance_hiker[rand_x - 1][rand_y].cost < cost_to_move &&
	   distance_hiker[rand_x - 1][rand_y].cost >= 0) {
	  min_x_next = rand_x - 1;
	  min_y_next = rand_y;
	  
	  cost_to_move = distance_hiker[rand_x - 1][rand_y].cost;
	}
	
	if(distance_hiker[rand_x][rand_y - 1].cost < cost_to_move &&
	   distance_hiker[rand_x][rand_y - 1].cost >= 0) {
	  min_x_next = rand_x;
	  min_y_next = rand_y - 1;
	  
	  cost_to_move = distance_hiker[rand_x][rand_y - 1].cost;
	}
	
	if( distance_hiker[rand_x + 1][rand_y + 1].cost < cost_to_move &&
	    distance_hiker[rand_x + 1][rand_y + 1].cost >= 0) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_hiker[rand_x + 1][rand_y + 1].cost;
	}
	
	if( distance_hiker[rand_x + 1][rand_y - 1].cost < cost_to_move &&
	    distance_hiker[rand_x + 1][rand_y - 1].cost >= 0) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y - 1;
	  
	  cost_to_move = distance_hiker[rand_x + 1][rand_y - 1].cost;
	}
	
	if(distance_hiker[rand_x - 1][rand_y + 1].cost < cost_to_move &&
	   distance_hiker[rand_x - 1][rand_y + 1].cost >= 0) {
	  min_x_next = rand_x - 1;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_hiker[rand_x - 1][rand_y + 1].cost;
	}
	
	if(distance_hiker[rand_x - 1][rand_y - 1].cost < cost_to_move &&
	   distance_hiker[rand_x - 1][rand_y - 1].cost >= 0) {
	  min_x_next = rand_x - 1;
	  min_y_next = rand_y - 1;
	  
	  cost_to_move = distance_hiker[rand_x - 1][rand_y - 1].cost;
	}
	
	m -> character_positions[rand_x][rand_y] -> cost_to_move = distance_rival[rand_x][rand_y].cost - cost_to_move;
	m -> character_positions[rand_x][rand_y] -> next_x = min_x_next;
	m -> character_positions[rand_x][rand_y] -> next_y = min_y_next;
	
	break; 
      }

      heap_insert(h, m -> character_positions[rand_x][rand_y]);
      placed_characters++;
    }
  }

}

void characterLogic::move_pacer(generatedMap *m, NPC_char *pacer_to_move, heap_t *h, PC_state &PC_s, int manhattan_x, int manhattan_y, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv) {

  characterLogic characterLogic;
  
  int current_x = (int) pacer_to_move -> x_pos;
  int current_y = (int) pacer_to_move -> y_pos;

  int last_x;
  int last_y;

  if(m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] != NULL) {
    if(m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> player_type == PC &&
       m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos] -> battled == 0) {
      
      characterLogic.engage_battle(PC_s, manhattan_x, manhattan_y, pkmn_list, pkmn_st, pkmn_mv, mv);
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos] -> battled = 1;
      
    }
  }

  // Always try to move the pacer
  if((m -> generate_map[pacer_to_move -> next_x][pacer_to_move -> next_y] != tree) &&
     (m -> generate_map[pacer_to_move -> next_x][pacer_to_move -> next_y] != boulder) &&
     (m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] == NULL)) {

    current_x = pacer_to_move -> next_x;
    current_y = pacer_to_move -> next_y;

    last_x = pacer_to_move -> x_pos;
    last_y = pacer_to_move -> y_pos;
    
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] =  (NPC_char*) malloc(sizeof(NPC_char));
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> player_type = pacer;
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> cost_to_move = pacer_to_move -> cost_to_move;
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> cur_direction = pacer_to_move -> cur_direction;
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> x_pos = current_x;
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> y_pos = current_y;
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> battled = pacer_to_move -> battled;

    m -> character_positions[last_x][last_y] = NULL;
    free(m -> character_positions[last_x][last_y]);
  }

  // Always find the next move so we don't get stuck
  switch(pacer_to_move -> cur_direction) {
      
  case down:
    characterLogic.move_down(m, h, pacer_to_move, current_x, current_y);
    break;
      
  case up:
    characterLogic.move_up(m, h, pacer_to_move, current_x, current_y);
    break;
      
  case right:
    characterLogic.move_right(m, h, pacer_to_move, current_x, current_y);
    break;
      
  case left:
    characterLogic.move_left(m, h, pacer_to_move, current_x, current_y);     
    break;
    
  }
  
}

void characterLogic::move_up(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y) {

  movementCosts pathfind;
  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;

  if( character_to_move -> y_pos - 1 > 0 ) {
    if(
       (m -> generate_map[current_x][current_y - 1] != tree) &&
       (m -> generate_map[current_x][current_y - 1] != boulder) &&
       (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
       (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
       (m -> character_positions[current_x][current_y - 1] == NULL)) {
    
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost +  pathfind.determine_cost_rival(m, current_x, current_y - 1);
      m -> character_positions[current_x][current_y] -> next_x = current_x;
      m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
    
    
    }
    else {
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
      m -> character_positions[current_x][current_y] -> next_x = current_x;
      m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
      m -> character_positions[current_x][current_y] -> cur_direction = down;
    
    
    }
  }
 
  if(m -> character_positions[current_x][current_y - 1] != NULL) {
    if(m -> character_positions[current_x][current_y - 1] -> player_type == PC) {
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost +  pathfind.determine_cost_rival(m, current_x, current_y - 1);
      m -> character_positions[current_x][current_y] -> next_x = current_x;
      m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
     
     
    }
  }

  heap_insert(h, m -> character_positions[current_x][current_y]);
  
}

void characterLogic::move_down(generatedMap *m, heap_t *h, NPC_char *character_to_move,  int current_x, int current_y) {

  movementCosts pathfind;
  
  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;
  if( character_to_move -> y_pos + 1 < 21 ) {
    if(
       (m -> generate_map[current_x][current_y + 1] != tree) &&
       (m -> generate_map[current_x][current_y + 1] != boulder) &&
       (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
       (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
       (m -> character_positions[current_x][current_y + 1] == NULL)) {
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
      m -> character_positions[current_x][current_y] -> next_x = current_x;
      m -> character_positions[current_x][current_y] -> next_y  = current_y + 1;
	  
	  
    }
    else {
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
      m -> character_positions[current_x][current_y] -> next_x = current_x;
      m -> character_positions[current_x][current_y] -> next_y  = current_y - 1;
      m -> character_positions[current_x][current_y] -> cur_direction = up;
	  
	  
    }
  }

  if(m -> character_positions[current_x][current_y + 1] != NULL) {
    if(m -> character_positions[current_x][current_y + 1] -> player_type == PC) {
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
      m -> character_positions[current_x][current_y] -> next_x = current_x;
      m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	    
	   
    }
  }
  heap_insert(h, m -> character_positions[current_x][current_y]);
}

void characterLogic::move_left(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y) {

  movementCosts pathfind;
  
  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;

  if( character_to_move -> x_pos - 1 > 0 ) {
    if(
       (m -> generate_map[current_x - 1][current_y] != tree) &&
       (m -> generate_map[current_x - 1][current_y] != boulder) &&
       (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
       (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
       (m -> character_positions[current_x - 1][current_y] == NULL)) {
	  
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
      m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
      m -> character_positions[current_x][current_y] -> next_y = current_y;
	  
	  
    }
    else {
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
      m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
      m -> character_positions[current_x][current_y] -> next_y = current_y;
      m -> character_positions[current_x][current_y] -> cur_direction = right;
	  
	  
    }
  }

  if(m -> character_positions[current_x - 1][current_y] != NULL) {
    if(m -> character_positions[current_x - 1][current_y] -> player_type == PC) {
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
      m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
      m -> character_positions[current_x][current_y] -> next_y = current_y;
	    
	    
    }
  }

  heap_insert(h, m -> character_positions[current_x][current_y]);
}

void characterLogic::move_right(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y) {

  movementCosts pathfind;

  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;

  if( character_to_move -> x_pos + 1 < 80 ) {
    if(
       (m -> generate_map[current_x + 1][current_y] != tree) &&
       (m -> generate_map[current_x + 1][current_y] != boulder) &&
       (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
       (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
       (m -> character_positions[current_x + 1][current_y] == NULL)) {
	  
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
      m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
      m -> character_positions[current_x][current_y] -> next_y = current_y;
	  
	  
    }
    else {
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
      m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
      m -> character_positions[current_x][current_y] -> next_y = current_y;
      m -> character_positions[current_x][current_y] -> cur_direction = left;
	  
	  
    }
  }

  if(m -> character_positions[current_x + 1][current_y] != NULL) {
    if(m -> character_positions[current_x + 1][current_y] -> player_type == PC) {
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
      m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
      m -> character_positions[current_x][current_y] -> next_y = current_y;
	    
	    
    }
  }

  heap_insert(h, m -> character_positions[current_x][current_y]);
}


void characterLogic::move_via_shortest_path(generatedMap *m, movementCosts dijkstra[HORIZONTAL][VERTICAL], NPC_char *character_to_move, heap_t *h, PC_state &PC_s, int manhattan_x, int manhattan_y, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv) {

  int min_x_next;
  int min_y_next;

  // I really shouldn't have used INT_MAX because my comparisons are messed up but idk if I'm gonna change it
  int cost_to_move = INT_MAX;

  movementCosts pathfind;

  if(m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] != NULL) {
    if(m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> player_type == PC &&
       m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> battled == 0) {
      
      engage_battle(PC_s, manhattan_x, manhattan_y, pkmn_list, pkmn_st, pkmn_mv, mv);
      m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> battled = 1;
    }
  }

  // Check to see if we are going to run into another character
  if(m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] == NULL &&
     m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> battled != 1) {
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] = (NPC_char*) malloc(sizeof(NPC_char));
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> player_type = character_to_move -> player_type;
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> battled = character_to_move -> battled;
    
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> cost_to_move = m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> cost_to_move;
    
    
    if( dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y].cost < cost_to_move &&
	dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y].cost >= 0) {
      min_x_next = character_to_move -> next_x + 1;
      min_y_next = character_to_move -> next_y;
      
      cost_to_move = dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y].cost;
    }
    
    if( dijkstra[character_to_move -> next_x][character_to_move -> next_y + 1].cost < cost_to_move &&
	dijkstra[character_to_move -> next_x][character_to_move -> next_y + 1].cost >= 0) {
      min_x_next = character_to_move -> next_x;
      min_y_next = character_to_move -> next_y + 1;
      
      cost_to_move = dijkstra[character_to_move -> next_x][character_to_move -> next_y + 1].cost;
    }
    
    if( dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y].cost < cost_to_move &&
	dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y].cost >= 0) {
      min_x_next = character_to_move -> next_x - 1;
      min_y_next = character_to_move -> next_y;
      
      cost_to_move = dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y].cost;
    }
    
    if( dijkstra[character_to_move -> next_x][character_to_move -> y_pos - 1].cost < cost_to_move &&
	dijkstra[character_to_move -> next_x][character_to_move -> next_y - 1].cost >= 0) {
      min_x_next = character_to_move -> next_x;
      min_y_next = character_to_move -> next_y - 1;
      
      cost_to_move = dijkstra[character_to_move -> next_x][character_to_move -> next_y - 1].cost;
    }
    
    if( dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y + 1].cost < cost_to_move &&
	dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y + 1].cost >= 0) {
      min_x_next = character_to_move -> next_x + 1;
      min_y_next = character_to_move -> next_y + 1;
      
      cost_to_move = dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y + 1].cost;
    }
    
    
    if( dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y - 1].cost < cost_to_move &&
	dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y - 1].cost >= 0) {
      min_x_next = character_to_move -> next_x + 1;
      min_y_next = character_to_move -> next_y - 1;
      
      cost_to_move = dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y - 1].cost;
    }
    
    
    if( dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y + 1].cost < cost_to_move &&
	dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y + 1].cost >= 0) {
      min_x_next = character_to_move -> next_x - 1;
      min_y_next = character_to_move -> next_y + 1;
      
      cost_to_move = dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y + 1].cost;
    }
    
    
    if( dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y - 1].cost < cost_to_move &&
	dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y - 1].cost >= 0 ) {
      min_x_next = character_to_move -> next_x - 1;
      min_y_next = character_to_move -> next_y - 1;
  
      cost_to_move = dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y - 1].cost;
    }

    int x_position = m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> next_x;
    int y_position = m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> next_y;
    
    int prev_x = m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> x_pos;
    int prev_y = m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> y_pos;
    
    if(character_to_move -> player_type == rival) {
      m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> cost_to_move += pathfind.determine_cost_rival(m, min_x_next, min_y_next);
    }
    else {
      m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> cost_to_move += pathfind.determine_cost_hiker(m, min_x_next, min_y_next);
    }
    
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> next_x = min_x_next;
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> next_y = min_y_next;
    
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> x_pos = x_position;
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> y_pos = y_position;
    
    m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] = NULL;
    free(m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos]);
    heap_insert(h, m -> character_positions[x_position][y_position]);

  }
  else {
    if(character_to_move -> player_type == rival) {
      m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> cost_to_move += pathfind.determine_cost_rival(m, character_to_move -> x_pos, character_to_move -> y_pos);
    }
    else {
      m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> cost_to_move += pathfind.determine_cost_hiker(m, character_to_move -> x_pos, character_to_move -> y_pos);
    }
    heap_insert(h, m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos]);
  }
}



void characterLogic::move_wanderer(generatedMap *m, NPC_char *wanderer_to_move, heap_t *h, PC_state &PC_s, int manhattan_x, int manhattan_y, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv) {

  int current_x = wanderer_to_move -> x_pos;
  int current_y = wanderer_to_move -> y_pos;

  int last_x;
  int last_y;

  if(m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] != NULL) {
    if((m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> player_type == PC) &&
       (m -> character_positions[wanderer_to_move -> x_pos][wanderer_to_move -> y_pos] -> battled == 0)) {
      
      engage_battle(PC_s, manhattan_x, manhattan_y, pkmn_list, pkmn_st, pkmn_mv, mv);
      m -> character_positions[wanderer_to_move -> x_pos][wanderer_to_move -> y_pos] -> battled = 1;
      
    }
  }

  // Check to see if the move is valid first
  if((m -> generate_map[wanderer_to_move -> next_x][wanderer_to_move -> next_y] != tree) &&
     (m -> generate_map[wanderer_to_move -> next_x][wanderer_to_move -> next_y] != boulder) &&
     (m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] == NULL)) {

    current_x = wanderer_to_move -> next_x;
    current_y = wanderer_to_move -> next_y;

    last_x = wanderer_to_move -> x_pos;
    last_y = wanderer_to_move -> y_pos;
    
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] = (NPC_char*) malloc(sizeof(NPC_char));
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> player_type = wanderer;
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> cost_to_move = wanderer_to_move -> cost_to_move;
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> cur_direction = wanderer_to_move -> cur_direction;
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> terrain_type = wanderer_to_move -> terrain_type;
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> x_pos = current_x;
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> y_pos = current_y;
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> battled = wanderer_to_move -> battled;
    
    m -> character_positions[last_x][last_y] = NULL;
    free(m -> character_positions[last_x][last_y]);

  }

  // Always check for a new move or we will stop
  // If we find we can't move the assigned direction,
  // We will attempt to move a random direction that
  // Isn't the same direction we were just moving
  switch(wanderer_to_move -> cur_direction) {
  case down:
    move_down_random(m, h, wanderer_to_move, current_x, current_y);
    break;
  case up:
    move_up_random(m, h, wanderer_to_move, current_x, current_y);
    break;
  case right:
    move_right_random(m, h, wanderer_to_move, current_x, current_y);
    break;
  case left:
    move_left_random(m, h, wanderer_to_move, current_x, current_y);     
    break;
  }

}

void characterLogic::move_up_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y) {

  movementCosts pathfind;

  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;

  if( character_to_move -> y_pos - 1 >= 0 ) {
    if(
       (m -> generate_map[current_x][current_y - 1] != tree) &&
       (m -> generate_map[current_x][current_y - 1] != boulder) &&
       (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
       (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
       (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1]) &&
       (m -> character_positions[current_x][current_y - 1] == NULL)) {
	  
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
      m -> character_positions[current_x][current_y] -> next_x = current_x;
      m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	  
	  
    }
    else {
	  
      direction direction_to_move;
      int valid = 0;

      while(!valid) {

	direction_to_move = static_cast<direction>(rand() % 3);
	  
	switch (direction_to_move) {
	      
	case 0:
	  if(( m -> generate_map[current_x][current_y + 1] != tree) &&
	     (m -> generate_map[current_x][current_y + 1] != boulder) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y + 1]) &&
	     (m -> character_positions[current_x][current_y + 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = down;
	    valid = 1;
	  }

	  break;
	      
	case 1:
	  if(( m -> generate_map[current_x + 1][current_y] != tree) &&
	     (m -> generate_map[current_x + 1][current_y] != boulder) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x + 1][current_y]) &&
	     (m -> character_positions[current_x + 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = right;
	    valid = 1;
	  }

	  break;
	      
	case 2:
	  if(( m -> generate_map[current_x - 1][current_y] != tree) &&
	     (m -> generate_map[current_x - 1][current_y] != boulder) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x - 1][current_y]) &&
	     (m -> character_positions[current_x - 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = left;
	    valid = 1;
	  }

	  break;
	}
      }	  
	  
	  
    }
    if(m -> character_positions[current_x][current_y - 1] != NULL &&
       (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1])) {
      if((m -> character_positions[current_x][current_y - 1] -> player_type == PC)) {
	char buffer[50];
	sprintf(buffer, "got into up PC assignment");
	mvaddstr(22, 5, buffer);
	refresh();
	m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
	m -> character_positions[current_x][current_y] -> next_x = current_x;
	m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	    
	    
      }
    }

    heap_insert(h, m -> character_positions[current_x][current_y]); 
  }
  
}

void characterLogic::move_down_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y) {
  
  movementCosts pathfind;
  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;

  if( character_to_move -> y_pos + 1 < 21 ) {
    if(
       (m -> generate_map[current_x][current_y + 1] != tree) &&
       (m -> generate_map[current_x][current_y + 1] != boulder) &&
       (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
       (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
       (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y + 1]) &&
       (m -> character_positions[current_x][current_y + 1] == NULL)) {
	  
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
      m -> character_positions[current_x][current_y] -> next_x = current_x;
      m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	  
	  
    }
	
    else {
	  
      direction direction_to_move;
      int valid = 0;

      while(!valid) {


	direction_to_move = static_cast<direction>(rand() % 3);
	  
	switch (direction_to_move) {
	      
	case 0:
	  if(( m -> generate_map[current_x][current_y - 1] != tree) &&
	     (m -> generate_map[current_x][current_y - 1] != boulder) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1]) &&
	     (m -> character_positions[current_x][current_y - 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = up;
	    valid = 1;
	  }

	  break;
	      
	case 1:
	  if(( m -> generate_map[current_x + 1][current_y] != tree) &&
	     (m -> generate_map[current_x + 1][current_y] != boulder) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x + 1][current_y]) &&
	     (m -> character_positions[current_x + 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = right;
	    valid = 1;
	  }

	  break;
	      
	case 2:
	  if(( m -> generate_map[current_x - 1][current_y] != tree) &&
	     (m -> generate_map[current_x - 1][current_y] != boulder) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x - 1][current_y]) &&
	     (m -> character_positions[current_x - 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = left;
	    valid = 1;
	  }

	  break;
	}
      }
	  
	  
	  
    }
	
    if(m -> character_positions[current_x][current_y + 1] != NULL &&
       (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1])) {
      if(m -> character_positions[current_x][current_y + 1] -> player_type == PC) {
	char buffer[50];
	sprintf(buffer, "got into down PC assignment");
	mvaddstr(22, 5, buffer);
	refresh();
	m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
	m -> character_positions[current_x][current_y] -> next_x = current_x;
	m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	    
	   
      }
    }
    heap_insert(h, m -> character_positions[current_x][current_y]); 
  }
  
}

void characterLogic::move_right_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y) {

  movementCosts pathfind;

  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;

  if( character_to_move -> x_pos + 1 < 80 ) {
    if(
       (m -> generate_map[current_x + 1][current_y] != tree) &&
       (m -> generate_map[current_x + 1][current_y] != boulder) &&
       (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
       (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
       (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x + 1][current_y]) &&
       (m -> character_positions[current_x + 1][current_y] == NULL)) {
	  
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
      m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
      m -> character_positions[current_x][current_y] -> next_y = current_y;
	  
	  
    }
    else {
	  
      direction direction_to_move;
      int valid = 0;

      while(!valid) {

	direction_to_move = static_cast<direction>(rand() % 3);
	  
	switch (direction_to_move) {
	      
	case 0:
	  if(( m -> generate_map[current_x][current_y + 1] != tree) &&
	     (m -> generate_map[current_x][current_y + 1] != boulder) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y + 1]) &&
	     (m -> character_positions[current_x][current_y + 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = down;
	    valid = 1;
	  }

	  break;
	      
	case 1:
	  if(( m -> generate_map[current_x][current_y - 1] != tree) &&
	     (m -> generate_map[current_x][current_y - 1] != boulder) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1]) &&
	     (m -> character_positions[current_x][current_y - 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = up;
	    valid = 1;
	  }

	  break;
	      
	case 2:
	  if(( m -> generate_map[current_x - 1][current_y] != tree) &&
	     (m -> generate_map[current_x - 1][current_y] != boulder) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x - 1][current_y]) &&
	     (m -> character_positions[current_x - 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = left;
	    valid = 1;
	  }

	  break;
	}
      }
	  
	  
	  
    }

    if(m -> character_positions[current_x + 1][current_y] != NULL &&
       (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1])) {
      if(m -> character_positions[current_x + 1][current_y] -> player_type == PC) {
	char buffer[50];
	sprintf(buffer, "got into right PC assignment");
	mvaddstr(22, 5, buffer);
	refresh();
	m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
	m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	m -> character_positions[current_x][current_y] -> next_y = current_y;
	    
	    
      }
    }
    heap_insert(h, m -> character_positions[current_x][current_y]);
  }
  
}

void characterLogic::move_left_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y) {

  movementCosts pathfind;

  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;

  if( character_to_move -> x_pos - 1 >= 0 ) {
    if(
       (m -> generate_map[current_x - 1][current_y] != tree) &&
       (m -> generate_map[current_x - 1][current_y] != boulder) &&
       (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
       (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
       (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x - 1][current_y]) &&
       (m -> character_positions[current_x - 1][current_y] == NULL)) {
	  
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost +  pathfind.determine_cost_rival(m, current_x - 1, current_y);
      m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
      m -> character_positions[current_x][current_y] -> next_y = current_y;
	  

    }
    else {
	  
      direction direction_to_move;
      int valid = 0;

      while(!valid) {

	direction_to_move = static_cast<direction>(rand() % 3);
	  
	switch (direction_to_move) {
	      
	case 0:
	  if(( m -> generate_map[current_x][current_y + 1] != tree) &&
	     (m -> generate_map[current_x][current_y + 1] != boulder) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y + 1]) &&
	     (m -> character_positions[current_x][current_y + 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost +  pathfind.determine_cost_rival(m, current_x, current_y + 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = down;
	    valid = 1;
	  }

	  break;
	      
	case 1:
	  if(( m -> generate_map[current_x + 1][current_y] != tree) &&
	     (m -> generate_map[current_x + 1][current_y] != boulder) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x + 1][current_y]) &&
	     (m -> character_positions[current_x + 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = right;
	    valid = 1;
	  }

	  break;
	      
	case 2:
	  if(( m -> generate_map[current_x][current_y - 1] != tree) &&
	     (m -> generate_map[current_x][current_y - 1] != boulder) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1]) &&
	     (m -> character_positions[current_x][current_y - 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost +  pathfind.determine_cost_rival(m, current_x, current_y - 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = up;
	    valid = 1;
	  }

	  break;
	}
      }
     
    }

    if(m -> character_positions[current_x - 1][current_y] != NULL &&
       (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1])) {
      if(m -> character_positions[current_x - 1][current_y] -> player_type == PC) {
	char buffer[50];
	sprintf(buffer, "got into left PC assignment");
	mvaddstr(22, 5, buffer);
	refresh();
	m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
	m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	m -> character_positions[current_x][current_y] -> next_y = current_y;
      }
    }
    heap_insert(h, m -> character_positions[current_x][current_y]);
  }
  
}

void characterLogic::move_random_walker(generatedMap *m, NPC_char *walker_to_move, heap_t *h, PC_state &PC_s, int manhattan_x, int manhattan_y, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv) {

  characterLogic characterLogic;

  int current_x = walker_to_move -> x_pos;
  int current_y = walker_to_move -> y_pos;;

  int last_x;
  int last_y;

  if(m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] != NULL) {
    if(m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> player_type == PC &&
       m -> character_positions[walker_to_move -> x_pos][walker_to_move -> y_pos] -> battled == 0) {
      
      engage_battle(PC_s, manhattan_x, manhattan_y, pkmn_list, pkmn_st, pkmn_mv, mv);
      m -> character_positions[walker_to_move -> x_pos][walker_to_move -> y_pos] -> battled = 1;
      
    }
  }

  // We only want to move if it's valid
  if((m -> generate_map[walker_to_move -> next_x][walker_to_move -> next_y] != tree) &&
     (m -> generate_map[walker_to_move -> next_x][walker_to_move -> next_y] != boulder) &&
     (m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] == NULL)) {

    current_x = walker_to_move -> next_x;
    current_y = walker_to_move -> next_y;

    last_x = walker_to_move -> x_pos;
    last_y = walker_to_move -> y_pos;
    
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] = (NPC_char*) malloc(sizeof(NPC_char));
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> player_type = random_walker;
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> cost_to_move = walker_to_move -> cost_to_move;
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> cur_direction = walker_to_move -> cur_direction;
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> x_pos = current_x;
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> y_pos = current_y;
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> battled = walker_to_move -> battled;
    
    
    m -> character_positions[last_x][last_y] = NULL;
    free(m -> character_positions[last_x][last_y]);

  }

  // Always check for a new move or we will stop
  // Random walker moves are a like wanderer moves
  // But they don't check for same terrain
  switch(walker_to_move -> cur_direction) {
  case down:
    characterLogic.move_down_walker(m, h, walker_to_move, current_x, current_y);
    break;
  case up:
    characterLogic.move_up_walker(m, h, walker_to_move, current_x, current_y);
    break;
  case right:
    characterLogic.move_right_walker(m, h, walker_to_move, current_x, current_y);
    break;
  case left:
    characterLogic.move_left_walker(m, h, walker_to_move, current_x, current_y);     
    break;
    
  }
}

void characterLogic::move_left_walker(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y) {

  movementCosts pathfind;

  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;

  if( character_to_move -> x_pos - 1 > 0 ) {
    if(
       (m -> generate_map[current_x - 1][current_y] != tree) &&
       (m -> generate_map[current_x - 1][current_y] != boulder) &&
       (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
       (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
       (m -> character_positions[current_x - 1][current_y] == NULL)) {
	 
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
      m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
      m -> character_positions[current_x][current_y] -> next_y = current_y;
	 
	 
    }
    else {
	 
      direction direction_to_move;
      int valid = 0;
	 
      while(!valid) {
	   
	direction_to_move = static_cast<direction>(rand() % 3);
	   
	switch (direction_to_move) {
	     
	case 0:
	  if(( m -> generate_map[current_x][current_y + 1] != tree) &&
	     (m -> generate_map[current_x][current_y + 1] != boulder) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y + 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move += pathfind.determine_cost_rival(m, current_x, current_y + 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = down;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x][current_y + 1] != NULL) {
	    if(m -> character_positions[current_x][current_y + 1] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
	      m -> character_positions[current_x][current_y] -> next_x = current_x;
	      m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	      m -> character_positions[current_x][current_y] -> cur_direction = down;
	      valid = 1;
	    }
	  }
	  break;
	     
	case 1:
	  if(( m -> generate_map[current_x + 1][current_y] != tree) &&
	     (m -> generate_map[current_x + 1][current_y] != boulder) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x + 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = right;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x + 1][current_y] != NULL) {
	    if(m -> character_positions[current_x + 1][current_y] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
	      m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	      m -> character_positions[current_x][current_y] -> next_y = current_y;
	      m -> character_positions[current_x][current_y] -> cur_direction = right;
	      valid = 1;
	    }
	  }
	  break;
	     
	case 2:
	  if(( m -> generate_map[current_x][current_y - 1] != tree) &&
	     (m -> generate_map[current_x][current_y - 1] != boulder) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y - 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = up;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x][current_y - 1] != NULL) {
	    if(m -> character_positions[current_x][current_y - 1] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
	      m -> character_positions[current_x][current_y] -> next_x = current_x;
	      m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	      m -> character_positions[current_x][current_y] -> cur_direction = up;
	      valid = 1;
	    }
	  }
	  break;
	}
      }
	 
	
	 
    }

    if ( m -> character_positions[current_x - 1][current_y] != NULL) {
      if (m -> character_positions[current_x - 1][current_y] -> player_type == PC) {
	m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
	m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	m -> character_positions[current_x][current_y] -> next_y = current_y;
	 
      }
	 
    }

    heap_insert(h, m -> character_positions[current_x][current_y]);
  }
}

void characterLogic::move_right_walker(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y) {

  movementCosts pathfind;

  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;

  if( character_to_move -> x_pos + 1 < 80 ) {
    if(
       (m -> generate_map[current_x + 1][current_y] != tree) &&
       (m -> generate_map[current_x + 1][current_y] != boulder) &&
       (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
       (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
       (m -> character_positions[current_x + 1][current_y] == NULL)) {
	  
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
      m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
      m -> character_positions[current_x][current_y] -> next_y = current_y;
	  
	  
    }
    else {
	  
      direction direction_to_move;
      int valid = 0;

      while(!valid) {

	direction_to_move = static_cast<direction>(rand() % 3);
	  
	switch (direction_to_move) {
	      
	case 0:
	  if(( m -> generate_map[current_x][current_y + 1] != tree) &&
	     (m -> generate_map[current_x][current_y + 1] != boulder) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y + 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = down;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x][current_y + 1] != NULL) {
	    if(m -> character_positions[current_x][current_y + 1] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
	      m -> character_positions[current_x][current_y] -> next_x = current_x;
	      m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	      m -> character_positions[current_x][current_y] -> cur_direction = down;
	      valid = 1;
	    }
	  }
	  break;
	      
	case 1:
	  if(( m -> generate_map[current_x - 1][current_y] != tree) &&
	     (m -> generate_map[current_x - 1][current_y] != boulder) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x - 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = left;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x - 1][current_y] != NULL) {
	    if(m -> character_positions[current_x - 1][current_y] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
	      m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	      m -> character_positions[current_x][current_y] -> next_y = current_y;
	      m -> character_positions[current_x][current_y] -> cur_direction = left;
	      valid = 1;
	    }
	  }
	  break;
	      
	case 2:
	  if(( m -> generate_map[current_x][current_y - 1] != tree) &&
	     (m -> generate_map[current_x][current_y - 1] != boulder) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y - 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = up;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x][current_y - 1] != NULL) {
	    if(m -> character_positions[current_x][current_y - 1] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
	      m -> character_positions[current_x][current_y] -> next_x = current_x;
	      m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	      m -> character_positions[current_x][current_y] -> cur_direction = up;
	      valid = 1;
	    }
	  }
	  break;
	}
      }
	  
	  
	  
    }
    if(m -> character_positions[current_x + 1][current_y] != NULL) {
      if(m -> character_positions[current_x + 1][current_y] -> player_type == PC) {
	m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
	m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	m -> character_positions[current_x][current_y] -> next_y = current_y;
	    
	   
      }
    }
    heap_insert(h, m -> character_positions[current_x][current_y]);
  }
  
}

void characterLogic::move_up_walker(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y) {

  movementCosts pathfind;

  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;

  if( character_to_move -> y_pos - 1 > 0 ) {
    if(
       (m -> generate_map[current_x][current_y - 1] != tree) &&
       (m -> generate_map[current_x][current_y - 1] != boulder) &&
       (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
       (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
       (m -> character_positions[current_x][current_y - 1] == NULL)) {
	  
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
      m -> character_positions[current_x][current_y] -> next_x = current_x;
      m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	  
	  
    }

    else {
	  
      direction direction_to_move;
      int valid = 0;

      while(!valid) {

	direction_to_move = static_cast<direction>(rand() % 3);
	  
	switch (direction_to_move) {
	      
	case 0:
	  if(( m -> generate_map[current_x][current_y + 1] != tree) &&
	     (m -> generate_map[current_x][current_y + 1] != boulder) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y + 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = down;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x][current_y + 1] != NULL) {
	    if(m -> character_positions[current_x][current_y + 1] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
	      m -> character_positions[current_x][current_y] -> next_x = current_x;
	      m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	      m -> character_positions[current_x][current_y] -> cur_direction = down;
	      valid = 1;
	    }
	  }
	  break;
	      
	case 1:
	  if(( m -> generate_map[current_x + 1][current_y] != tree) &&
	     (m -> generate_map[current_x + 1][current_y] != boulder) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x + 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = right;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x + 1][current_y] != NULL) {
	    if(m -> character_positions[current_x + 1][current_y] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
	      m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	      m -> character_positions[current_x][current_y] -> next_y = current_y;
	      m -> character_positions[current_x][current_y] -> cur_direction = right;
	      valid = 1;
	    }
	  }
	  break;
	      
	case 2:
	  if(( m -> generate_map[current_x - 1][current_y] != tree) &&
	     (m -> generate_map[current_x - 1][current_y] != boulder) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x - 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = left;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x - 1][current_y] != NULL) {
	    if(m -> character_positions[current_x - 1][current_y] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
	      m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	      m -> character_positions[current_x][current_y] -> next_y = current_y;
	      m -> character_positions[current_x][current_y] -> cur_direction = left;
	      valid = 1;
	    }
	  }
	  break;
	}
      }
	  
	  
	  
    }
    if(m -> character_positions[current_x][current_y - 1] != NULL) {
      if(m -> character_positions[current_x][current_y - 1] -> player_type == PC) {
	m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
	m -> character_positions[current_x][current_y] -> next_x = current_x;
	m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	    
	    
      }
    }
    heap_insert(h, m -> character_positions[current_x][current_y]);
  }
  
}

void characterLogic::move_down_walker(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y) {

  movementCosts pathfind;

  int current_cost = m -> character_positions[current_x][current_y] -> cost_to_move;

  if( character_to_move -> y_pos + 1 < 21 ) {
    if(
       (m -> generate_map[current_x][current_y + 1] != tree) &&
       (m -> generate_map[current_x][current_y + 1] != boulder) &&
       (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
       (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
       (m -> character_positions[current_x][current_y + 1] == NULL)) {
	  
      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
      m -> character_positions[current_x][current_y] -> next_x = current_x;
      m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	  
	  
    }
    else {
	  
      direction direction_to_move;
      int valid = 0;

      while(!valid) {

	direction_to_move = static_cast<direction>(rand() % 3);
	  
	switch (direction_to_move) {
	      
	case 0:
	  if(( m -> generate_map[current_x][current_y - 1] != tree) &&
	     (m -> generate_map[current_x][current_y - 1] != boulder) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
	     (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
	     (m -> character_positions[current_x][current_y - 1] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
	    m -> character_positions[current_x][current_y] -> next_x = current_x;
	    m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	    m -> character_positions[current_x][current_y] -> cur_direction = up;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x][current_y - 1] != NULL) {
	    if(m -> character_positions[current_x][current_y - 1] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y - 1);
	      m -> character_positions[current_x][current_y] -> next_x = current_x;
	      m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	      m -> character_positions[current_x][current_y] -> cur_direction = up;
	      valid = 1;
	    }
	  }
	  break;
	      
	case 1:
	  if(( m -> generate_map[current_x + 1][current_y] != tree) &&
	     (m -> generate_map[current_x + 1][current_y] != boulder) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x + 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = right;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x + 1][current_y] != NULL) {
	    if(m -> character_positions[current_x + 1][current_y] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x + 1, current_y);
	      m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	      m -> character_positions[current_x][current_y] -> next_y = current_y;
	      m -> character_positions[current_x][current_y] -> cur_direction = right;
	      valid = 1;
	    }
	  }
	  break;
	      
	case 2:
	  if(( m -> generate_map[current_x - 1][current_y] != tree) &&
	     (m -> generate_map[current_x - 1][current_y] != boulder) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
	     (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
	     (m -> character_positions[current_x - 1][current_y] == NULL)) {
	    m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
	    m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	    m -> character_positions[current_x][current_y] -> next_y = current_y;
	    m -> character_positions[current_x][current_y] -> cur_direction = left;
	    valid = 1;
	  }
	  else if (m -> character_positions[current_x - 1][current_y] != NULL) {
	    if(m -> character_positions[current_x - 1][current_y] -> player_type == PC) {
	      m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x - 1, current_y);
	      m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	      m -> character_positions[current_x][current_y] -> next_y = current_y;
	      m -> character_positions[current_x][current_y] -> cur_direction = left;
	      valid = 1;
	    }
	  }
	  break;
	}
      }
	  
	  
    }
    if(m -> character_positions[current_x][current_y + 1] != NULL) {
      if(m -> character_positions[current_x][current_y + 1] -> player_type == PC) {
	m -> character_positions[current_x][current_y] -> cost_to_move = current_cost + pathfind.determine_cost_rival(m, current_x, current_y + 1);
	m -> character_positions[current_x][current_y] -> next_x = current_x;
	m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	    
	    
      }
    }
    heap_insert(h, m -> character_positions[current_x][current_y]);
  }
  
}

void characterLogic::attempt_move_PC(int x_move, int y_move, generatedMap *m, heap_t *h, int *PC_added_to_heap) {

  movementCosts pathfind;
  char buffer[50];

  if(x_move >= 0
     && x_move < 80
     && y_move >= 0
     && y_move < 21) {
    if(m -> generate_map[x_move][y_move] != boulder &&
       m -> generate_map[x_move][y_move] != tree) {
      m -> character_positions[m -> PC_position_x][m -> PC_position_y] -> next_x = x_move;
      m -> character_positions[m -> PC_position_x][m -> PC_position_y] -> next_y = y_move;

      if(m -> character_positions[x_move][y_move] != NULL) {
	  m -> character_positions[m -> PC_position_x][m -> PC_position_y] -> cost_to_move = m -> game_time + 10;
      }
      else {	
	m -> character_positions[m -> PC_position_x][m -> PC_position_y] -> cost_to_move = m -> game_time + pathfind.determine_cost_PC(m, x_move, y_move);
      }

      sprintf(buffer, "Next move: %d", m -> character_positions[m -> PC_position_x][m -> PC_position_y] -> cost_to_move);
      mvaddstr(23, 0, buffer);

      int x = m -> PC_position_x;
      int y = m -> PC_position_y;
      
      heap_insert(h, m -> character_positions[x][y]);
      *PC_added_to_heap = 1;
    }
  }
  
}

void characterLogic::move_PC(PC_char *player_char, generatedMap *m, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, int manhattan_x, int manhattan_y, PC_state &PC_s) {

  int prev_x = player_char -> x_pos;
  int prev_y = player_char -> y_pos;

  int next_x_move = player_char -> next_x;
  int next_y_move = player_char -> next_y;

  if(m -> character_positions[player_char -> next_x][player_char -> next_y] != NULL) {
    char buffer[50];
    sprintf(buffer, "battled: %d", m -> character_positions[player_char -> next_x][player_char -> next_y] -> battled);
    mvaddstr(21, 0, buffer);
    refresh();
  }
  
  if(m -> character_positions[player_char -> next_x][player_char -> next_y] == NULL) {
    
    m -> character_positions[next_x_move][next_y_move] = (PC_char*) malloc(sizeof(PC_char));
    m -> character_positions[next_x_move][next_y_move] -> player_type = PC;
    
    m -> character_positions[next_x_move][next_y_move] -> x_pos = next_x_move;
    m -> character_positions[next_x_move][next_y_move] -> y_pos = next_y_move;

    m -> PC_position_x = next_x_move;
    m -> PC_position_y = next_y_move;

    m -> character_positions[prev_x][prev_y] = NULL;
    free(m -> character_positions[prev_x][prev_y]);

    if (m -> generate_map[next_x_move][next_y_move] == tall_grass) {
      battle wild_encounter;

      int enter_battle = 0;
      
      enter_battle = wild_encounter.determine_battle();

      if(enter_battle) {
	wild_encounter.engage_battle_wild(pkmn_list, pkmn_st, pkmn_mv, mv, manhattan_x, manhattan_y);
      }
    }
    
  }
  else if (m -> character_positions[next_x_move][next_y_move] -> battled == 0) {
     
    engage_battle(PC_s, manhattan_x, manhattan_y, pkmn_list, pkmn_st, pkmn_mv, mv);
    m -> character_positions[next_x_move][next_y_move] -> battled = 1;
      
  }
}
