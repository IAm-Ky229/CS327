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
     //printf("removed\n");
     switch (to_move -> player_type) {
       
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
   usleep(100000);
   game_time++;
 }

 //choose_random_road_spot(map_exploration[y_explore_position][x_explore_position], &road_spot_x, &road_spot_y);

 //printf("Random road spot X: %d\n", road_spot_x);
 //printf("Random road spot Y: %d\n", road_spot_y);
 //dijkstra_path_rival(map_exploration[y_explore_position][x_explore_position], road_spot_x, road_spot_y);
 //dijkstra_path_hiker(map_exploration[y_explore_position][x_explore_position], road_spot_x, road_spot_y);

 /*
 // Continually get buffered input till we quit
 while(1) {
   // Assume we don't have any neighbors
   exit_bottom = -1;
   exit_right = -1;
   exit_top = -1;
   exit_left = -1;
   
   scanf(" %c", &scanned);

   // In general:
   // Check for an existing map. If no map, generate one
   // Update movement
   // Allocate space for map
   // Check for exits that need to connect to the map we will make
   // Generate the new map using the found exits and building spawn rate

   switch(scanned) {
     
   case 'n':
     printf("going north\n");
     if ((y_explore_position + 1) < WORLD_Y_LENGTH) {
       if (map_exploration[y_explore_position + 1][x_explore_position] == NULL) {
	 y_explore_position++;
	 manhattan_y++;
	 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
	 
	 check_exits(map_exploration,
		     x_explore_position,
		     y_explore_position,
		     &exit_bottom, &exit_right, &exit_left, &exit_top);
	 generate_new_map(map_exploration[y_explore_position][x_explore_position],
			  exit_bottom,
			  exit_right,
			  exit_left,
			  exit_top,
			  manhattan_x,
			  manhattan_y);
       }
       else {
	 y_explore_position++;
	 manhattan_y++;
	 print_map(map_exploration[y_explore_position][x_explore_position]);
       }
     }
     else {
       printf("that is not a valid move\n");
     }
     break;
     
   case 's':
     printf("going south\n");
     if ((y_explore_position - 1) > -1) {
       if (map_exploration[y_explore_position - 1][x_explore_position] == NULL) {
	 y_explore_position--;
	 manhattan_y--;
	 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
	 
	 check_exits(map_exploration,
		     x_explore_position,
		     y_explore_position,
		     &exit_bottom, &exit_right, &exit_left, &exit_top);
	 generate_new_map(map_exploration[y_explore_position][x_explore_position],
			  exit_bottom,
			  exit_right,
			  exit_left,
			  exit_top,
			  manhattan_x,
			  manhattan_y);
       }
       else {
	 y_explore_position--;
	 manhattan_y--;
	 print_map(map_exploration[y_explore_position][x_explore_position]);
       }
     }
     else {
       printf("that is not a valid move\n");
     }     
     break;
     
   case 'e':
     printf("going east\n");
     if ((x_explore_position + 1) < WORLD_X_LENGTH) {
       if (map_exploration[y_explore_position][x_explore_position + 1] == NULL) {
	 x_explore_position++;
	 manhattan_x++;
	 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
	 
	 check_exits(map_exploration,
		     x_explore_position,
		     y_explore_position,
		     &exit_bottom, &exit_right, &exit_left, &exit_top);
	 generate_new_map(map_exploration[y_explore_position][x_explore_position],
			  exit_bottom,
			  exit_right,
			  exit_left,
			  exit_top,
			  manhattan_x,
			  manhattan_y);
       }
       else {
	 x_explore_position++;
	 manhattan_x++;
	 print_map(map_exploration[y_explore_position][x_explore_position]);
       }
     }
     else {
       printf("that is not a valid move\n");
     }     
     break;
     
   case 'w':
     printf("going west\n");
     if ((x_explore_position - 1) > -1) {
       if (map_exploration[y_explore_position][x_explore_position - 1] == NULL) {
	 x_explore_position--;
	 manhattan_x--;
	 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
	 
	 check_exits(map_exploration,
		     x_explore_position,
		     y_explore_position,
		     &exit_bottom, &exit_right, &exit_left, &exit_top);
	 generate_new_map(map_exploration[y_explore_position][x_explore_position],
			  exit_bottom,
			  exit_right,
			  exit_left,
			  exit_top,
			  manhattan_x,
			  manhattan_y);
       }
       else {
	 x_explore_position--;
	 manhattan_x--;
	 print_map(map_exploration[y_explore_position][x_explore_position]);
       }
     }
     else {
       printf("that is not a valid move\n");
     }     
     break;
     
   case 'f':
     printf("enter an X and a Y to fly to:\n");
     int x_explore_new;
     int y_explore_new;
     
     scanf("%d", &x_explore_new);
     scanf("%d", &y_explore_new);
     printf("flying to (%d, %d)\n", x_explore_new, y_explore_new);
     
     // Check for in bounds fly
     if(x_explore_new < WORLD_X_LENGTH && x_explore_new > -1
	&& y_explore_new < WORLD_Y_LENGTH && y_explore_new > -1) {
       
       x_explore_position = x_explore_new;
       y_explore_position = y_explore_new;
       
       // Set correct manhattan position
       manhattan_x = x_explore_position - 199;
       manhattan_y = y_explore_position - 199;
       
       if (map_exploration[y_explore_position][x_explore_position] == NULL) {
	 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
	 check_exits(map_exploration,
		     x_explore_position,
		     y_explore_position,
		     &exit_bottom, &exit_right, &exit_left, &exit_top);
	 
	 generate_new_map(map_exploration[y_explore_position][x_explore_position],
			  exit_bottom,
			  exit_right,
			  exit_left,
			  exit_top,
			  manhattan_x,
			  manhattan_y);
       }
       else {
	 print_map(map_exploration[y_explore_position][x_explore_position]);
       }
     }
     else {
       printf("that is not a valid move\n");
     }     
     break;
     
   case 'q':
     printf("quitting game\n");
     return 0;
     
   default:
     printf("that is not a valid command\n");
     break;
   }

   // Do dijkstra's on the map we generated / found in memory
   //choose_random_road_spot(map_exploration[y_explore_position][x_explore_position], &road_spot_x, &road_spot_y);
   
   //printf("Random road spot X: %d\n", road_spot_x);
   //printf("Random road spot Y: %d\n", road_spot_y);
   //dijkstra_path_rival(map_exploration[y_explore_position][x_explore_position], road_spot_x, road_spot_y);
   //dijkstra_path_hiker(map_exploration[y_explore_position][x_explore_position], road_spot_x, road_spot_y);
   

 }
 */
 
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
	m -> character_positions[rand_x][rand_y] == NULL) {
      
      
      printf("assigning character: x %d and y %d\n", rand_x, rand_y);
      m -> character_positions[rand_x][rand_y] = malloc(sizeof(character_t));
      m -> character_positions[rand_x][rand_y] -> player_type = characters_to_place[placed_characters];
      m -> character_positions[rand_x][rand_y] -> cost_to_move = 0;

      switch(m -> character_positions[rand_x][rand_y] -> player_type) {
      case stationary:
	// do nothing
	break;
      case pacer:
	if(m -> generate_map[rand_x][rand_y + 1] != tree ||
	   m -> generate_map[rand_x][rand_y + 1] != boulder) {
	  m -> character_positions[rand_x][rand_y] -> pacer_direction = down;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x, rand_y + 1);
	    m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	    m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	}
	else if(m -> generate_map[rand_x][rand_y - 1] != tree ||
	   m -> generate_map[rand_x][rand_y - 1] != boulder) {
	  	  m -> character_positions[rand_x][rand_y] -> pacer_direction = up;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x, rand_y - 1);
	    m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	    m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	}
	else if(m -> generate_map[rand_x + 1][rand_y] != tree ||
	   m -> generate_map[rand_x + 1][rand_y] != boulder) {
	  	  m -> character_positions[rand_x][rand_y] -> pacer_direction = right;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x + 1, rand_y);
	    m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	    m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	}
	else if(m -> generate_map[rand_x - 1][rand_y] != tree ||
	   m -> generate_map[rand_x - 1][rand_y] != boulder) {
	  	  m -> character_positions[rand_x][rand_y] -> pacer_direction = left;
	  m -> character_positions[rand_x][rand_y] -> cost_to_move +=
	    determine_cost_rival(m, rand_x - 1, rand_y);
	    m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
	    m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
	}
	break;
	
      case rival:

  if( distance_rival[rand_x + 1][rand_y].cost < cost_to_move) {
    min_x_next = rand_x + 1;
    min_y_next = rand_y;
    
    cost_to_move = distance_rival[rand_x + 1][rand_y].cost;
  }

  if( distance_rival[rand_x][rand_y + 1].cost < cost_to_move) {
    min_x_next = rand_x;
    min_y_next = rand_y + 1;
    
    cost_to_move = distance_rival[rand_x][rand_y + 1].cost;
  }

  if( distance_rival[rand_x - 1][rand_y].cost < cost_to_move) {
    min_x_next = rand_x - 1;
    min_y_next = rand_y;
    
    cost_to_move = distance_rival[rand_x - 1][rand_y].cost;
  }

  if( distance_rival[rand_x][rand_y - 1].cost < cost_to_move) {
    min_x_next = rand_x;
    min_y_next = rand_y - 1;
    
    cost_to_move = distance_rival[rand_x][rand_y - 1].cost;
  }

  if( distance_rival[rand_x + 1][rand_y + 1].cost < cost_to_move) {
    min_x_next = rand_x + 1;
    min_y_next = rand_y + 1;
    
    cost_to_move = distance_rival[rand_x + 1][rand_y + 1].cost;
  }

  if( distance_rival[rand_x + 1][rand_y - 1].cost < cost_to_move) {
    min_x_next = rand_x + 1;
    min_y_next = rand_y - 1;
    
    cost_to_move = distance_rival[rand_x + 1][rand_y - 1].cost;
  }

  if( distance_rival[rand_x - 1][rand_y + 1].cost < cost_to_move) {
    min_x_next = rand_x - 1;
    min_y_next = rand_y + 1;
    
    cost_to_move = distance_rival[rand_x - 1][rand_y + 1].cost;
  }

  if( distance_rival[rand_x - 1][rand_y - 1].cost < cost_to_move) {
    min_x_next = rand_x - 1;
    min_y_next = rand_y - 1;
    
    cost_to_move = distance_rival[rand_x - 1][rand_y - 1].cost;
  }
  
  
  m -> character_positions[rand_x][rand_y] += cost_to_move;
  m -> character_positions[rand_x][rand_y] -> x_pos = min_x_next;
  m -> character_positions[rand_x][rand_y] -> y_pos = min_y_next;
	
	break;

      case hiker:

	break; 
      }
      heap_insert(h, m -> character_positions[rand_x][rand_y]);
      placed_characters++;
    }
  }

}

