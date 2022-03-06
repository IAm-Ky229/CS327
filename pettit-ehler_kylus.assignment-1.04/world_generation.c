#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>

#include "terrain_generation.h"
#include "world_generation.h"


static int32_t path_cmp(const void *key, const void *with) {
  return ((cost_t *) key)->cost - ((cost_t *) with)->cost;
}

static int32_t move_cost_cmp(const void *key, const void *with) {
  return ((character_t *) key)-> cost_to_move - ((character_t *) with)-> cost_to_move;
}

int main(int argc, char * argv[]) {
  
  // Keep track of place in world
  int x_explore_position;
  int y_explore_position;

  // Used to select some road spot
  int road_spot_x;
  int road_spot_y;
  
  // Choose random road spot for PC
  int random_road_x;
  int random_road_y;

  // The scanned input character
  char scanned;

  // Map we are going to traverse
  generated_map_t *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH];

  // Distance map for hiker
  cost_t distance_hiker[HORIZONTAL][VERTICAL];
  
  // Distance map for rival
  cost_t distance_rival[HORIZONTAL][VERTICAL];

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

  heap_t characters_to_move;

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

 heap_init(&characters_to_move, move_cost_cmp, NULL);

 // Make first map
 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
 generate_new_map(map_exploration[y_explore_position][x_explore_position],
		  &characters_to_move,
		  exit_bottom,
		  exit_right,
		  exit_left,
		  exit_top,
		  manhattan_x,
		  manhattan_y,
		  &random_road_x,
		  &random_road_y,
		  distance_hiker,
		  distance_rival);

 int game_time = 0;

 // Movement is implemented here
 while(1) {
   
   character_t *to_move;

   to_move = heap_peek_min(&characters_to_move);
   printf("peeked %d \n", to_move -> cost_to_move);
   while(to_move -> cost_to_move <= game_time && (game_time != 0)) {
     to_move = heap_remove_min(&characters_to_move);
     switch (to_move -> player_type) {
       case random_walker:
	 move_random_walker(map_exploration[y_explore_position][x_explore_position], to_move, &characters_to_move); 
       break;
     case wanderer:
       move_wanderer(map_exploration[y_explore_position][x_explore_position], to_move, &characters_to_move);
       break;
     case pacer:
       //printf("before updating: %d\n", to_move -> cost_to_move);
       //printf("size: %d\n", characters_to_move.size);
       move_pacer(map_exploration[y_explore_position][x_explore_position], to_move, &characters_to_move);
       break;

     case hiker:
       dijkstra_path_hiker(map_exploration[y_explore_position][x_explore_position], distance_hiker, random_road_x, random_road_y);
       move_via_shortest_path(map_exploration[y_explore_position][x_explore_position], distance_hiker, to_move, &characters_to_move);
       break;

     case rival:
       dijkstra_path_hiker(map_exploration[y_explore_position][x_explore_position], distance_rival, random_road_x, random_road_y);
       move_via_shortest_path(map_exploration[y_explore_position][x_explore_position], distance_rival, to_move, &characters_to_move);
       break;

     }
     
     to_move = heap_peek_min(&characters_to_move);
     
   }

   print_map(map_exploration[y_explore_position][x_explore_position]);
   usleep(250000);
   game_time++;
 }

 
 return 0;
}


void check_edge_cases(generated_map_t *map_data, int y_explore_position, int x_explore_position) {

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

void generate_new_map(generated_map_t *map_data,
		      heap_t *h,
		      int exit_bottom,
		      int exit_right,
		      int exit_left,
		      int exit_top,
		      int manhattan_x,
		      int manhattan_y,
		      int *random_path_x,
		      int *random_path_y,
		      cost_t distance_hiker[HORIZONTAL][VERTICAL],
		      cost_t distance_rival[HORIZONTAL][VERTICAL]) {
  
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
  place_clearings(map_data);
  place_tall_grass(map_data);
  place_border_boulders(map_data);
  place_exits(map_data, exit_bottom, exit_right, exit_left, exit_top);
  place_paths(map_data);
  place_buildings(map_data, building_spawn_rate);
  fill_blank_space(map_data);

  // To avoid passing in y and x explore, I just add 199 to the manhattan counters
  check_edge_cases(map_data, manhattan_y + 199, manhattan_x + 199);

  // Insert the PC
  choose_random_road_spot(map_data, random_path_x, random_path_y);
  map_data -> character_positions[*random_path_x][*random_path_y] = malloc(sizeof(character_t));
  map_data -> character_positions[*random_path_x][*random_path_y] -> player_type = PC;
  heap_insert(h, map_data -> character_positions[*random_path_x][*random_path_y]);

  dijkstra_path_rival(map_data, distance_rival, *random_path_x, *random_path_y);
  dijkstra_path_hiker(map_data, distance_hiker, *random_path_x, *random_path_y);
  
  place_characters(map_data, h, distance_hiker, distance_rival);
  
  print_map(map_data);
}

void check_exits(generated_map_t *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH],
		 int x_explore,
		 int y_explore,
		 int *exit_bottom,
		 int *exit_right,
		 int *exit_left,
		 int *exit_top) {

  if(y_explore + 1 < 400) {
    if(map_exploration[y_explore + 1][x_explore] != NULL) {
      *exit_top = map_exploration[y_explore + 1][x_explore] -> exit_bottom;
    }
  }

  if(x_explore + 1 < 400) {
    if(map_exploration[y_explore][x_explore + 1] != NULL) {
      *exit_right = map_exploration[y_explore][x_explore + 1] -> exit_left;
    }
  }

  if(y_explore - 1 > -1) {
    if(map_exploration[y_explore - 1][x_explore] != NULL) {
      *exit_bottom = map_exploration[y_explore - 1][x_explore] -> exit_top;
    }
  }

  if(x_explore - 1 > -1) {
    if(map_exploration[y_explore][x_explore - 1] != NULL) {
      *exit_left = map_exploration[y_explore][x_explore - 1] -> exit_right;
    }
  }
  
}

void choose_random_road_spot(generated_map_t *map_data, int *chosen_spot_x, int *chosen_spot_y) {

  int i;
  int j;

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

int determine_cost_rival(generated_map_t *map_data, int x_dim, int y_dim) {

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
  default:
    return 0;
  }
}

int determine_cost_hiker(generated_map_t *map_data, int x_dim, int y_dim) {

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
  default:
    return 0;
  }
}

