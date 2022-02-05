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
 generate_new_map(map_exploration[y_explore_position][x_explore_position]);
 
 // Continually get buffered input till we quit
 while(1) {
   scanf(" %c", &scanned);

   switch(scanned) {
   case 'n':
     printf("going north\n");
     if (map_exploration[y_explore_position - 1][x_explore_position] == NULL) {
       y_explore_position--;
       map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
       generate_new_map(map_exploration[y_explore_position][x_explore_position]);
     }
     else {
       y_explore_position--;
       print_map(map_exploration[y_explore_position][x_explore_position]);
     }
     break;
     
   case 's':
     printf("going south\n");
     if (map_exploration[y_explore_position + 1][x_explore_position] == NULL) {
       y_explore_position++;
       map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
       generate_new_map(map_exploration[y_explore_position][x_explore_position]);
     }
     else {
       y_explore_position++;
       print_map(map_exploration[y_explore_position][x_explore_position]);
     }
     break;
     
   case 'e':
     printf("going east\n");
     if (map_exploration[y_explore_position][x_explore_position + 1] == NULL) {
       x_explore_position++;
       map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
       generate_new_map(map_exploration[y_explore_position][x_explore_position]);
     }
     else {
       x_explore_position++;
       print_map(map_exploration[y_explore_position][x_explore_position]);
     }
     break;
     
   case 'w':
     printf("going west\n");
     if (map_exploration[y_explore_position][x_explore_position - 1] == NULL) {
       x_explore_position--;
       map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
       generate_new_map(map_exploration[y_explore_position][x_explore_position]);
     }
     else {
       x_explore_position--;
       print_map(map_exploration[y_explore_position][x_explore_position]);
     }
     break;
     
   case 'f':
     printf("enter an X and a Y to fly to:\n");
     scanf("%d", &x_explore_position);
     scanf("%d", &y_explore_position);
     printf("flying to (%d, %d)\n", x_explore_position, y_explore_position);
     
     if (map_exploration[y_explore_position][x_explore_position] == NULL) {
       map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
       generate_new_map(map_exploration[y_explore_position][x_explore_position]);
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

void generate_new_map(generated_map_t *map_data) {
  
  // I was getting some weird memory problems without initializing
  // everything to nothing
  int i;
  int j;
  
  for(i = 0; i < VERTICAL; i++) {
    for(j = 0; j < HORIZONTAL; j++) {
      map_data -> generate_map[j][i] = nothing;
    }
  }
  
  // Function calls need to happen in this order
  place_clearings(map_data);
  place_tall_grass(map_data);
  place_border_boulders(map_data);
  place_exits(map_data);
  place_paths(map_data);
  place_buildings(map_data);
  fill_blank_space(map_data);
  
  print_map(map_data);
  
}