void move_pacer(generated_map_t *m, character_t *pacer_to_move, heap_t *h) {
 
  // Gonna be honest, there were more edge cases in this than I thought. So, this is probabably gonna
  // look really sloppy.

      printf("X: %d Y: %d\n", pacer_to_move -> x_pos, pacer_to_move -> y_pos);
  
  switch(pacer_to_move -> pacer_direction) {
  case down:
    printf("got into down\n");
    if( pacer_to_move -> y_pos + 1 < 21 ) {
      if(
	(m -> generate_map[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] != tree) &&
	(m -> generate_map[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] != boulder) &&
	(m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] == NULL)) {
      printf("before assignments\n");
      
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] = malloc(sizeof(character_t));
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] -> player_type = pacer;
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] -> cost_to_move = pacer_to_move -> cost_to_move;
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] -> x_pos = pacer_to_move -> x_pos;
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] -> y_pos = pacer_to_move -> y_pos + 1;
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] -> pacer_direction = down;
      pacer_to_move -> y_pos++;

      printf("got past assignments\n");
      if( pacer_to_move -> y_pos + 1 < 21) {
	if (m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] == NULL &&
	    m -> generate_map[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] != tree &&
	    m -> generate_map[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] != boulder) {
	  
	  m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos] -> cost_to_move = 
	    m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos] -> cost_to_move +
	    determine_cost_rival(m, pacer_to_move -> x_pos, pacer_to_move -> y_pos + 1);	  
	}
      }
      
      heap_insert(h, m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos]);
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] = NULL;
      free(m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1]);
      printf("upon exit X: %d Y: %d\n", pacer_to_move -> x_pos, pacer_to_move -> y_pos);
      break;
      }
    }
    else {
      printf("got into else statement");
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos] -> pacer_direction = up;
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos] -> cost_to_move +=
	    determine_cost_rival(m, pacer_to_move -> x_pos, pacer_to_move -> y_pos + 1);
      heap_insert(h,  m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos]);
      break;
    }
    
  case up:
    printf("got into up\n");
    printf("X: %d, Y - 1: %d\n", pacer_to_move -> x_pos, pacer_to_move -> y_pos - 1);

      if ( (pacer_to_move -> y_pos - 1 > 0) && 
	(m -> generate_map[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] != tree) &&
	(m -> generate_map[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] != boulder) &&
	(m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] == NULL)) {

	printf("got before updates\n");
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] = malloc(sizeof(character_t));
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] -> player_type = pacer;
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] -> cost_to_move = pacer_to_move -> cost_to_move;
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] -> x_pos = pacer_to_move -> x_pos;
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] -> y_pos = pacer_to_move -> y_pos - 1;
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] -> pacer_direction = up;
      pacer_to_move -> y_pos--;

      printf("got after updates\n");
      if( pacer_to_move -> y_pos - 1 > 0) {
	if (m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] == NULL &&
	    m -> generate_map[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] != tree &&
	    m -> generate_map[pacer_to_move -> x_pos][pacer_to_move -> y_pos - 1] != boulder) {

	  printf("got into cost updates\n");
	  m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos] -> cost_to_move +=
	    determine_cost_rival(m, pacer_to_move -> x_pos, pacer_to_move -> y_pos - 1);
	}
      }
      printf("got out of cost updates\n");
      
      heap_insert(h, m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos]);
      m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1] = NULL;
      free(m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos + 1]);

      printf("before break statement\n");
      break;
      }
    else {
	printf("went into else for up\n");
	m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos] -> pacer_direction = down;
	m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos] -> cost_to_move +=
	  determine_cost_rival(m, pacer_to_move -> x_pos, pacer_to_move -> y_pos + 1);
	heap_insert(h,  m -> character_positions[pacer_to_move -> x_pos][pacer_to_move -> y_pos]);
	break;
      }
    printf("didn't go into the thing\n");

    
  case right:
    break;
    
  case left:
    break;
    
  }
}