static void dijkstra_path_rival(generated_map_t *m, cost_t dijkstra[HORIZONTAL][VERTICAL], int from_x, int from_y)
{

  // We'll use this to store and compute costs
  cost_t  *p;

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
  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    // In general: check bounds
    // check if the cost to go a certain direction is less than what is already there
    // If it is less, update the cost
    // Else do nothing
    
    // Try to go left
    if(p-> x_pos - 1 > 0) {
      
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos    ].cost) >
	  (p-> cost + determine_cost_rival(m, p-> x_pos - 1, p -> y_pos)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos    ].cost =
	  ((p->cost + determine_cost_rival(m, p-> x_pos - 1, p -> y_pos)));
	  dijkstra[p->x_pos - 1][p->y_pos    ].next_x = p -> x_pos;
	  dijkstra[p->x_pos - 1][p->y_pos    ].next_y = p -> y_pos;
      }
    }
    
    // Try to go right
    if(p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos    ].cost) >
	  (p->cost + determine_cost_rival(m, p-> x_pos + 1, p -> y_pos)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos    ].cost =
	  ((p->cost + determine_cost_rival(m, p-> x_pos + 1, p -> y_pos)));
	  dijkstra[p->x_pos + 1][p->y_pos    ].next_x = p -> x_pos;
	  dijkstra[p->x_pos + 1][p->y_pos    ].next_y = p -> y_pos;
      }
    }

    // Try to go down
    if(p->y_pos - 1 > 0) {
      if (
	  (dijkstra[p->x_pos][p->y_pos - 1    ].cost) >
	  (p->cost + determine_cost_rival(m, p-> x_pos, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos][p->y_pos - 1    ].cost =
	  ((p->cost + determine_cost_rival(m, p-> x_pos, p -> y_pos - 1)));
	  dijkstra[p->x_pos][p->y_pos - 1].next_x = p -> x_pos;
	  dijkstra[p->x_pos][p->y_pos - 1].next_y = p -> y_pos;
      }
    }
    
    // Try to go up
    if(p-> y_pos + 1 < VERTICAL) {
      if (
	  (dijkstra[p->x_pos][p->y_pos + 1    ].cost) >
	  (p->cost + determine_cost_rival(m, p-> x_pos, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos][p->y_pos + 1    ].cost =
	  ((p->cost + determine_cost_rival(m, p-> x_pos, p -> y_pos + 1)));
	  dijkstra[p->x_pos][p->y_pos + 1].next_x = p -> x_pos;
	  dijkstra[p->x_pos][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    // Try to go up right
    if(p-> y_pos + 1 < VERTICAL && p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos + 1    ].cost) >
	  (p->cost + determine_cost_rival(m, p-> x_pos + 1, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos + 1    ].cost =
	  ((p->cost + determine_cost_rival(m, p-> x_pos + 1, p -> y_pos + 1)));
	  dijkstra[p->x_pos + 1][p->y_pos + 1].next_x = p -> x_pos;
	  dijkstra[p->x_pos + 1][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    // Try to go down right
    if(p-> y_pos - 1 < VERTICAL && p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos + 1    ].cost) >
	  (p->cost + determine_cost_rival(m, p-> x_pos - 1, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos + 1    ].cost =
	  ((p->cost + determine_cost_rival(m, p-> x_pos - 1, p -> y_pos + 1)));
	  dijkstra[p->x_pos - 1][p->y_pos + 1].next_x = p -> x_pos;
	  dijkstra[p->x_pos - 1][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    // Try to go down left
    if(p-> y_pos - 1 < VERTICAL && p-> x_pos - 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos - 1    ].cost) >
	  (p->cost + determine_cost_rival(m, p-> x_pos - 1, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos - 1    ].cost =
	  ((p->cost + determine_cost_rival(m, p-> x_pos - 1, p -> y_pos - 1)));
	  dijkstra[p->x_pos - 1][p->y_pos - 1].next_x = p -> x_pos;
	  dijkstra[p->x_pos - 1][p->y_pos - 1].next_y = p -> y_pos;
      }
    }

    // Try to go up left
    if(p-> y_pos + 1 < VERTICAL && p-> x_pos - 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos - 1    ].cost) >
	  (p->cost + determine_cost_rival(m, p-> x_pos + 1, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos - 1    ].cost =
	  ((p->cost + determine_cost_rival(m, p-> x_pos + 1, p -> y_pos - 1)));
	  dijkstra[p->x_pos + 1][p->y_pos - 1].next_x = p -> x_pos;
	  dijkstra[p->x_pos + 1][p->y_pos - 1].next_y = p -> y_pos;
      }
    }
    
    // Check for valid right neighbor
    if( p-> x_pos + 1 < HORIZONTAL && p -> x_pos - 1 > 0 ) {
      if(dijkstra[p->x_pos + 1][p->y_pos].visited != 1 && determine_cost_rival(m, p-> x_pos + 1, p -> y_pos) != INT_MAX) {
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
      if(dijkstra[p->x_pos - 1][p->y_pos].visited != 1  && determine_cost_rival(m, p-> x_pos - 1, p -> y_pos) != INT_MAX) {
	dijkstra[p->x_pos - 1][p->y_pos].visited = 1;
	dijkstra[p-> x_pos - 1][p -> y_pos].hn = heap_insert(&h, &dijkstra[p-> x_pos - 1][p-> y_pos]); 
      }
    }
    
    // Check for valid up neighbor
    if(p-> y_pos + 1 < VERTICAL && p-> y_pos - 1 > 0) {
      if(dijkstra[p->x_pos][p->y_pos + 1].visited != 1  && determine_cost_rival(m, p-> x_pos, p -> y_pos + 1) != INT_MAX) {
	dijkstra[p->x_pos][p->y_pos + 1].visited = 1;
	dijkstra[p-> x_pos][p -> y_pos + 1].hn = heap_insert(&h, &dijkstra[p-> x_pos][p-> y_pos + 1]); 
      }
    }

    // Check for valid down neighbor
    if(p-> y_pos - 1 > 0 && p-> y_pos + 1 < VERTICAL) {
      if(dijkstra[p->x_pos][p->y_pos - 1].visited != 1 && determine_cost_rival(m, p-> x_pos, p -> y_pos - 1) != INT_MAX) {
	dijkstra[p->x_pos][p->y_pos - 1].visited = 1;
	dijkstra[p-> x_pos][p -> y_pos - 1].hn = heap_insert(&h, &dijkstra[p-> x_pos][p-> y_pos - 1]); 
      }
    }
    
  }

  // We're done with the heap, delete it
  heap_delete(&h);
  

  // Print out the dijkstra cost computations
  /*
  printf("\n");
  printf("RIVAL GRAPH: \n");
  printf("\n");
  for (y = 1; y < VERTICAL - 1; y++) {
    for (x = 1; x < HORIZONTAL - 1; x++) {
      if(dijkstra[x][y].cost < -1) {
	printf("   ");
      }
      else {
	printf("%02d ", dijkstra[x][y].cost % 100);
      }
    }
    printf("\n");
  }
  printf("\n");
  */
}

// This is the same as dijkstra_path_rival, we're just calling a different cost computation function
static void dijkstra_path_hiker(generated_map_t *m, cost_t dijkstra[HORIZONTAL][VERTICAL], int from_x, int from_y)
{

  // We'll use this to store and compute costs
  cost_t *p;
  
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
  
  
  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;
    
    
    if(p-> x_pos - 1 > 0) {
      
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos    ].cost) >
	  (p-> cost + determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos)));
	dijkstra[p->x_pos - 1][p->y_pos].next_x = p -> x_pos;
	dijkstra[p->x_pos - 1][p->y_pos].next_y = p -> y_pos;
      }
    }
    
    
    if(p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos)));
	dijkstra[p->x_pos + 1][p->y_pos].next_x = p -> x_pos;
	dijkstra[p->x_pos + 1][p->y_pos].next_y = p -> y_pos;
      }
    }
    
    if(p->y_pos - 1 > 0) {
      if (
	  (dijkstra[p->x_pos][p->y_pos - 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos][p->y_pos - 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos, p -> y_pos - 1)));
	dijkstra[p->x_pos][p->y_pos - 1].next_x = p -> x_pos;
	dijkstra[p->x_pos][p->y_pos - 1].next_y = p -> y_pos;
      }
    }
    
    
    if(p-> y_pos + 1 < VERTICAL) {
      if (
	  (dijkstra[p->x_pos][p->y_pos + 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos][p->y_pos + 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos, p -> y_pos + 1)));
	dijkstra[p->x_pos][p->y_pos + 1].next_x = p -> x_pos;
	dijkstra[p->x_pos][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    if(p-> y_pos + 1 < VERTICAL && p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos + 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos + 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos + 1)));
	dijkstra[p->x_pos + 1][p->y_pos + 1].next_x = p -> x_pos;
	dijkstra[p->x_pos + 1][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    if(p-> y_pos - 1 < VERTICAL && p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos + 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos + 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos + 1)));
	dijkstra[p->x_pos - 1][p->y_pos + 1].next_x = p -> x_pos;
	dijkstra[p->x_pos - 1][p->y_pos + 1].next_y = p -> y_pos;
      }
    }

    if(p-> y_pos - 1 < VERTICAL && p-> x_pos - 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos - 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos - 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos - 1)));
	dijkstra[p->x_pos - 1][p->y_pos - 1].next_x = p -> x_pos;
	dijkstra[p->x_pos - 1][p->y_pos - 1].next_y = p -> y_pos;
      }
    }

    if(p-> y_pos + 1 < VERTICAL && p-> x_pos - 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos - 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos - 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos - 1)));
	dijkstra[p->x_pos + 1][p->y_pos - 1].next_x = p -> x_pos;
	dijkstra[p->x_pos + 1][p->y_pos - 1].next_y = p -> y_pos;
      }
    }
    
    
    if( p-> x_pos + 1 < HORIZONTAL && p -> x_pos - 1 > 0 ) {
      if(dijkstra[p->x_pos + 1][p->y_pos].visited != 1 && determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos) != INT_MAX) {
	dijkstra[p->x_pos + 1][p->y_pos].visited = 1;
	dijkstra[p-> x_pos + 1][p -> y_pos].hn = heap_insert(&h, &dijkstra[p-> x_pos + 1][p-> y_pos]);  
      }
    }
    
    if( p-> x_pos - 1 > 0 && p -> x_pos + 1 < HORIZONTAL) {
      if(dijkstra[p->x_pos - 1][p->y_pos].visited != 1  && determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos) != INT_MAX) {
	dijkstra[p->x_pos - 1][p->y_pos].visited = 1;
	dijkstra[p-> x_pos - 1][p -> y_pos].hn = heap_insert(&h, &dijkstra[p-> x_pos - 1][p-> y_pos]); 
      }
    }
    
    
    if(p-> y_pos + 1 < VERTICAL && p-> y_pos - 1 > 0) {
      if(dijkstra[p->x_pos][p->y_pos + 1].visited != 1  && determine_cost_hiker(m, p-> x_pos, p -> y_pos + 1) != INT_MAX) {
	dijkstra[p->x_pos][p->y_pos + 1].visited = 1;
	dijkstra[p-> x_pos][p -> y_pos + 1].hn = heap_insert(&h, &dijkstra[p-> x_pos][p-> y_pos + 1]); 
      }
    }
    
    if(p-> y_pos - 1 > 0 && p-> y_pos + 1 < VERTICAL) {
      if(dijkstra[p->x_pos][p->y_pos - 1].visited != 1 && determine_cost_hiker(m, p-> x_pos, p -> y_pos - 1) != INT_MAX) {
	dijkstra[p->x_pos][p->y_pos - 1].visited = 1;
	dijkstra[p-> x_pos][p -> y_pos - 1].hn = heap_insert(&h, &dijkstra[p-> x_pos][p-> y_pos - 1]); 
      }
    }
    
  }
  
  heap_delete(&h);
  
  /*
  printf("\n");
  printf("HIKER GRAPH: \n");
  printf("\n");
  for (y = 1; y < VERTICAL - 1; y++) {
    for (x = 1; x < HORIZONTAL - 1; x++) {
      if(dijkstra[x][y].cost < -1) {
	printf("   ");
      }
      else {
	printf("%02d ", dijkstra[x][y].cost % 100);
      }
    }
    printf("\n");
  }
  printf("\n");
  */
}

