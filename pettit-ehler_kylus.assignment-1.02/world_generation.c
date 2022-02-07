#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "terrain_generation.h"
#include "world_generation.h"

int main(int argc, char * argv[]) {

  int x_explore_position;
  int y_explore_position;

  char scanned;

  generated_map_t *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH];

  // Keep track of exits
  int exit_bottom = -1;
  int exit_right = -1;
  int exit_left = -1;
  int exit_top = -1;

  // Track x and y distance away from center
  int manhattan_x = 0;
  int manhattan_y = 0;

  // Be sure that the world is initialized to be completely null
  int i;
  int j;

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

 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
 generate_new_map(map_exploration[y_explore_position][x_explore_position],
		  exit_bottom,
		  exit_right,
		  exit_left,
		  exit_top,
		  manhattan_x,
		  manhattan_y);
 
 // Continually get buffered input till we quit
 while(1) {
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
     break;
     
   case 's':
     printf("going south\n");
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
     break;
     
   case 'e':
     printf("going east\n");
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
     break;
     
   case 'w':
     printf("going west\n");
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
     break;
     
   case 'f':
     printf("enter an X and a Y to fly to:\n");
     scanf("%d", &x_explore_position);
     scanf("%d", &y_explore_position);
     printf("flying to (%d, %d)\n", x_explore_position, y_explore_position);

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
     break;
     
   case 'q':
     printf("quitting game\n");
     return 0;
     
   default:
     printf("that is not a valid command\n");
     break;
   }
   
 }
 
 return 0;
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


  // Calculate manhattan distance
  int manhattan_distance = abs(manhattan_x) + abs(manhattan_y);

  // Do the actual spawn rate conversion (I don't divide by 100)
  int building_spawn_rate = (((-45 * manhattan_distance) / 200) + 50);
  
  // Function calls need to happen in this order
  place_clearings(map_data);
  place_tall_grass(map_data);
  place_border_boulders(map_data);
  place_exits(map_data, exit_bottom, exit_right, exit_left, exit_top);
  place_paths(map_data);
  place_buildings(map_data, building_spawn_rate);
  fill_blank_space(map_data);
  
  print_map(map_data);
  
}

void check_exits(generated_map_t *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH],
		 int x_explore,
		 int y_explore,
		 int *exit_bottom,
		 int *exit_right,
		 int *exit_left,
		 int *exit_top) {

  if(map_exploration[y_explore + 1][x_explore] != NULL) {
    *exit_bottom = map_exploration[y_explore + 1][x_explore] -> exit_top;
  }

  if(map_exploration[y_explore][x_explore + 1] != NULL) {
    *exit_right = map_exploration[y_explore][x_explore + 1] -> exit_left;
  }

  if(map_exploration[y_explore - 1][x_explore] != NULL) {
    *exit_top = map_exploration[y_explore - 1][x_explore] -> exit_bottom;
  }
  
  if(map_exploration[y_explore][x_explore - 1] != NULL) {
    *exit_left = map_exploration[y_explore][x_explore - 1] -> exit_right;
  }
  
}
