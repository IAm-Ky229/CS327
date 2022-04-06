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

// These have to be static, so they have to be in the main game loop file
static int32_t path_cmp(const void *key, const void *with) {
  return ((movementCosts *) key)->cost - ((movementCosts *) with)->cost;
}

// These have to be static, so they have to be in the main game loop file
static int32_t move_cost_cmp(const void *key, const void *with) {
  return ((characterData *) key)-> cost_to_move - ((characterData *) with)-> cost_to_move;
}

// These have to be static, so they have to be in the main game loop file
static void dijkstra_path_rival(generatedMap *m, movementCosts dijkstra[HORIZONTAL][VERTICAL], int from_x, int from_y)
{

  // We'll use this to store and compute costs
  movementCosts  *p, computeCost;

  // Heap and initialization variables
  heap_t h;
  uint32_t x, y;
  
  // Initialize all the positions
  for(y = 0; y < VERTICAL; y++) {
    for(x = 0; x < HORIZONTAL; x++) {
      dijkstra[x][y].x_pos = x;
      dijkstra[x][y].y_pos = y;
      dijkstra[x][y].visited = 0;
    }
  }
  
  // Set all costs to be MAX at first
  for (y = 0; y < VERTICAL; y++) {
    for (x = 0; x < HORIZONTAL; x++) {
      dijkstra[x][y].cost = INT_MAX;
    }
  }

  // Set starting node distance 0
  dijkstra[from_x][from_y].cost = 0;
  
  heap_init(&h, path_cmp, NULL);
  
  // Insert the first node
  dijkstra[from_x][from_y].hn = heap_insert(&h, &dijkstra[from_x][from_y]);

  // Extract the best (cheapest) node in the heap
  while ((p = (movementCosts*) heap_remove_min(&h))) {
    p->hn = NULL;

    // In general: check bounds
    // check if the cost to go a certain direction is less than what is already there
    // If it is less, update the cost
    // Else do nothing
    
    // Try to go left
    if(p-> x_pos - 1 > 0) {
      
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos    ].cost) >
	  (p-> cost + computeCost.determine_cost_rival(m, p-> x_pos - 1, p -> y_pos)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos    ].cost =
	  ((p->cost + computeCost.determine_cost_rival(m, p-> x_pos - 1, p -> y_pos)));
	dijkstra[p->x_pos - 1][p->y_pos    ].next_x = p -> x_pos;
	dijkstra[p->x_pos - 1][p->y_pos    ].next_y = p -> y_pos;
      }
    }
    
    // Try to go right
    if(p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos    ].cost) >
	  (p->cost + computeCost.determine_cost_rival(m, p-> x_pos + 1, p -> y_pos)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos    ].cost =
	  ((p->cost + computeCost.determine_cost_rival(m, p-> x_pos + 1, p -> y_pos)));
	dijkstra[p->x_pos + 1][p->y_pos    ].next_x = p -> x_pos;
	dijkstra[p->x_pos + 1][p->y_pos    ].next_y = p -> y_pos;
      }
    }

    // Try to go down
    if(p->y_pos - 1 > 0) {
      if (
	  (dijkstra[p->x_pos][p->y_pos - 1    ].cost) >
	  (p->cost + computeCost.determine_cost_rival(m, p-> x_pos, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos][p->y_pos - 1    ].cost =
	  ((p->cost + computeCost.determine_cost_rival(m, p-> x_pos, p -> y_pos - 1)));
	dijkstra[p->x_pos][p->y_pos - 1].next_x = p -> x_pos;
	dijkstra[p->x_pos][p->y_pos - 1].next_y = p -> y_pos;
      }
    }
    
    // Try to go up
    if(p-> y_pos + 1 < VERTICAL) {
      if (
	  (dijkstra[p->x_pos][p->y_pos + 1    ].cost) >
	  (p->cost + computeCost.determine_cost_rival(m, p-> x_pos, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos][p->y_pos + 1    ].cost =
	  ((p->cost + computeCost.determine_cost_rival(m, p-> x_pos, p -> y_pos + 1)));
	dijkstra[p->x_pos][p->y_pos + 1].next_x = p -> x_pos;
	dijkstra[p->x_pos][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    // Try to go up right
    if(p-> y_pos + 1 < VERTICAL && p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos + 1    ].cost) >
	  (p->cost + computeCost.determine_cost_rival(m, p-> x_pos + 1, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos + 1    ].cost =
	  ((p->cost + computeCost.determine_cost_rival(m, p-> x_pos + 1, p -> y_pos + 1)));
	dijkstra[p->x_pos + 1][p->y_pos + 1].next_x = p -> x_pos;
	dijkstra[p->x_pos + 1][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    // Try to go down right
    if(p-> y_pos - 1 < VERTICAL && p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos + 1    ].cost) >
	  (p->cost + computeCost.determine_cost_rival(m, p-> x_pos - 1, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos + 1    ].cost =
	  ((p->cost + computeCost.determine_cost_rival(m, p-> x_pos - 1, p -> y_pos + 1)));
	dijkstra[p->x_pos - 1][p->y_pos + 1].next_x = p -> x_pos;
	dijkstra[p->x_pos - 1][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    // Try to go down left
    if(p-> y_pos - 1 < VERTICAL && p-> x_pos - 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos - 1    ].cost) >
	  (p->cost + computeCost.determine_cost_rival(m, p-> x_pos - 1, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos - 1    ].cost =
	  ((p->cost + computeCost.determine_cost_rival(m, p-> x_pos - 1, p -> y_pos - 1)));
	dijkstra[p->x_pos - 1][p->y_pos - 1].next_x = p -> x_pos;
	dijkstra[p->x_pos - 1][p->y_pos - 1].next_y = p -> y_pos;
      }
    }

    // Try to go up left
    if(p-> y_pos + 1 < VERTICAL && p-> x_pos - 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos - 1    ].cost) >
	  (p->cost + computeCost.determine_cost_rival(m, p-> x_pos + 1, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos - 1    ].cost =
	  ((p->cost + computeCost.determine_cost_rival(m, p-> x_pos + 1, p -> y_pos - 1)));
	dijkstra[p->x_pos + 1][p->y_pos - 1].next_x = p -> x_pos;
	dijkstra[p->x_pos + 1][p->y_pos - 1].next_y = p -> y_pos;
      }
    }
    
    // Check for valid right neighbor
    if( p-> x_pos + 1 < HORIZONTAL && p -> x_pos - 1 > 0 ) {
      if(dijkstra[p->x_pos + 1][p->y_pos].visited != 1 && computeCost.determine_cost_rival(m, p-> x_pos + 1, p -> y_pos) != INT_MAX) {
	dijkstra[p->x_pos + 1][p->y_pos].visited = 1;
	dijkstra[p-> x_pos + 1][p -> y_pos].hn = heap_insert(&h, &dijkstra[p-> x_pos + 1][p-> y_pos]);  
      }
    }

    // In general:
    // Check to see if neighbor in bounds
    // If neigbor has NOT been marked as visited, add to the heap
    // Otherwise do nothing
    
    // Check for valid left neighbor
    if( p-> x_pos - 1 > 0 && p -> x_pos + 1 < HORIZONTAL) {
      if(dijkstra[p->x_pos - 1][p->y_pos].visited != 1  && computeCost.determine_cost_rival(m, p-> x_pos - 1, p -> y_pos) != INT_MAX) {
	dijkstra[p->x_pos - 1][p->y_pos].visited = 1;
	dijkstra[p-> x_pos - 1][p -> y_pos].hn = heap_insert(&h, &dijkstra[p-> x_pos - 1][p-> y_pos]); 
      }
    }
    
    // Check for valid up neighbor
    if(p-> y_pos + 1 < VERTICAL && p-> y_pos - 1 > 0) {
      if(dijkstra[p->x_pos][p->y_pos + 1].visited != 1  && computeCost.determine_cost_rival(m, p-> x_pos, p -> y_pos + 1) != INT_MAX) {
	dijkstra[p->x_pos][p->y_pos + 1].visited = 1;
	dijkstra[p-> x_pos][p -> y_pos + 1].hn = heap_insert(&h, &dijkstra[p-> x_pos][p-> y_pos + 1]); 
      }
    }

    // Check for valid down neighbor
    if(p-> y_pos - 1 > 0 && p-> y_pos + 1 < VERTICAL) {
      if(dijkstra[p->x_pos][p->y_pos - 1].visited != 1 && computeCost.determine_cost_rival(m, p-> x_pos, p -> y_pos - 1) != INT_MAX) {
	dijkstra[p->x_pos][p->y_pos - 1].visited = 1;
	dijkstra[p-> x_pos][p -> y_pos - 1].hn = heap_insert(&h, &dijkstra[p-> x_pos][p-> y_pos - 1]); 
      }
    }
    
  }

  // We're done with the heap, delete it
  heap_delete(&h);
  
}

// These have to be static, so they have to be in the main game loop file
static void dijkstra_path_hiker(generatedMap *m, movementCosts dijkstra[HORIZONTAL][VERTICAL], int from_x, int from_y)
{

  // We'll use this to store and compute costs
  movementCosts *p, computeCost;
  
  heap_t h;
  uint32_t x, y;
  
  // Initialize all the positions
  for(y = 0; y < VERTICAL; y++) {
    for(x = 0; x < HORIZONTAL; x++) {
      dijkstra[x][y].x_pos = x;
      dijkstra[x][y].y_pos = y;
      dijkstra[x][y].visited = 0;
    }
  }
  
  // Set all costs to be MAX at first
  for (y = 0; y < VERTICAL; y++) {
    for (x = 0; x < HORIZONTAL; x++) {
      dijkstra[x][y].cost = INT_MAX;
    }
  }
  
  dijkstra[from_x][from_y].cost = 0;
  
  heap_init(&h, path_cmp, NULL);
  
  // Insert the first node
  dijkstra[from_x][from_y].hn = heap_insert(&h, &dijkstra[from_x][from_y]);
  
  while ((p = (movementCosts*) heap_remove_min(&h))) {
    p->hn = NULL;
    
    
    if(p-> x_pos - 1 > 0) {
      
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos    ].cost) >
	  (p-> cost + computeCost.determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos    ].cost =
	  ((p->cost + computeCost.determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos)));
	dijkstra[p->x_pos - 1][p->y_pos].next_x = p -> x_pos;
	dijkstra[p->x_pos - 1][p->y_pos].next_y = p -> y_pos;
      }
    }
    
    
    if(p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos    ].cost) >
	  (p->cost + computeCost.determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos    ].cost =
	  ((p->cost + computeCost.determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos)));
	dijkstra[p->x_pos + 1][p->y_pos].next_x = p -> x_pos;
	dijkstra[p->x_pos + 1][p->y_pos].next_y = p -> y_pos;
      }
    }
    
    if(p->y_pos - 1 > 0) {
      if (
	  (dijkstra[p->x_pos][p->y_pos - 1    ].cost) >
	  (p->cost + computeCost.determine_cost_hiker(m, p-> x_pos, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos][p->y_pos - 1    ].cost =
	  ((p->cost + computeCost.determine_cost_hiker(m, p-> x_pos, p -> y_pos - 1)));
	dijkstra[p->x_pos][p->y_pos - 1].next_x = p -> x_pos;
	dijkstra[p->x_pos][p->y_pos - 1].next_y = p -> y_pos;
      }
    }
    
    
    if(p-> y_pos + 1 < VERTICAL) {
      if (
	  (dijkstra[p->x_pos][p->y_pos + 1    ].cost) >
	  (p->cost + computeCost.determine_cost_hiker(m, p-> x_pos, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos][p->y_pos + 1    ].cost =
	  ((p->cost + computeCost.determine_cost_hiker(m, p-> x_pos, p -> y_pos + 1)));
	dijkstra[p->x_pos][p->y_pos + 1].next_x = p -> x_pos;
	dijkstra[p->x_pos][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    if(p-> y_pos + 1 < VERTICAL && p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos + 1    ].cost) >
	  (p->cost + computeCost.determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos + 1    ].cost =
	  ((p->cost + computeCost.determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos + 1)));
	dijkstra[p->x_pos + 1][p->y_pos + 1].next_x = p -> x_pos;
	dijkstra[p->x_pos + 1][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    if(p-> y_pos - 1 < VERTICAL && p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos + 1    ].cost) >
	  (p->cost + computeCost.determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos + 1    ].cost =
	  ((p->cost + computeCost.determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos + 1)));
	dijkstra[p->x_pos - 1][p->y_pos + 1].next_x = p -> x_pos;
	dijkstra[p->x_pos - 1][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    if(p-> y_pos - 1 < VERTICAL && p-> x_pos - 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos - 1    ].cost) >
	  (p->cost + computeCost.determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos - 1    ].cost =
	  ((p->cost + computeCost.determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos - 1)));
	dijkstra[p->x_pos - 1][p->y_pos - 1].next_x = p -> x_pos;
	dijkstra[p->x_pos - 1][p->y_pos - 1].next_y = p -> y_pos;
      }
    }

    if(p-> y_pos + 1 < VERTICAL && p-> x_pos - 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos - 1    ].cost) >
	  (p->cost + computeCost.determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos - 1    ].cost =
	  ((p->cost + computeCost.determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos - 1)));
	dijkstra[p->x_pos + 1][p->y_pos - 1].next_x = p -> x_pos;
	dijkstra[p->x_pos + 1][p->y_pos - 1].next_y = p -> y_pos;
      }
    }
    
    
    if( p-> x_pos + 1 < HORIZONTAL && p -> x_pos - 1 > 0 ) {
      if(dijkstra[p->x_pos + 1][p->y_pos].visited != 1 && computeCost.determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos) != INT_MAX) {
	dijkstra[p->x_pos + 1][p->y_pos].visited = 1;
	dijkstra[p-> x_pos + 1][p -> y_pos].hn = heap_insert(&h, &dijkstra[p-> x_pos + 1][p-> y_pos]);  
      }
    }
    
    if( p-> x_pos - 1 > 0 && p -> x_pos + 1 < HORIZONTAL) {
      if(dijkstra[p->x_pos - 1][p->y_pos].visited != 1  && computeCost.determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos) != INT_MAX) {
	dijkstra[p->x_pos - 1][p->y_pos].visited = 1;
	dijkstra[p-> x_pos - 1][p -> y_pos].hn = heap_insert(&h, &dijkstra[p-> x_pos - 1][p-> y_pos]); 
      }
    }
    
    
    if(p-> y_pos + 1 < VERTICAL && p-> y_pos - 1 > 0) {
      if(dijkstra[p->x_pos][p->y_pos + 1].visited != 1  && computeCost.determine_cost_hiker(m, p-> x_pos, p -> y_pos + 1) != INT_MAX) {
	dijkstra[p->x_pos][p->y_pos + 1].visited = 1;
	dijkstra[p-> x_pos][p -> y_pos + 1].hn = heap_insert(&h, &dijkstra[p-> x_pos][p-> y_pos + 1]); 
      }
    }
    
    if(p-> y_pos - 1 > 0 && p-> y_pos + 1 < VERTICAL) {
      if(dijkstra[p->x_pos][p->y_pos - 1].visited != 1 && computeCost.determine_cost_hiker(m, p-> x_pos, p -> y_pos - 1) != INT_MAX) {
	dijkstra[p->x_pos][p->y_pos - 1].visited = 1;
	dijkstra[p-> x_pos][p -> y_pos - 1].hn = heap_insert(&h, &dijkstra[p-> x_pos][p-> y_pos - 1]); 
      }
    }
    
  }
  
  heap_delete(&h);
}