void place_characters(generated_map_t *m, heap_t *h, cost_t distance_hiker[HORIZONTAL][VERTICAL], cost_t distance_rival[HORIZONTAL][VERTICAL]) {

  int placed_characters = 0;
  int rand_x;
  int rand_y;

  int min_x_next;
  int min_y_next;
  int cost_to_move;

  int i;
  enum char_type characters_to_place[10];
  
  int choose_character;

  for (i = 0; i < 8; i++) {

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

  i = 0;
  while (placed_characters < 8) {
    rand_x = (rand() % (78 - 2 + 1)) + 2;
    rand_y = (rand() % (19 - 2 + 1)) + 2;

    if(m -> generate_map[rand_x][rand_y] != boulder &&
	m -> generate_map[rand_x][rand_y] != tree &&
	m -> generate_map[rand_x][rand_y] != pokemon_mart &&
	m -> generate_map[rand_x][rand_y] != pokemon_center &&
        m -> generate_map[rand_x][rand_y] != path &&
	m -> character_positions[rand_x][rand_y] == NULL) {
      
      
      printf("assigning character: x %d and y %d\n", rand_x, rand_y);
      m -> character_positions[rand_x][rand_y] = malloc(sizeof(character_t));
      m -> character_positions[rand_x][rand_y] -> player_type = characters_to_place[placed_characters];
      m -> character_positions[rand_x][rand_y] -> cost_to_move = 0;
      m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
      m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;

      printf("player_type: %d\n", m -> character_positions[rand_x][rand_y] -> player_type);
      
      switch(m -> character_positions[rand_x][rand_y] -> player_type) {
      case wanderer:
	m -> character_positions[rand_x][rand_y] -> terrain_type = m -> generate_map[rand_x][rand_y];
	
	if(m -> generate_map[rand_x][rand_y + 1] != tree &&
	   m -> generate_map[rand_x][rand_y + 1] != boulder &&
	   m -> character_positions[rand_x][rand_y] -> terrain_type == m -> generate_map[rand_x][rand_y + 1]) {
	  m -> character_positions[rand_x][rand_y] -> direction = down;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x, rand_y + 1);
	    m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	    m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	    m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	    m -> character_positions[rand_x][rand_y] -> next_y = rand_y + 1;
	}
	else if(m -> generate_map[rand_x][rand_y - 1] != tree &&
	   m -> generate_map[rand_x][rand_y - 1] != boulder &&
		m -> character_positions[rand_x][rand_y] -> terrain_type == m -> generate_map[rand_x][rand_y - 1]) {
	  	  m -> character_positions[rand_x][rand_y] -> direction = up;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x, rand_y - 1);
	    m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	    m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	    m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	    m -> character_positions[rand_x][rand_y] -> next_y = rand_y - 1;
	}
	else if(m -> generate_map[rand_x + 1][rand_y] != tree &&
	   m -> generate_map[rand_x + 1][rand_y] != boulder &&
		m -> character_positions[rand_x][rand_y] -> terrain_type == m -> generate_map[rand_x + 1][rand_y]) {
	  	  m -> character_positions[rand_x][rand_y] -> direction = right;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x + 1, rand_y);
	    m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	    m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	    m -> character_positions[rand_x][rand_y] -> next_x = rand_x + 1;
	    m -> character_positions[rand_x][rand_y] -> next_y = rand_y;
	}
	else if(m -> generate_map[rand_x - 1][rand_y] != tree &&
	   m -> generate_map[rand_x - 1][rand_y] != boulder &&
		m -> character_positions[rand_x][rand_y] -> terrain_type == m -> generate_map[rand_x - 1][rand_y]) {
	  	  m -> character_positions[rand_x][rand_y] -> direction = left;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x - 1, rand_y);
	    m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	    m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	    m -> character_positions[rand_x][rand_y] -> next_x = rand_x - 1;
	    m -> character_positions[rand_x][rand_y] -> next_y = rand_y;
	}
	break;
      case random_walker:
	if(m -> generate_map[rand_x][rand_y + 1] != tree &&
	   m -> generate_map[rand_x][rand_y + 1] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> direction = down;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x, rand_y + 1);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y + 1;
	}
	else if(m -> generate_map[rand_x][rand_y - 1] != tree &&
		m -> generate_map[rand_x][rand_y - 1] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> direction = up;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x, rand_y - 1);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y - 1;
	}
	else if(m -> generate_map[rand_x + 1][rand_y] != tree &&
		m -> generate_map[rand_x + 1][rand_y] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> direction = right;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x + 1, rand_y);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x + 1;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y;
	}
	else if(m -> generate_map[rand_x - 1][rand_y] != tree &&
		m -> generate_map[rand_x - 1][rand_y] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> direction = left;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x - 1, rand_y);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x - 1;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y;
	}
	break;
      case stationary:
	break;
      case pacer:
	if(m -> generate_map[rand_x][rand_y + 1] != tree &&
	   m -> generate_map[rand_x][rand_y + 1] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> direction = down;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x, rand_y + 1);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y + 1;
	}
	else if(m -> generate_map[rand_x][rand_y - 1] != tree &&
		m -> generate_map[rand_x][rand_y - 1] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> direction = up;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x, rand_y - 1);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y - 1;
	}
	else if(m -> generate_map[rand_x + 1][rand_y] != tree &&
		m -> generate_map[rand_x + 1][rand_y] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> direction = right;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x + 1, rand_y);
	  m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	  m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	  m -> character_positions[rand_x][rand_y] -> next_x = rand_x + 1;
	  m -> character_positions[rand_x][rand_y] -> next_y = rand_y;
	}
	else if(m -> generate_map[rand_x - 1][rand_y] != tree &&
		m -> generate_map[rand_x - 1][rand_y] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> direction = left;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x - 1, rand_y);
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
	
	if(abs(distance_rival[rand_x + 1][rand_y].cost) < cost_to_move) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y;
	  
	  cost_to_move = distance_rival[rand_x + 1][rand_y].cost;
	}
	
	if( abs(distance_rival[rand_x][rand_y + 1].cost) < cost_to_move) {
	  min_x_next = rand_x;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_rival[rand_x][rand_y + 1].cost;
	}
	
	if( abs(distance_rival[rand_x - 1][rand_y].cost) < cost_to_move) {
	  min_x_next = rand_x - 1;
	  min_y_next = rand_y;
	  
	  cost_to_move = distance_rival[rand_x - 1][rand_y].cost;
	}
	
	if( abs(distance_rival[rand_x][rand_y - 1].cost) < cost_to_move) {
	  min_x_next = rand_x;
	  min_y_next = rand_y - 1;
	  
	  cost_to_move = distance_rival[rand_x][rand_y - 1].cost;
	}
	
	if( abs(distance_rival[rand_x + 1][rand_y + 1].cost) < cost_to_move) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_rival[rand_x + 1][rand_y + 1].cost;
	}
	
	if( abs(distance_rival[rand_x + 1][rand_y - 1].cost) < cost_to_move) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y - 1;
	  
	  cost_to_move = distance_rival[rand_x + 1][rand_y - 1].cost;
	}
	
	if(abs(distance_rival[rand_x - 1][rand_y + 1].cost) < cost_to_move) {
	  min_x_next = rand_x - 1;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_rival[rand_x - 1][rand_y + 1].cost;
	}
	
	if( abs(distance_rival[rand_x - 1][rand_y - 1].cost) < cost_to_move) {
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
	
	if(abs(distance_hiker[rand_x + 1][rand_y].cost) < cost_to_move) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y;
	  
	  cost_to_move = distance_hiker[rand_x + 1][rand_y].cost;
	}
	
	if( abs(distance_hiker[rand_x][rand_y + 1].cost) < cost_to_move) {
	  min_x_next = rand_x;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_hiker[rand_x][rand_y + 1].cost;
	}
	
	if( abs(distance_hiker[rand_x - 1][rand_y].cost) < cost_to_move) {
	  min_x_next = rand_x - 1;
	  min_y_next = rand_y;
	  
	  cost_to_move = distance_hiker[rand_x - 1][rand_y].cost;
	}
	
	if( abs(distance_hiker[rand_x][rand_y - 1].cost) < cost_to_move) {
	  min_x_next = rand_x;
	  min_y_next = rand_y - 1;
	  
	  cost_to_move = distance_hiker[rand_x][rand_y - 1].cost;
	}
	
	if( abs(distance_hiker[rand_x + 1][rand_y + 1].cost) < cost_to_move) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_hiker[rand_x + 1][rand_y + 1].cost;
	}
	
	if( abs(distance_hiker[rand_x + 1][rand_y - 1].cost) < cost_to_move) {
	  min_x_next = rand_x + 1;
	  min_y_next = rand_y - 1;
	  
	  cost_to_move = distance_hiker[rand_x + 1][rand_y - 1].cost;
	}
	
	if(abs(distance_hiker[rand_x - 1][rand_y + 1].cost) < cost_to_move) {
	  min_x_next = rand_x - 1;
	  min_y_next = rand_y + 1;
	  
	  cost_to_move = distance_hiker[rand_x - 1][rand_y + 1].cost;
	}
	
	if( abs(distance_hiker[rand_x - 1][rand_y - 1].cost) < cost_to_move) {
	  min_x_next = rand_x - 1;
	  min_y_next = rand_y - 1;
	  
	  cost_to_move = distance_hiker[rand_x - 1][rand_y - 1].cost;
	}
	
	m -> character_positions[rand_x][rand_y] -> cost_to_move = distance_rival[rand_x][rand_y].cost - cost_to_move;
	m -> character_positions[rand_x][rand_y] -> next_x = min_x_next;
	m -> character_positions[rand_x][rand_y] -> next_y = min_y_next;
	
	break; 
      }
      
      printf("inserting cost: %d\n", m -> character_positions[rand_x][rand_y] -> cost_to_move);
      heap_insert(h, m -> character_positions[rand_x][rand_y]);
      placed_characters++;
    }
  }

}

