#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <limits.h>

#include "terrain_generation.h"
#include "world_generation.h"


static int32_t path_cmp(const void *key, const void *with) {
  return ((cost_t *) key)->cost - ((cost_t *) with)->cost;
}

int main(int argc, char * argv[]) {
  
  // Keep track of place in world
  int x_explore_position;
  int y_explore_position;

  // Used to select some road spot
  int road_spot_x;
  int road_spot_y;

  // The scanned input character
  char scanned;

  // Map we are going to traverse
  generated_map_t *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH];

  // Map to store characters
  character_t *player_positions[WORLD_X_LENGTH][WORLD_Y_LENGTH];

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

 // Make first map
 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
 generate_new_map(map_exploration[y_explore_position][x_explore_position],
		  exit_bottom,
		  exit_right,
		  exit_left,
		  exit_top,
		  manhattan_x,
		  manhattan_y);

 choose_random_road_spot(map_exploration[y_explore_position][x_explore_position], &road_spot_x, &road_spot_y);

 printf("Random road spot X: %d\n", road_spot_x);
 printf("Random road spot Y: %d\n", road_spot_y);
 dijkstra_path_rival(map_exploration[y_explore_position][x_explore_position], road_spot_x, road_spot_y);
 dijkstra_path_hiker(map_exploration[y_explore_position][x_explore_position], road_spot_x, road_spot_y);
 
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
		      int exit_bottom,
		      int exit_right,
		      int exit_left,
		      int exit_top,
		      int manhattan_x,
		      int manhattan_y) {
  
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
  place_characters(map_data);

  // To avoid passing in y and x explore, I just add 199 to the manhattan counters
  check_edge_cases(map_data, manhattan_y + 199, manhattan_x + 199);
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

static void dijkstra_path_rival(generated_map_t *m, int from_x, int from_y)
{

  // We'll use this to store and compute costs
  cost_t dijkstra[HORIZONTAL][VERTICAL], *p;

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
}

// This is the same as dijkstra_path_rival, we're just calling a different cost computation function
static void dijkstra_path_hiker(generated_map_t *m, int from_x, int from_y)
{

  // We'll use this to store and compute costs
  cost_t dijkstra[HORIZONTAL][VERTICAL], *p;
  
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
      }
    }
    
    
    if(p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos)));
      }
    }
    
    if(p->y_pos - 1 > 0) {
      if (
	  (dijkstra[p->x_pos][p->y_pos - 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos][p->y_pos - 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos, p -> y_pos - 1)));
      }
    }
    
    
    if(p-> y_pos + 1 < VERTICAL) {
      if (
	  (dijkstra[p->x_pos][p->y_pos + 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos][p->y_pos + 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos, p -> y_pos + 1)));
      }
    }

    if(p-> y_pos + 1 < VERTICAL && p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos + 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos + 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos + 1)));
      }
    }

    if(p-> y_pos - 1 < VERTICAL && p-> x_pos + 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos + 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos + 1)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos + 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos + 1)));
      }
    }

    if(p-> y_pos - 1 < VERTICAL && p-> x_pos - 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos - 1][p->y_pos - 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos - 1][p->y_pos - 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos - 1, p -> y_pos - 1)));
      }
    }

    if(p-> y_pos + 1 < VERTICAL && p-> x_pos - 1 < HORIZONTAL) {
      if (
	  (dijkstra[p->x_pos + 1][p->y_pos - 1    ].cost) >
	  (p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos - 1)
	   )) {
	dijkstra[p->x_pos + 1][p->y_pos - 1    ].cost =
	  ((p->cost + determine_cost_hiker(m, p-> x_pos + 1, p -> y_pos - 1)));
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
}

void place_characters(generated_map_t *m) {

// Generate random positions for all the characters

// place them in the 2 - D array

// Once we make the map, go over the character array and print out all the characters

// I still am not really sure how to do the movement lol

m -> character_positions[1][1] = malloc(sizeof(character_t));

}