int main(int argc, char *argv[]) {
  
  // Keep track of place in world
  int x_explore_position;
  int y_explore_position;

  // Used to select some road spot
  int road_spot_x;
  int road_spot_y;
  
  // Choose random road spot for PC
  int PC_road_x;
  int PC_road_y;

  // The scanned input character
  char scanned;

  // Keep track of how many trainers we need
  int numtrainers = 10;

  // Map we are going to traverse
  generatedMap *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH];

  // Distance map for hiker
  movementCosts distance_hiker[HORIZONTAL][VERTICAL];
  
  // Distance map for rival
  movementCosts distance_rival[HORIZONTAL][VERTICAL];

  // Keep track of exits
  int exit_bottom = -1;
  int exit_right = -1;
  int exit_left = -1;
  int exit_top = -1;

  // Track x and y distance away from center
  // I honestly didn't need to include these, but I did just for clarity
  int manhattan_x = 0;
  int manhattan_y = 0;

  // Be sure that the world is initialized to be completely null
  int i;
  int j;

  // Get the user's input
  char user_input;

  // We only want random placement when we enter the first map
  int first_map = -1;

  // We only want to add 1 PC move at a time
  int PC_added_to_heap = 0;

  // Modify and create maps
  mapGen createMap;
  characterLogic processAction;
  movementCosts pathfind;
  generatedMap processMap;
  file_reader FileReader;

  // Data storage objects
  std::vector<experience> exp;
  std::vector<moves> mv;
  std::vector<pokemon> pkmn;
  std::vector<pokemon_moves> pkmn_mv;
  std::vector<pokemon_species> pkmn_spc;
  std::vector<type_names> typ_nm;
  std::vector<pokemon_stats> pkmn_st;

  for(i = 1; i < argc; i++) {
    // Read in specified file
    FileReader.read_in_file(argv[i],
			    exp,
			    mv,
			    pkmn,
			    pkmn_mv,
			    pkmn_spc,
			    typ_nm,
			    pkmn_st);

    // Assign the number of trainers
    if(strcmp(argv[i], "--numtrainers") == 0)
      {
	numtrainers = atoi(argv[i + 1]);
      }
  }

  // curses init
  initscr();
  cbreak();
  halfdelay(1);
  noecho();

  start_color();

  init_pair(GRASS_PAIR, COLOR_GREEN, COLOR_BLACK);
  init_pair(PATH_PAIR, COLOR_YELLOW, COLOR_BLACK);
  init_pair(PLAYER_PAIR, COLOR_CYAN, COLOR_BLACK);
  init_pair(BOULDER_PAIR, COLOR_RED, COLOR_BLACK);
  init_pair(TREE_PAIR, COLOR_BLUE, COLOR_BLACK);

  // Initialize map
  for(i = 0; i < WORLD_Y_LENGTH; i++) {
    for(j = 0; j < WORLD_X_LENGTH; j++) {
      map_exploration[i][j] = NULL;
    }
  }

  // Random seed 
  srand(time(NULL));

  // Start at (199,199)
  x_explore_position = WORLD_X_START;
  y_explore_position = WORLD_Y_START;

  char buffer[50];

  // Make first map. Malloc the first space, init the heap, then generate the map
  map_exploration[y_explore_position][x_explore_position] = (generatedMap*) malloc(sizeof(generatedMap));
  heap_init(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move, move_cost_cmp, NULL);
  createMap.generate_new_map(map_exploration[y_explore_position][x_explore_position],
			     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
			     exit_bottom,
			     exit_right,
			     exit_left,
			     exit_top,
			     manhattan_x,
			     manhattan_y,
			     PC_road_x,
			     PC_road_y,
			     distance_hiker,
			     distance_rival,
			     numtrainers,
			     &first_map);


 
  // Movement is implemented here
  // It's based on peeking the minimum cost character move in the queue
  // If we find one should be removed (equal to game time), do it
  // And replace the same character with an updated cost / new move

  int quit_game = 0;
 
  while(!quit_game) {

    user_input = getch();
   
    characterData *to_move;
    if(map_exploration[y_explore_position][x_explore_position] -> characters_to_move.size != 0) {
      to_move = (characterData*) heap_peek_min(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
      while(to_move -> cost_to_move <= map_exploration[y_explore_position][x_explore_position] -> game_time
	    && (map_exploration[y_explore_position][x_explore_position] -> game_time != 0)) {
       
	to_move = (characterData*) heap_remove_min(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move);

	switch (to_move -> player_type) {
	case random_walker:
	  processAction.move_random_walker(map_exploration[y_explore_position][x_explore_position], (NPC_char*) to_move, &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
	  break;
	 
	case wanderer:
	  processAction.move_wanderer(map_exploration[y_explore_position][x_explore_position], (NPC_char*) to_move, &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
	  break;
	 
	case pacer:
	  processAction.move_pacer(map_exploration[y_explore_position][x_explore_position], (NPC_char*) to_move, &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
	  break;
	 
	case hiker:
	  dijkstra_path_hiker(map_exploration[y_explore_position][x_explore_position], distance_hiker, map_exploration[y_explore_position][x_explore_position] -> PC_position_x , map_exploration[y_explore_position][x_explore_position] -> PC_position_y);
	  processAction.move_via_shortest_path(map_exploration[y_explore_position][x_explore_position], distance_hiker, (NPC_char*) to_move, &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
	  break;
	 
	case rival:
	  dijkstra_path_rival(map_exploration[y_explore_position][x_explore_position], distance_rival, map_exploration[y_explore_position][x_explore_position] -> PC_position_x, map_exploration[y_explore_position][x_explore_position] -> PC_position_y);
	  processAction.move_via_shortest_path(map_exploration[y_explore_position][x_explore_position], distance_rival, (NPC_char*) to_move, &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
	  break;

	case PC:
	  processAction.move_PC((PC_char*) to_move, map_exploration[y_explore_position][x_explore_position], pkmn, pkmn_st, pkmn_mv, mv, manhattan_x, manhattan_y);
	  PC_added_to_heap = 0;
	  break;
	 
	}
       
	to_move = (characterData*) heap_peek_min(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
       
      }
    }

    processMap.print_map(map_exploration[y_explore_position][x_explore_position]);
    map_exploration[y_explore_position][x_explore_position] -> game_time++;

    char buffer[50];
    sprintf(buffer, "PC added to heap: %d", PC_added_to_heap);
    mvaddstr(22, 0, buffer);

    switch(user_input) {

    case '7':
      mvaddstr(22, 30, "got 7");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;

    case 'y':
      mvaddstr(22, 30, "got y");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;
     
    case '8':
      mvaddstr(22, 30, "got 8");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;

    case 'k':
      mvaddstr(22, 30, "got k");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;

    case '9':
      mvaddstr(22, 30, "got 9");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;

    case 'u':
      mvaddstr(22, 30, "got u");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
     
      break;

    case '6':
      mvaddstr(22, 30, "got 6");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;

    case 'l':
      mvaddstr(22, 30, "got l");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;

    case '3':
      mvaddstr(22, 30, "got 3");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;

    case 'n':
      mvaddstr(22, 30, "got n");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;
     
    case '2':
      mvaddstr(22, 30, "got 2");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;

    case 'j':
      mvaddstr(22, 30, "got j");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;
     
    case '1':
      mvaddstr(22, 30, "got 1");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;

    case 'b':
      mvaddstr(22, 30, "got b");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;

    case '4':
      mvaddstr(22, 30, "got 4");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;

    case 'h':
      mvaddstr(22, 30, "got h");

      if(!PC_added_to_heap) {
	processAction.attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
				      map_exploration[y_explore_position][x_explore_position] -> PC_position_y,
				      map_exploration[y_explore_position][x_explore_position],
				      &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				      &PC_added_to_heap);
      }
     
      break;
     
    case '>':

      mvaddstr(22, 30, "got >");

     
      if(map_exploration[y_explore_position][x_explore_position] -> generate_map[map_exploration[y_explore_position][x_explore_position] -> PC_position_x][map_exploration[y_explore_position][x_explore_position] -> PC_position_y] == pokemon_center ||
	 map_exploration[y_explore_position][x_explore_position] -> generate_map[map_exploration[y_explore_position][x_explore_position] -> PC_position_x][map_exploration[y_explore_position][x_explore_position] -> PC_position_y] == pokemon_mart) {

	clear();
	mvaddstr(11, 20, "PLACEHOLDER FOR POKEMON MART / CENTER");
	refresh();
       
	while(getchar() != '<') {
	}  
     
      }
     
       
      break;

    case '5':
      mvaddstr(22, 30, "got 5");
      // Do nothing
      break;

    case ' ':
      mvaddstr(22, 30, "got _");
      // Do nothing
      break;

    case '.':
      mvaddstr(22, 30, "got .");
      // Do nothing
      break;

    case 't':
      {
	mvaddstr(22, 30, "got t");

	heap_t characters_copy;
	int window = 0;
	displayList listUtil;

	int i;
	int j;

	heap_init(&characters_copy, move_cost_cmp, NULL);
     
	for(i = 0; i < VERTICAL; i++) {
	  for(j = 0; j < HORIZONTAL; j++) {
	    if(map_exploration[y_explore_position][x_explore_position] -> character_positions[j][i] != NULL) {
	      if(map_exploration[y_explore_position][x_explore_position] -> character_positions[j][i] -> player_type != PC) {
		heap_insert(&characters_copy, map_exploration[y_explore_position][x_explore_position] -> character_positions[j][i]);
	      }
	    }
	  }
	}

	characterData *characters_list;
	characters_list = (characterData*) malloc(map_exploration[y_explore_position][x_explore_position] -> characters_to_move.size * sizeof(characterData));

	int size = map_exploration[y_explore_position][x_explore_position] -> characters_to_move.size;
	characterData *temp_character;

	for(i = 0; i < size; i++) {
	  temp_character = (characterData*) heap_remove_min(&characters_copy);
	  characters_list[i] = *temp_character;
	}

	clear();

	int return_to_game = 0;
	int pressed_key_1;
	int pressed_key_2;
	int pressed_key_3;

	int extra = 0;
	if(size % 10 != 0) {
	  extra = 1;
	}
     
	while(!return_to_game) {
	  pressed_key_1 = -1;
	  pressed_key_2 = -1;
	  pressed_key_3 = -1;
       
	  listUtil.update_list(map_exploration[y_explore_position][x_explore_position], (NPC_char*) characters_list, window, size);
	  char buffer[50];
	  sprintf(buffer, "window: %d", window);
	  mvaddstr(20, 10, buffer);
	  refresh();
	  pressed_key_1 = getchar();
       
	  if(pressed_key_1 == 0x1B) {
	    pressed_key_2 = getchar();
	 
	    if(pressed_key_2 == 0x5B) {
	   
	      pressed_key_3 = getchar();

	      // Up arrow
	      if(pressed_key_3 == 0x41) {
		if(window > 0) {
		  window--;
		}
	      }

	      // Down arrow
	      if(pressed_key_3 == 0x42) {
		if(window < ((size / 10) + extra - 1)) {
		  window++;
		}
	      }
	   
	    }

	    // Escape
	    if(pressed_key_2 == 0x1B) {
	      return_to_game = 1;
	    }
	  }
       
	}

	characters_list = NULL;
	free(characters_list);
	heap_delete(&characters_copy);
     
	break;
      }

    case 'q':
      {
	quit_game = 1;
	endwin();
	break;
      }
     
    }

    // exit right
    if(map_exploration[y_explore_position][x_explore_position] -> PC_position_x == 79) {
      map_exploration[y_explore_position][x_explore_position]
	-> character_positions[map_exploration[y_explore_position][x_explore_position] -> PC_position_x][map_exploration[y_explore_position][x_explore_position] -> PC_position_y] = NULL;
      free(map_exploration[y_explore_position][x_explore_position]
	   -> character_positions[map_exploration[y_explore_position][x_explore_position] -> PC_position_x][map_exploration[y_explore_position][x_explore_position] -> PC_position_y]);
     
      exit_bottom = -1;
      exit_right = -1;
      exit_top = -1;
      exit_left = -1;
      if (map_exploration[y_explore_position][x_explore_position + 1] == NULL) {
	x_explore_position++;
	manhattan_x++;
	map_exploration[y_explore_position][x_explore_position] = (generatedMap*) malloc(sizeof(generatedMap));
       
	createMap.check_exits(map_exploration,
			      x_explore_position,
			      y_explore_position,
			      &exit_bottom, &exit_right, &exit_left, &exit_top);
	heap_init(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move, move_cost_cmp, NULL);
	PC_road_x = 1;
	PC_road_y = exit_left;
	createMap.generate_new_map(map_exploration[y_explore_position][x_explore_position],
				   &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				   exit_bottom,
				   exit_right,
				   exit_left,
				   exit_top,
				   manhattan_x,
				   manhattan_y,
				   PC_road_x,
				   PC_road_y,
				   distance_hiker,
				   distance_rival,
				   numtrainers,
				   &first_map);
      }
      else {
	x_explore_position++;
	manhattan_x++;
	createMap.check_exits(map_exploration,
			      x_explore_position,
			      y_explore_position,
			      &exit_bottom, &exit_right, &exit_left, &exit_top);
	PC_road_x = 1;
	PC_road_y = exit_left;

	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] = (characterData*) malloc(sizeof(characterData));
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> player_type = PC;
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> x_pos = PC_road_x;
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> y_pos = PC_road_y;
       
	map_exploration[y_explore_position][x_explore_position] -> PC_position_x = PC_road_x;
	map_exploration[y_explore_position][x_explore_position] -> PC_position_y = PC_road_y;
       
	processMap.print_map(map_exploration[y_explore_position][x_explore_position]);
      }
     
    }

    // exit left
    if(map_exploration[y_explore_position][x_explore_position] -> PC_position_x == 0) {
      map_exploration[y_explore_position][x_explore_position]
	-> character_positions[map_exploration[y_explore_position][x_explore_position] -> PC_position_x][map_exploration[y_explore_position][x_explore_position] -> PC_position_y] = NULL;
      free(map_exploration[y_explore_position][x_explore_position]
	   -> character_positions[map_exploration[y_explore_position][x_explore_position] -> PC_position_x][map_exploration[y_explore_position][x_explore_position] -> PC_position_y]);
     
      exit_bottom = -1;
      exit_right = -1;
      exit_top = -1;
      exit_left = -1;
      if (map_exploration[y_explore_position][x_explore_position - 1] == NULL) {
	x_explore_position--;
	manhattan_x--;
	map_exploration[y_explore_position][x_explore_position] = (generatedMap*) malloc(sizeof(generatedMap));
       
	createMap.check_exits(map_exploration,
			      x_explore_position,
			      y_explore_position,
			      &exit_bottom, &exit_right, &exit_left, &exit_top);
	heap_init(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move, move_cost_cmp, NULL);
	PC_road_x = 78;
	PC_road_y = exit_right;
	createMap.generate_new_map(map_exploration[y_explore_position][x_explore_position],
				   &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				   exit_bottom,
				   exit_right,
				   exit_left,
				   exit_top,
				   manhattan_x,
				   manhattan_y,
				   PC_road_x,
				   PC_road_y,
				   distance_hiker,
				   distance_rival,
				   numtrainers,
				   &first_map);
      }
      else {
	x_explore_position--;
	manhattan_x--;
	createMap.check_exits(map_exploration,
			      x_explore_position,
			      y_explore_position,
			      &exit_bottom, &exit_right, &exit_left, &exit_top);
	PC_road_x = 78;
	PC_road_y = exit_right;

	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] = (characterData*) malloc(sizeof(characterData));
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> player_type = PC;
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> x_pos = PC_road_x;
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> y_pos = PC_road_y;
       
	map_exploration[y_explore_position][x_explore_position] -> PC_position_x = PC_road_x;
	map_exploration[y_explore_position][x_explore_position] -> PC_position_y = PC_road_y;
       
	processMap.print_map(map_exploration[y_explore_position][x_explore_position]);
      }
    }

    // exit up
    if(map_exploration[y_explore_position][x_explore_position] -> PC_position_y == 0) {
      map_exploration[y_explore_position][x_explore_position]
	-> character_positions[map_exploration[y_explore_position][x_explore_position] -> PC_position_x][map_exploration[y_explore_position][x_explore_position] -> PC_position_y] = NULL;
      free(map_exploration[y_explore_position][x_explore_position]
	   -> character_positions[map_exploration[y_explore_position][x_explore_position] -> PC_position_x][map_exploration[y_explore_position][x_explore_position] -> PC_position_y]);
     
      exit_bottom = -1;
      exit_right = -1;
      exit_top = -1;
      exit_left = -1;
      if (map_exploration[y_explore_position - 1][x_explore_position] == NULL) {
	y_explore_position--;
	manhattan_y--;
	map_exploration[y_explore_position][x_explore_position] = (generatedMap*) malloc(sizeof(generatedMap));
       
	createMap.check_exits(map_exploration,
			      x_explore_position,
			      y_explore_position,
			      &exit_bottom, &exit_right, &exit_left, &exit_top);
	heap_init(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move, move_cost_cmp, NULL);
	PC_road_x = exit_bottom;
	PC_road_y = 19;
	createMap.generate_new_map(map_exploration[y_explore_position][x_explore_position],
				   &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				   exit_bottom,
				   exit_right,
				   exit_left,
				   exit_top,
				   manhattan_x,
				   manhattan_y,
				   PC_road_x,
				   PC_road_y,
				   distance_hiker,
				   distance_rival,
				   numtrainers,
				   &first_map);
      }
      else {
	y_explore_position--;
	manhattan_y--;
	createMap.check_exits(map_exploration,
			      x_explore_position,
			      y_explore_position,
			      &exit_bottom, &exit_right, &exit_left, &exit_top);
	PC_road_x = exit_bottom;
	PC_road_y = 19;

	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] = (characterData*) malloc(sizeof(characterData));
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> player_type = PC;
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> x_pos = PC_road_x;
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> y_pos = PC_road_y;
       
	map_exploration[y_explore_position][x_explore_position] -> PC_position_x = PC_road_x;
	map_exploration[y_explore_position][x_explore_position] -> PC_position_y = PC_road_y;
       
	processMap.print_map(map_exploration[y_explore_position][x_explore_position]);
      }
    }

    // exit down
    if(map_exploration[y_explore_position][x_explore_position] -> PC_position_y == 20) {
      map_exploration[y_explore_position][x_explore_position]
	-> character_positions[map_exploration[y_explore_position][x_explore_position] -> PC_position_x][map_exploration[y_explore_position][x_explore_position] -> PC_position_y] = NULL;
      free(map_exploration[y_explore_position][x_explore_position]
	   -> character_positions[map_exploration[y_explore_position][x_explore_position] -> PC_position_x][map_exploration[y_explore_position][x_explore_position] -> PC_position_y]);
     
      exit_bottom = -1;
      exit_right = -1;
      exit_top = -1;
      exit_left = -1;
      if (map_exploration[y_explore_position + 1][x_explore_position] == NULL) {
	y_explore_position++;
	manhattan_y++;
	map_exploration[y_explore_position][x_explore_position] = (generatedMap*) malloc(sizeof(generatedMap));
       
	createMap.check_exits(map_exploration,
			      x_explore_position,
			      y_explore_position,
			      &exit_bottom, &exit_right, &exit_left, &exit_top);
	heap_init(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move, move_cost_cmp, NULL);
	PC_road_x = exit_top;
	PC_road_y = 1;
	createMap.generate_new_map(map_exploration[y_explore_position][x_explore_position],
				   &map_exploration[y_explore_position][x_explore_position] -> characters_to_move,
				   exit_bottom,
				   exit_right,
				   exit_left,
				   exit_top,
				   manhattan_x,
				   manhattan_y,
				   PC_road_x,
				   PC_road_y,
				   distance_hiker,
				   distance_rival,
				   numtrainers,
				   &first_map);
      }
      else {
	y_explore_position++;
	manhattan_y++;
	createMap.check_exits(map_exploration,
			      x_explore_position,
			      y_explore_position,
			      &exit_bottom, &exit_right, &exit_left, &exit_top);
	PC_road_x = exit_top;
	PC_road_y = 1;

	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] = (characterData*) malloc(sizeof(characterData));
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> player_type = PC;
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> x_pos = PC_road_x;
	map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> y_pos = PC_road_y;
       
	map_exploration[y_explore_position][x_explore_position] -> PC_position_x = PC_road_x;
	map_exploration[y_explore_position][x_explore_position] -> PC_position_y = PC_road_y;
       
	processMap.print_map(map_exploration[y_explore_position][x_explore_position]);
      }
    }
   
  }

 
  return 0;
}


void mapGen::check_edge_cases(generatedMap *map_data, int y_explore_position, int x_explore_position) {

  int i;

  if(y_explore_position == 0) {
    for(i = 0; i < HORIZONTAL; i++) {
      map_data -> generate_map[i][VERTICAL - 1] = boulder;
    }
  }

  if(y_explore_position == 399) {
    for(i = 0; i < HORIZONTAL; i++) {
      map_data -> generate_map[i][0] = boulder;
    }
  }

  if(x_explore_position == 0) {
    for(i = 0; i < VERTICAL; i++) {
      map_data -> generate_map[0][i] = boulder;
    }
  }

  if(x_explore_position == 399) {
    for(i = 0; i < VERTICAL; i++) {
      map_data -> generate_map[HORIZONTAL - 1][i] = boulder;
    }
  }
  
}

void mapGen::generate_new_map(generatedMap *map_data,
			      heap_t *h,
			      int exit_bottom,
			      int exit_right,
			      int exit_left,
			      int exit_top,
			      int manhattan_x,
			      int manhattan_y,
			      int PC_path_x,
			      int PC_path_y,
			      movementCosts distance_hiker[HORIZONTAL][VERTICAL],
			      movementCosts distance_rival[HORIZONTAL][VERTICAL],
			      int numtrainers,
			      int *first_map) {

  // Used to create the map / find shortest paths
  generatedMap processMap;
  mapGen createMap;
  movementCosts pathfind;
  characterLogic characterLogic;
  
  // I was getting some weird memory problems without initializing
  // everything to nothing
  int i;
  int j;
  
  for(i = 0; i < VERTICAL; i++) {
    for(j = 0; j < HORIZONTAL; j++) {
      map_data -> generate_map[j][i] = nothing;
    }
  }
  
  for(i = 0; i < VERTICAL; i++) {
    for(j = 0; j < HORIZONTAL; j++) {
      map_data -> character_positions[j][i] = NULL;
    }
  }

  // Calculate manhattan distance
  int manhattan_distance = abs(manhattan_x) + abs(manhattan_y);

  // Do the actual spawn rate conversion (I don't divide by 100)
  int building_spawn_rate = (((-45 * manhattan_distance) / 200) + 50);

  if (building_spawn_rate < 5) {
    building_spawn_rate = 5;
  }
  
  // Function calls need to happen in this order
  processMap.place_clearings(map_data);
  processMap.place_tall_grass(map_data);
  processMap.place_border_boulders(map_data);
  
  processMap.place_exits(map_data, exit_bottom, exit_right, exit_left, exit_top);
  processMap.place_paths(map_data);
  processMap.place_buildings(map_data, building_spawn_rate);
  processMap.fill_blank_space(map_data);

  // To avoid passing in y and x explore, I just add 199 to the manhattan counters
  createMap.check_edge_cases(map_data, manhattan_y + 199, manhattan_x + 199);

  // Insert the PC
  // Only randomize position for the first map
  if(*first_map == -1) {
    createMap.choose_random_road_spot(map_data, &PC_path_x, &PC_path_y);
    *first_map = 1;
  }
 
  map_data -> character_positions[PC_path_x][PC_path_y] = (PC_char*) malloc(sizeof(PC_char));
  map_data -> character_positions[PC_path_x][PC_path_y] -> player_type = PC;
  map_data -> character_positions[PC_path_x][PC_path_y] -> x_pos = PC_path_x;
  map_data -> character_positions[PC_path_x][PC_path_y] -> y_pos = PC_path_y;

  map_data -> PC_position_x = PC_path_x;
  map_data -> PC_position_y = PC_path_y;

  dijkstra_path_rival(map_data, distance_rival, PC_path_x, PC_path_y);
  dijkstra_path_hiker(map_data, distance_hiker, PC_path_x, PC_path_y);
  
  characterLogic.place_characters(map_data, h, distance_hiker, distance_rival, numtrainers);
  
  processMap.print_map(map_data);
}

void mapGen::check_exits(generatedMap *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH],
			 int x_explore,
			 int y_explore,
			 int *exit_bottom,
			 int *exit_right,
			 int *exit_left,
			 int *exit_top) {

  if(y_explore + 1 < 400) {
    if(map_exploration[y_explore + 1][x_explore] != NULL) {
      *exit_bottom = map_exploration[y_explore + 1][x_explore] -> exit_top;
    }
  }

  if(x_explore + 1 < 400) {
    if(map_exploration[y_explore][x_explore + 1] != NULL) {
      *exit_right = map_exploration[y_explore][x_explore + 1] -> exit_left;
    }
  }

  if(y_explore - 1 > -1) {
    if(map_exploration[y_explore - 1][x_explore] != NULL) {
      *exit_top = map_exploration[y_explore - 1][x_explore] -> exit_bottom;
    }
  }

  if(x_explore - 1 > -1) {
    if(map_exploration[y_explore][x_explore - 1] != NULL) {
      *exit_left = map_exploration[y_explore][x_explore - 1] -> exit_right;
    }
  }
  
}