void move_pacer(generated_map_t *m, character_t *pacer_to_move, heap_t *h) {
  
  int current_x;
  int current_y;

  int last_x;
  int last_y;
  
  if((m -> generate_map[pacer_to_move -> next_x][pacer_to_move -> next_y] != tree) &&
     (m -> generate_map[pacer_to_move -> next_x][pacer_to_move -> next_y] != boulder) &&
     (m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] == NULL)) {

    current_x = pacer_to_move -> next_x;
    current_y = pacer_to_move -> next_y;

    last_x = pacer_to_move -> x_pos;
    last_y = pacer_to_move -> y_pos;
    
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] = malloc(sizeof(character_t));
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> player_type = pacer;
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> cost_to_move = pacer_to_move -> cost_to_move;
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> direction = pacer_to_move -> direction;
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> x_pos = current_x;
    m -> character_positions[pacer_to_move -> next_x][pacer_to_move -> next_y] -> y_pos = current_y;

    switch(pacer_to_move -> direction) {
      
    case down:
      move_down(m, h, pacer_to_move, current_x, current_y);
      break;
    case up:
      move_up(m, h, pacer_to_move, current_x, current_y);
      break;
      
    case right:
      move_right(m, h, pacer_to_move, current_x, current_y);
      break;
      
    case left:
      move_left(m, h, pacer_to_move, current_x, current_y);     
      break;
    }

    m -> character_positions[last_x][last_y] = NULL;
    free(m -> character_positions[last_x][last_y]);
    
  }
  
  
}