void move_via_shortest_path(generated_map_t *m, cost_t dijkstra[HORIZONTAL][VERTICAL], character_t *character_to_move, heap_t *h) {

  int min_x_next;
  int min_y_next;
  int character_x_coord = character_to_move -> x_pos;
  int character_y_coord = character_to_move -> y_pos;
  int cost_to_move = INT_MAX;

   if(m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] == NULL) {
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] = malloc(sizeof(character_t));
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> player_type = character_to_move -> player_type;

    character_to_move -> x_pos = character_to_move -> next_x;
    character_to_move -> y_pos = character_to_move -> next_y; 
  
  if( dijkstra[character_to_move -> x_pos + 1][character_to_move -> y_pos].cost < cost_to_move) {
    min_x_next = character_to_move -> x_pos + 1;
    min_y_next = character_to_move -> y_pos;
    
    cost_to_move = dijkstra[character_to_move -> x_pos + 1][character_to_move -> y_pos].cost;
  }
  
  if( dijkstra[character_to_move -> x_pos][character_to_move -> y_pos + 1].cost < cost_to_move) {
    min_x_next = character_to_move -> x_pos;
    min_y_next = character_to_move -> y_pos + 1;
    
    cost_to_move = dijkstra[character_to_move -> x_pos - 1][character_to_move -> y_pos + 1].cost;
  }
  
  if( dijkstra[character_to_move -> x_pos - 1][character_to_move -> y_pos].cost < cost_to_move) {
    min_x_next = character_to_move -> x_pos - 1;
    min_y_next = character_to_move -> y_pos;
    
    cost_to_move = dijkstra[character_to_move -> x_pos][character_to_move -> y_pos - 1].cost;
  }
  
  if( dijkstra[character_to_move -> x_pos][character_to_move -> y_pos - 1].cost < cost_to_move) {
    min_x_next = character_to_move -> x_pos;
    min_y_next = character_to_move -> y_pos - 1;
    
    cost_to_move = dijkstra[character_to_move -> x_pos][character_to_move -> y_pos - 1].cost;
  }
  
  if( dijkstra[character_to_move -> x_pos + 1][character_to_move -> y_pos + 1].cost < cost_to_move) {
    min_x_next = character_to_move -> x_pos + 1;
    min_y_next = character_to_move -> y_pos + 1;
    
    cost_to_move = dijkstra[character_to_move -> x_pos + 1][character_to_move -> y_pos + 1].cost;
  }
  
  
  if( dijkstra[character_to_move -> x_pos + 1][character_to_move -> y_pos - 1].cost < cost_to_move) {
    min_x_next = character_to_move -> x_pos + 1;
    min_y_next = character_to_move -> y_pos - 1;
    
    cost_to_move = dijkstra[character_to_move -> x_pos + 1][character_to_move -> y_pos - 1].cost;
  }
  
  
  if( dijkstra[character_to_move -> x_pos - 1][character_to_move -> y_pos + 1].cost < cost_to_move) {
    min_x_next = character_to_move -> x_pos - 1;
    min_y_next = character_to_move -> y_pos + 1;
    
    cost_to_move = dijkstra[character_to_move -> x_pos - 1][character_to_move -> y_pos + 1].cost;
  }
  
  
  if( dijkstra[character_to_move -> x_pos - 1][character_to_move -> y_pos - 1].cost < cost_to_move) {
    min_x_next = character_to_move -> x_pos - 1;
    min_y_next = character_to_move -> y_pos - 1;
    
    cost_to_move = dijkstra[character_to_move -> x_pos - 1][character_to_move -> y_pos - 1].cost;
  }
  
  m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> cost_to_move += cost_to_move;
  m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> next_x = min_x_next;
  m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> next_y = min_y_next;
  
  heap_insert(h, m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos]);
  
  m -> character_positions[character_x_coord][character_y_coord] = NULL;
  free(m -> character_positions[character_x_coord][character_y_coord]);
   }
   else{
     heap_insert(h, m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos]);
   }
}