void mapGen::choose_random_road_spot(generatedMap *map_data, int *chosen_spot_x, int *chosen_spot_y) {

  int i;
  int j;

  while(*chosen_spot_x == 0){
    for (i = (rand() % (19 - 2 + 1)) + 2; i < VERTICAL; i++) {
      for(j = (rand() % (78 - 2 + 1)) + 2; j < HORIZONTAL; j++) {
	if(map_data -> generate_map[j][i] == path) {
	  *chosen_spot_x = j;
	  *chosen_spot_y = i;
	  return;
	}
      }
    }
  }
}

void displayList::update_list(generatedMap *m, NPC_char *list_copy, int window, int size) {

  int to_print_final;
  int pages;
  int total;
  
  to_print_final = size % 10;
  pages = size / 10;
  total = pages * 10 + to_print_final;
  
  int position;
  int i;
  
  char buffer[100];
  char player_type[15];
  int iterate = 3;

  clear();
  
  if(window == pages) {
    position = 10 * window;
    
    for(i = position; i < to_print_final + pages * 10; i++) {
      
      switch(list_copy[i].player_type) {
      case rival:
	strcpy(player_type, "RIVAL");
	break;
      case hiker:
	strcpy(player_type, "HIKER");
	break;
      case pacer:
	strcpy(player_type, "PACER");
	break;
      case wanderer:
	strcpy(player_type, "WANDERER");
	break;
      case random_walker:
	strcpy(player_type, "RANDOM WALKER");
	break;
      case stationary:
	strcpy(player_type, "STATIONARY");
	break;
      }
      
      sprintf(buffer, "CHARACTER: %s DISTANCE FROM PC X: %d Y: %d", player_type, list_copy[i].x_pos - m -> PC_position_x, list_copy[i].y_pos - m -> PC_position_y);
      mvaddstr(iterate, 10, buffer);
      
      iterate += 1;
      
    }
  }
  
  else {
    
    position = window * 10;
    
    for(i = position; i < window * 10 + 10; i++) {
      
      switch(list_copy[i].player_type) {
      case rival:
	strcpy(player_type, "RIVAL");
	break;
      case hiker:
	strcpy(player_type, "HIKER");
	break;
      case pacer:
	strcpy(player_type, "PACER");
	break;
      case wanderer:
	strcpy(player_type, "WANDERER");
	break;
      case random_walker:
	strcpy(player_type, "RANDOM WALKER");
	break;
      case stationary:
	strcpy(player_type, "STATIONARY");
	break;
      }
      
      sprintf(buffer, "CHARACTER: %s [] DISTANCE FROM PC X: %d Y: %d", player_type, list_copy[i].x_pos - m -> PC_position_x, list_copy[i].y_pos - m -> PC_position_y);
      mvaddstr(iterate, 10, buffer);
      
      iterate += 1;
    }

    refresh();
    
  }
}

void characterLogic::engage_battle() {

  clear();
  mvaddstr(11, 20, "PLACEHOLDER FOR POKEMON BATTLE");
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