void move_up(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> y_pos - 1 > 0 ) {
	if(
	   (m -> generate_map[current_x][current_y - 1] != tree) &&
	   (m -> generate_map[current_x][current_y - 1] != boulder) &&
	   (m -> character_positions[current_x][current_y - 1] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y - 1);
	  m -> character_positions[current_x][current_y] -> next_x = current_x;
	  m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y + 1);
	  m -> character_positions[current_x][current_y] -> next_x = current_x;
	  m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	  m -> character_positions[current_x][current_y] -> direction = down;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
  
}

void move_down(generated_map_t *m, heap_t *h, character_t *character_to_move,  int current_x, int current_y) {
if( character_to_move -> y_pos + 1 < 21 ) {
	if(
	   (m -> generate_map[current_x][current_y + 1] != tree) &&
	   (m -> generate_map[current_x][current_y + 1] != boulder) &&
	   (m -> character_positions[current_x][current_y + 1] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y + 1);
	  m -> character_positions[current_x][current_y] -> next_x = current_x;
	  m -> character_positions[current_x][current_y] -> next_y  = current_y + 1;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  printf("going up now\n");
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y - 1);
	  m -> character_positions[current_x][current_y] -> next_x = current_x;
	  m -> character_positions[current_x][current_y] -> next_y  = current_y - 1;
	  m -> character_positions[current_x][current_y] -> direction = up;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
}

void move_left(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> x_pos - 1 > 0 ) {
	if(
	   (m -> generate_map[current_x - 1][current_y] != tree) &&
	   (m -> generate_map[current_x - 1][current_y] != boulder) &&
	   (m -> character_positions[current_x - 1][current_y] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x - 1, current_y);
	  m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	  m -> character_positions[current_x][current_y] -> next_y = current_y;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x + 1, current_y);
	  m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	  m -> character_positions[current_x][current_y] -> next_y = current_y;
	  m -> character_positions[current_x][current_y] -> direction = right;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
  
}

void move_right(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> x_pos + 1 < 80 ) {
	if(
	   (m -> generate_map[current_x + 1][current_y] != tree) &&
	   (m -> generate_map[current_x + 1][current_y] != boulder) &&
	   (m -> character_positions[current_x + 1][current_y] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x + 1, current_y);
	  m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	  m -> character_positions[current_x][current_y] -> next_y = current_y;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x - 1, current_y);
	  m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	  m -> character_positions[current_x][current_y] -> next_y = current_y;
	  m -> character_positions[current_x][current_y] -> direction = left;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
  
}


void move_via_shortest_path(generated_map_t *m, cost_t dijkstra[HORIZONTAL][VERTICAL], character_t *character_to_move, heap_t *h) {

  int min_x_next;
  int min_y_next;
  int cost_to_move = INT_MAX;
  
  if(m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] == NULL) {
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] = malloc(sizeof(character_t));
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> player_type = character_to_move -> player_type;
    
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> cost_to_move = m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> cost_to_move;
    
    if( abs(dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y].cost) < cost_to_move) {
      min_x_next = character_to_move -> next_x + 1;
      min_y_next = character_to_move -> next_y;
      
      cost_to_move = dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y].cost;
    }
    
    if( abs(dijkstra[character_to_move -> next_x][character_to_move -> next_y + 1].cost) < cost_to_move) {
      min_x_next = character_to_move -> next_x;
      min_y_next = character_to_move -> next_y + 1;
      
      cost_to_move = dijkstra[character_to_move -> next_x][character_to_move -> next_y + 1].cost;
    }
    
    if( abs(dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y].cost) < cost_to_move) {
      min_x_next = character_to_move -> next_x - 1;
      min_y_next = character_to_move -> next_y;
      
      cost_to_move = dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y].cost;
    }
    
    if( abs(dijkstra[character_to_move -> next_x][character_to_move -> y_pos - 1].cost) < cost_to_move) {
      min_x_next = character_to_move -> next_x;
      min_y_next = character_to_move -> next_y - 1;
      
      cost_to_move = dijkstra[character_to_move -> next_x][character_to_move -> next_y - 1].cost;
    }
    
    if( abs(dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y + 1].cost) < cost_to_move) {
      min_x_next = character_to_move -> next_x + 1;
      min_y_next = character_to_move -> next_y + 1;
      
      cost_to_move = dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y + 1].cost;
    }
    
    
    if( abs(dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y - 1].cost) < cost_to_move) {
      min_x_next = character_to_move -> next_x + 1;
      min_y_next = character_to_move -> next_y - 1;
      
      cost_to_move = dijkstra[character_to_move -> next_x + 1][character_to_move -> next_y - 1].cost;
    }
    
    
    if( abs(dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y + 1].cost) < cost_to_move) {
      min_x_next = character_to_move -> next_x - 1;
      min_y_next = character_to_move -> next_y + 1;
      
      cost_to_move = dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y + 1].cost;
    }
    
    
    if( abs(dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y - 1].cost) < cost_to_move) {
      min_x_next = character_to_move -> next_x - 1;
      min_y_next = character_to_move -> next_y - 1;
      
      cost_to_move = dijkstra[character_to_move -> next_x - 1][character_to_move -> next_y - 1].cost;
    }
    
    // I have no idea why this is adding 10 :(
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> cost_to_move += dijkstra[character_to_move -> x_pos][character_to_move -> y_pos].cost - cost_to_move - 10;
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> next_x = min_x_next;
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> next_y = min_y_next;
    
    int x_position = m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> next_x;
    int y_position = m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> next_y;
    
    int prev_x = m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> x_pos;
    int prev_y = m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> y_pos;

    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> x_pos = x_position;
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> y_pos = y_position;

    m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] = NULL;
    free(m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos]);
    heap_insert(h, m -> character_positions[x_position][y_position]);
   }
  else{
    m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> cost_to_move +=
      determine_cost_rival(m, character_to_move -> x_pos, character_to_move -> y_pos);
    heap_insert(h, m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos]);
  }
}



void move_wanderer(generated_map_t *m, character_t *wanderer_to_move, heap_t *h) {

  int current_x;
  int current_y;

  int last_x;
  int last_y;
  
  if((m -> generate_map[wanderer_to_move -> next_x][wanderer_to_move -> next_y] != tree) &&
     (m -> generate_map[wanderer_to_move -> next_x][wanderer_to_move -> next_y] != boulder) &&
     (m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] == NULL)) {

    current_x = wanderer_to_move -> next_x;
    current_y = wanderer_to_move -> next_y;

    last_x = wanderer_to_move -> x_pos;
    last_y = wanderer_to_move -> y_pos;
    
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] = malloc(sizeof(character_t));
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> player_type = wanderer;
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> cost_to_move = wanderer_to_move -> cost_to_move;
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> direction = wanderer_to_move -> direction;
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> terrain_type = wanderer_to_move -> terrain_type;
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> x_pos = current_x;
    m -> character_positions[wanderer_to_move -> next_x][wanderer_to_move -> next_y] -> y_pos = current_y;
    printf("got past assignments\n");
    
    printf("moving: %d\n", wanderer_to_move -> direction);
    printf("stay on type %d\n", wanderer_to_move -> terrain_type);
    printf("moving x: %d y: %d\n", last_x, last_y);
    
    switch(wanderer_to_move -> direction) {
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
    
    m -> character_positions[last_x][last_y] = NULL;
    free(m -> character_positions[last_x][last_y]);
    
  }
}

void move_up_random(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> y_pos - 1 > 0 ) {
	if(
	   (m -> generate_map[current_x][current_y - 1] != tree) &&
	   (m -> generate_map[current_x][current_y - 1] != boulder) &&
	   (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1]) &&
	   (m -> character_positions[current_x][current_y - 1] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y - 1);
	  m -> character_positions[current_x][current_y] -> next_x = current_x;
	  m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  
	  direction_t direction_to_move;
	  int valid = 0;

	  while(!valid) {
	    printf("valid up: %d\n", valid);

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y + 1]) &&
		 (m -> character_positions[current_x][current_y + 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y + 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
		m -> character_positions[current_x][current_y] -> direction = down;
		valid = 1;
	      }
	      break;
	      
	    case 1:
	      if(( m -> generate_map[current_x + 1][current_y] != tree) &&
		 (m -> generate_map[current_x + 1][current_y] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x + 1][current_y]) &&
		 (m -> character_positions[current_x + 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x + 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = right;
		valid = 1;
	      }
	      break;
	      
	    case 2:
	      if(( m -> generate_map[current_x - 1][current_y] != tree) &&
		 (m -> generate_map[current_x - 1][current_y] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x - 1][current_y]) &&
		 (m -> character_positions[current_x - 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x - 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = left;
		valid = 1;
	      }
	      break;
	    }
	  }
	  
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
  
}

void move_down_random(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> y_pos - 1 > 0 ) {
	if(
	   (m -> generate_map[current_x][current_y + 1] != tree) &&
	   (m -> generate_map[current_x][current_y + 1] != boulder) &&
	   (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y + 1]) &&
	   (m -> character_positions[current_x][current_y + 1] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y + 1);
	  m -> character_positions[current_x][current_y] -> next_x = current_x;
	  m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  
	  direction_t direction_to_move;
	  int valid = 0;

	  while(!valid) {

	    printf("valid down: %d\n", valid);

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y - 1] != tree) &&
		 (m -> generate_map[current_x][current_y - 1] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1]) &&
		 (m -> character_positions[current_x][current_y - 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y - 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
		m -> character_positions[current_x][current_y] -> direction = up;
		valid = 1;
	      }
	      break;
	      
	    case 1:
	      if(( m -> generate_map[current_x + 1][current_y] != tree) &&
		 (m -> generate_map[current_x + 1][current_y] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x + 1][current_y]) &&
		 (m -> character_positions[current_x + 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x + 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = right;
		valid = 1;
	      }
	      break;
	      
	    case 2:
	      if(( m -> generate_map[current_x - 1][current_y] != tree) &&
		 (m -> generate_map[current_x - 1][current_y] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x - 1][current_y]) &&
		 (m -> character_positions[current_x - 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x - 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = left;
		valid = 1;
	      }
	      break;
	    }
	  }
	  
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
  
}

void move_right_random(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> x_pos + 1 < 80 ) {
	if(
	   (m -> generate_map[current_x + 1][current_y] != tree) &&
	   (m -> generate_map[current_x + 1][current_y] != boulder) &&
	   (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x + 1][current_y]) &&
	   (m -> character_positions[current_x + 1][current_y] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x + 1, current_y);
	  m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	  m -> character_positions[current_x][current_y] -> next_y = current_y;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  
	  direction_t direction_to_move;
	  int valid = 0;

	  while(!valid) {
	    printf("valid right: %d\n", valid);

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y + 1]) &&
		 (m -> character_positions[current_x][current_y + 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y + 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
		m -> character_positions[current_x][current_y] -> direction = down;
		valid = 1;
	      }
	      break;
	      
	    case 1:
	      if(( m -> generate_map[current_x][current_y - 1] != tree) &&
		 (m -> generate_map[current_x][current_y - 1] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1]) &&
		 (m -> character_positions[current_x][current_y - 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y - 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
		m -> character_positions[current_x][current_y] -> direction = up;
		valid = 1;
	      }
	      break;
	      
	    case 2:
	      if(( m -> generate_map[current_x - 1][current_y] != tree) &&
		 (m -> generate_map[current_x - 1][current_y] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x - 1][current_y]) &&
		 (m -> character_positions[current_x - 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x - 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = left;
		valid = 1;
	      }
	      break;
	    }
	  }
	  
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
  
}

void move_left_random(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> x_pos - 1 > 0 ) {
	if(
	   (m -> generate_map[current_x - 1][current_y] != tree) &&
	   (m -> generate_map[current_x - 1][current_y] != boulder) &&
	   (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x - 1][current_y]) &&
	   (m -> character_positions[current_x - 1][current_y] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x - 1, current_y);
	  m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	  m -> character_positions[current_x][current_y] -> next_y = current_y;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  
	  direction_t direction_to_move;
	  int valid = 0;

	  while(!valid) {
	    printf("valid left: %d\n", valid);

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y + 1]) &&
		 (m -> character_positions[current_x][current_y + 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y + 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
		m -> character_positions[current_x][current_y] -> direction = down;
		valid = 1;
	      }
	      break;
	      
	    case 1:
	      if(( m -> generate_map[current_x + 1][current_y] != tree) &&
		 (m -> generate_map[current_x + 1][current_y] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x + 1][current_y]) &&
		 (m -> character_positions[current_x + 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x + 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = right;
		valid = 1;
	      }
	      break;
	      
	    case 2:
	      if(( m -> generate_map[current_x][current_y - 1] != tree) &&
		 (m -> generate_map[current_x][current_y - 1] != boulder) &&
		 (m -> character_positions[current_x][current_y] -> terrain_type == m -> generate_map[current_x][current_y - 1]) &&
		 (m -> character_positions[current_x][current_y - 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y - 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
		m -> character_positions[current_x][current_y] -> direction = up;
		valid = 1;
	      }
	      break;
	    }
	  }
	  
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
  
}

void move_random_walker(generated_map_t *m, character_t *walker_to_move, heap_t *h) {

  int current_x;
  int current_y;

  int last_x;
  int last_y;
  
  if((m -> generate_map[walker_to_move -> next_x][walker_to_move -> next_y] != tree) &&
     (m -> generate_map[walker_to_move -> next_x][walker_to_move -> next_y] != boulder) &&
     (m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] == NULL)) {

    current_x = walker_to_move -> next_x;
    current_y = walker_to_move -> next_y;

    last_x = walker_to_move -> x_pos;
    last_y = walker_to_move -> y_pos;
    
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] = malloc(sizeof(character_t));
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> player_type = random_walker;
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> cost_to_move = walker_to_move -> cost_to_move;
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> direction = walker_to_move -> direction;
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> x_pos = current_x;
    m -> character_positions[walker_to_move -> next_x][walker_to_move -> next_y] -> y_pos = current_y;
    printf("got past assignments\n");
    
    printf("moving: %d\n", walker_to_move -> direction);
    printf("moving x: %d y: %d\n", last_x, last_y);
    
    switch(walker_to_move -> direction) {
    case down:
      move_down_walker(m, h, walker_to_move, current_x, current_y);
      break;
    case up:
      move_up_walker(m, h, walker_to_move, current_x, current_y);
      break;
    case right:
      move_right_walker(m, h, walker_to_move, current_x, current_y);
      break;
    case left:
      move_left_walker(m, h, walker_to_move, current_x, current_y);     
      break;
    }
    
    m -> character_positions[last_x][last_y] = NULL;
    free(m -> character_positions[last_x][last_y]);
    
  }
}

void move_left_walker(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> x_pos - 1 > 0 ) {
	if(
	   (m -> generate_map[current_x - 1][current_y] != tree) &&
	   (m -> generate_map[current_x - 1][current_y] != boulder) &&
	   (m -> character_positions[current_x - 1][current_y] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x - 1, current_y);
	  m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
	  m -> character_positions[current_x][current_y] -> next_y = current_y;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  
	  direction_t direction_to_move;
	  int valid = 0;

	  while(!valid) {
	    printf("valid left: %d\n", valid);

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> character_positions[current_x][current_y + 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y + 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
		m -> character_positions[current_x][current_y] -> direction = down;
		valid = 1;
	      }
	      break;
	      
	    case 1:
	      if(( m -> generate_map[current_x + 1][current_y] != tree) &&
		 (m -> generate_map[current_x + 1][current_y] != boulder) &&
		 (m -> character_positions[current_x + 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x + 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = right;
		valid = 1;
	      }
	      break;
	      
	    case 2:
	      if(( m -> generate_map[current_x][current_y - 1] != tree) &&
		 (m -> generate_map[current_x][current_y - 1] != boulder) &&
		 (m -> character_positions[current_x][current_y - 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y - 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
		m -> character_positions[current_x][current_y] -> direction = up;
		valid = 1;
	      }
	      break;
	    }
	  }
	  
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
  
}

void move_right_walker(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> x_pos + 1 < 80 ) {
	if(
	   (m -> generate_map[current_x + 1][current_y] != tree) &&
	   (m -> generate_map[current_x + 1][current_y] != boulder) &&
	   (m -> character_positions[current_x + 1][current_y] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x + 1, current_y);
	  m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
	  m -> character_positions[current_x][current_y] -> next_y = current_y;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  
	  direction_t direction_to_move;
	  int valid = 0;

	  while(!valid) {
	    printf("valid left: %d\n", valid);

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> character_positions[current_x][current_y + 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y + 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
		m -> character_positions[current_x][current_y] -> direction = down;
		valid = 1;
	      }
	      break;
	      
	    case 1:
	      if(( m -> generate_map[current_x - 1][current_y] != tree) &&
		 (m -> generate_map[current_x - 1][current_y] != boulder) &&
		 (m -> character_positions[current_x - 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x - 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = left;
		valid = 1;
	      }
	      break;
	      
	    case 2:
	      if(( m -> generate_map[current_x][current_y - 1] != tree) &&
		 (m -> generate_map[current_x][current_y - 1] != boulder) &&
		 (m -> character_positions[current_x][current_y - 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y - 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
		m -> character_positions[current_x][current_y] -> direction = up;
		valid = 1;
	      }
	      break;
	    }
	  }
	  
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
  
}

void move_up_walker(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> y_pos - 1 > 0 ) {
	if(
	   (m -> generate_map[current_x][current_y - 1] != tree) &&
	   (m -> generate_map[current_x][current_y - 1] != boulder) &&
	   (m -> character_positions[current_x][current_y - 1] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y - 1);
	  m -> character_positions[current_x][current_y] -> next_x = current_x;
	  m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  
	  direction_t direction_to_move;
	  int valid = 0;

	  while(!valid) {
	    printf("valid left: %d\n", valid);

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> character_positions[current_x][current_y + 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y + 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
		m -> character_positions[current_x][current_y] -> direction = down;
		valid = 1;
	      }
	      break;
	      
	    case 1:
	      if(( m -> generate_map[current_x + 1][current_y] != tree) &&
		 (m -> generate_map[current_x + 1][current_y] != boulder) &&
		 (m -> character_positions[current_x + 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x + 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = right;
		valid = 1;
	      }
	      break;
	      
	    case 2:
	      if(( m -> generate_map[current_x - 1][current_y] != tree) &&
		 (m -> generate_map[current_x - 1][current_y] != boulder) &&
		 (m -> character_positions[current_x - 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x - 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = left;
		valid = 1;
	      }
	      break;
	    }
	  }
	  
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
  
}

void move_down_walker(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> y_pos + 1 < 21 ) {
	if(
	   (m -> generate_map[current_x][current_y + 1] != tree) &&
	   (m -> generate_map[current_x][current_y + 1] != boulder) &&
	   (m -> character_positions[current_x][current_y + 1] == NULL)) {
	  
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y + 1);
	  m -> character_positions[current_x][current_y] -> next_x = current_x;
	  m -> character_positions[current_x][current_y] -> next_y = current_y + 1;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
	  
	  direction_t direction_to_move;
	  int valid = 0;

	  while(!valid) {
	    printf("valid left: %d\n", valid);

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y - 1] != tree) &&
		 (m -> generate_map[current_x][current_y - 1] != boulder) &&
		 (m -> character_positions[current_x][current_y - 1] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y - 1);
		m -> character_positions[current_x][current_y] -> next_x = current_x;
		m -> character_positions[current_x][current_y] -> next_y = current_y - 1;
		m -> character_positions[current_x][current_y] -> direction = up;
		valid = 1;
	      }
	      break;
	      
	    case 1:
	      if(( m -> generate_map[current_x + 1][current_y] != tree) &&
		 (m -> generate_map[current_x + 1][current_y] != boulder) &&
		 (m -> character_positions[current_x + 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x + 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x + 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = right;
		valid = 1;
	      }
	      break;
	      
	    case 2:
	      if(( m -> generate_map[current_x - 1][current_y] != tree) &&
		 (m -> generate_map[current_x - 1][current_y] != boulder) &&
		 (m -> character_positions[current_x - 1][current_y] == NULL)) {
		m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x - 1, current_y);
		m -> character_positions[current_x][current_y] -> next_x = current_x - 1;
		m -> character_positions[current_x][current_y] -> next_y = current_y;
		m -> character_positions[current_x][current_y] -> direction = left;
		valid = 1;
	      }
	      break;
	    }
	  }
	  
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
      }
  
}

