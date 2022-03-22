#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>

#include "terrain_generation.h"
#include "world_generation.h"


static int32_t path_cmp(const void *key, const void *with) {
  return ((cost_t *) key)->cost - ((cost_t *) with)->cost;
}

static int32_t move_cost_cmp(const void *key, const void *with) {
  return ((character_t *) key)-> cost_to_move - ((character_t *) with)-> cost_to_move;
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
  int numtrainers = -1;

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

  // Get the user's input
  char user_input;

  // We only want random placement when we enter the first map
  int first_map = -1;

  // curses init
  initscr();
  cbreak();
  halfdelay(1);
  noecho();


  // Assign the number of trainers
  if(argc == 3) {
    if(strcmp(argv[1], "--numtrainers") == 0)
      {
	numtrainers = atoi(argv[2]);
      }
  }
  else {
    numtrainers = 10;
  }

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
 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
 heap_init(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move, move_cost_cmp, NULL);
 generate_new_map(map_exploration[y_explore_position][x_explore_position],
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
   
   character_t *to_move;
   if(map_exploration[y_explore_position][x_explore_position] -> characters_to_move.size != 0) {
     to_move = heap_peek_min(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move);

     char buffer[50];
     sprintf(buffer, "moving X: %d Y: %d cost: %d", to_move -> x_pos, to_move -> y_pos, to_move -> cost_to_move);
     mvaddstr(21, 30, buffer);
     refresh();
     
     while(to_move -> cost_to_move <= map_exploration[y_explore_position][x_explore_position] -> game_time && (map_exploration[y_explore_position][x_explore_position] -> game_time != 0)) {
       
       to_move = heap_remove_min(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move);

       switch (to_move -> player_type) {
       case random_walker:
	 move_random_walker(map_exploration[y_explore_position][x_explore_position], to_move, &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
	 break;
	 
       case wanderer:
	 move_wanderer(map_exploration[y_explore_position][x_explore_position], to_move, &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
	 break;
	 
       case pacer:
	 move_pacer(map_exploration[y_explore_position][x_explore_position], to_move, &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
	 break;
	 
       case hiker:
	 dijkstra_path_hiker(map_exploration[y_explore_position][x_explore_position], distance_hiker, map_exploration[y_explore_position][x_explore_position] -> PC_position_x , map_exploration[y_explore_position][x_explore_position] -> PC_position_y);
	 move_via_shortest_path(map_exploration[y_explore_position][x_explore_position], distance_hiker, to_move, &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
	 break;
	 
       case rival:
	 dijkstra_path_hiker(map_exploration[y_explore_position][x_explore_position], distance_rival, map_exploration[y_explore_position][x_explore_position] -> PC_position_x, map_exploration[y_explore_position][x_explore_position] -> PC_position_y);
	 move_via_shortest_path(map_exploration[y_explore_position][x_explore_position], distance_rival, to_move, &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
	 break;

       case PC:
	 move_PC(to_move, map_exploration[y_explore_position][x_explore_position]);
	 break;
	 
       }
       
       to_move = heap_peek_min(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
       
     }
   }

   print_map(map_exploration[y_explore_position][x_explore_position]);
   //usleep(150000);
   map_exploration[y_explore_position][x_explore_position] -> game_time++;

   switch(user_input) {

   case '7':
     mvaddstr(22, 30, "got 7");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case 'y':
     mvaddstr(22, 30, "got y");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;
     
   case '8':
     mvaddstr(22, 30, "got 8");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case 'k':
     mvaddstr(22, 30, "got k");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case '9':
     mvaddstr(22, 30, "got 9");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case 'u':
     mvaddstr(22, 30, "got u");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y - 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case '6':
     mvaddstr(22, 30, "got 6");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case 'l':
     mvaddstr(22, 30, "got l");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case '3':
     mvaddstr(22, 30, "got 3");
     
     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case 'n':
     mvaddstr(22, 30, "got n");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x + 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;
     
   case '2':
     mvaddstr(22, 30, "got 2");
     
     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case 'j':
     mvaddstr(22, 30, "got j");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;
     
   case '1':
     mvaddstr(22, 30, "got 1");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case 'b':
     mvaddstr(22, 30, "got b");

     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y + 1,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case '4':
     mvaddstr(22, 30, "got 4");
     
     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
     break;

   case 'h':
     mvaddstr(22, 30, "got h");
     
     attempt_move_PC(map_exploration[y_explore_position][x_explore_position] -> PC_position_x - 1,
		     map_exploration[y_explore_position][x_explore_position] -> PC_position_y,
		     map_exploration[y_explore_position][x_explore_position],
		     &map_exploration[y_explore_position][x_explore_position] -> characters_to_move);
     
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
     mvaddstr(22, 30, "got t");
     break;

   case 'q':
     quit_game = 1;
     endwin();
     break;
     
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
       map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
       
       check_exits(map_exploration,
		   x_explore_position,
		   y_explore_position,
		   &exit_bottom, &exit_right, &exit_left, &exit_top);
       heap_init(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move, move_cost_cmp, NULL);
       PC_road_x = 1;
       PC_road_y = exit_left;
       generate_new_map(map_exploration[y_explore_position][x_explore_position],
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
       check_exits(map_exploration,
		   x_explore_position,
		   y_explore_position,
		   &exit_bottom, &exit_right, &exit_left, &exit_top);
       PC_road_x = 1;
       PC_road_y = exit_left;

       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] = malloc(sizeof(character_t));
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> player_type = PC;
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> x_pos = PC_road_x;
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> y_pos = PC_road_y;
       
       map_exploration[y_explore_position][x_explore_position] -> PC_position_x = PC_road_x;
       map_exploration[y_explore_position][x_explore_position] -> PC_position_y = PC_road_y;
       
       print_map(map_exploration[y_explore_position][x_explore_position]);
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
       map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
       
       check_exits(map_exploration,
		   x_explore_position,
		   y_explore_position,
		   &exit_bottom, &exit_right, &exit_left, &exit_top);
       heap_init(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move, move_cost_cmp, NULL);
       PC_road_x = 78;
       PC_road_y = exit_right;
       generate_new_map(map_exploration[y_explore_position][x_explore_position],
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
       check_exits(map_exploration,
		   x_explore_position,
		   y_explore_position,
		   &exit_bottom, &exit_right, &exit_left, &exit_top);
       PC_road_x = 78;
       PC_road_y = exit_right;

       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] = malloc(sizeof(character_t));
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> player_type = PC;
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> x_pos = PC_road_x;
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> y_pos = PC_road_y;
       
       map_exploration[y_explore_position][x_explore_position] -> PC_position_x = PC_road_x;
       map_exploration[y_explore_position][x_explore_position] -> PC_position_y = PC_road_y;
       
       print_map(map_exploration[y_explore_position][x_explore_position]);
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
       map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
       
       check_exits(map_exploration,
		   x_explore_position,
		   y_explore_position,
		   &exit_bottom, &exit_right, &exit_left, &exit_top);
       heap_init(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move, move_cost_cmp, NULL);
       PC_road_x = exit_bottom;
       PC_road_y = 19;
       generate_new_map(map_exploration[y_explore_position][x_explore_position],
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
       check_exits(map_exploration,
		   x_explore_position,
		   y_explore_position,
		   &exit_bottom, &exit_right, &exit_left, &exit_top);
       PC_road_x = exit_bottom;
       PC_road_y = 19;

       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] = malloc(sizeof(character_t));
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> player_type = PC;
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> x_pos = PC_road_x;
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> y_pos = PC_road_y;
       
       map_exploration[y_explore_position][x_explore_position] -> PC_position_x = PC_road_x;
       map_exploration[y_explore_position][x_explore_position] -> PC_position_y = PC_road_y;
       
       print_map(map_exploration[y_explore_position][x_explore_position]);
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
       map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
       
       check_exits(map_exploration,
		   x_explore_position,
		   y_explore_position,
		   &exit_bottom, &exit_right, &exit_left, &exit_top);
       heap_init(&map_exploration[y_explore_position][x_explore_position] -> characters_to_move, move_cost_cmp, NULL);
       PC_road_x = exit_top;
       PC_road_y = 1;
       generate_new_map(map_exploration[y_explore_position][x_explore_position],
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
       check_exits(map_exploration,
		   x_explore_position,
		   y_explore_position,
		   &exit_bottom, &exit_right, &exit_left, &exit_top);
       PC_road_x = exit_top;
       PC_road_y = 1;

       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] = malloc(sizeof(character_t));
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> player_type = PC;
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> x_pos = PC_road_x;
       map_exploration[y_explore_position][x_explore_position] -> character_positions[PC_road_x][PC_road_y] -> y_pos = PC_road_y;
       
       map_exploration[y_explore_position][x_explore_position] -> PC_position_x = PC_road_x;
       map_exploration[y_explore_position][x_explore_position] -> PC_position_y = PC_road_y;
       
       print_map(map_exploration[y_explore_position][x_explore_position]);
     }
   }
   
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
		      int PC_path_x,
		      int PC_path_y,
		      cost_t distance_hiker[HORIZONTAL][VERTICAL],
		      cost_t distance_rival[HORIZONTAL][VERTICAL],
		      int numtrainers,
		      int *first_map) {
  
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
  // Only randomize position for the first map
  if(*first_map == -1) {
  choose_random_road_spot(map_data, &PC_path_x, &PC_path_y);
  *first_map = 1;
  }
 
  map_data -> character_positions[PC_path_x][PC_path_y] = malloc(sizeof(character_t));
  map_data -> character_positions[PC_path_x][PC_path_y] -> player_type = PC;
  map_data -> character_positions[PC_path_x][PC_path_y] -> x_pos = PC_path_x;
  map_data -> character_positions[PC_path_x][PC_path_y] -> y_pos = PC_path_y;

  map_data -> PC_position_x = PC_path_x;
  map_data -> PC_position_y = PC_path_y;

  dijkstra_path_rival(map_data, distance_rival, PC_path_x, PC_path_y);
  dijkstra_path_hiker(map_data, distance_hiker, PC_path_x, PC_path_y);
  
  place_characters(map_data, h, distance_hiker, distance_rival, numtrainers);
  
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

void choose_random_road_spot(generated_map_t *map_data, int *chosen_spot_x, int *chosen_spot_y) {

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
  case stationary_occupied:
    return INT_MAX;
  default:
    return 0;
  }
}

int determine_cost_PC(generated_map_t *map_data, int x_dim, int y_dim) {

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
  case stationary_occupied:
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
}

void place_characters(generated_map_t *m, heap_t *h, cost_t distance_hiker[HORIZONTAL][VERTICAL], cost_t distance_rival[HORIZONTAL][VERTICAL], int numtrainers) {

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
     
      m -> character_positions[rand_x][rand_y] = malloc(sizeof(character_t));
      m -> character_positions[rand_x][rand_y] -> player_type = characters_to_place[placed_characters];
      m -> character_positions[rand_x][rand_y] -> cost_to_move = 0;
      m -> character_positions[rand_x][rand_y] -> x_pos = rand_x;
      m -> character_positions[rand_x][rand_y] -> y_pos = rand_y;
      
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
	m -> generate_map[rand_x][rand_y] = stationary_occupied;
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

      //printf("inserting character\n");
      heap_insert(h, m -> character_positions[rand_x][rand_y]);
      placed_characters++;
    }
  }

}

void move_pacer(generated_map_t *m, character_t *pacer_to_move, heap_t *h) {
  
  int current_x = pacer_to_move -> x_pos;
  int current_y = pacer_to_move -> y_pos;

  int last_x;
  int last_y;

  // Always try to move the pacer
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

    m -> character_positions[last_x][last_y] = NULL;
    free(m -> character_positions[last_x][last_y]);
  }

  // Always find the next move so we don't get stuck
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
  
}

void move_up(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> y_pos - 1 > 0 ) {
	if(
	   (m -> generate_map[current_x][current_y - 1] != tree) &&
	   (m -> generate_map[current_x][current_y - 1] != boulder) &&
	   (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
	   (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
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
	   (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
	   (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
	   (m -> character_positions[current_x][current_y + 1] == NULL)) {
	  m -> character_positions[current_x][current_y] -> cost_to_move += determine_cost_rival(m, current_x, current_y + 1);
	  m -> character_positions[current_x][current_y] -> next_x = current_x;
	  m -> character_positions[current_x][current_y] -> next_y  = current_y + 1;
	  
	  heap_insert(h, m -> character_positions[current_x][current_y]);
	}
	else {
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
	   (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
	   (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
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
	   (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
	   (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
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

  // I really shouldn't have used INT_MAX because my comparisons are messed up but idk if I'm gonna change it
  int cost_to_move = INT_MAX;

  // Check to see if we are going to run into another character
  if(m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] == NULL) {
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] = malloc(sizeof(character_t));
    m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> player_type = character_to_move -> player_type;
    
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

    char buffer[50];
    sprintf(buffer, "min X: %d min Y: %d cost: %d", min_x_next, min_y_next, m -> character_positions[x_position][y_position] -> cost_to_move);
    mvaddstr(22, 30, buffer);
    refresh();


    
      // I have no idea why this is adding 10 :(
      // So I just subtracted 10

      m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> cost_to_move += determine_cost_rival(m, min_x_next, min_y_next);
      m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> next_x = min_x_next;
      m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> next_y = min_y_next;
      
      m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> x_pos = x_position;
      m -> character_positions[character_to_move -> next_x][character_to_move -> next_y] -> y_pos = y_position;
      
      m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] = NULL;
      free(m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos]);
      heap_insert(h, m -> character_positions[x_position][y_position]);

    
  }
  else {
    m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos] -> cost_to_move +=
      determine_cost_rival(m, character_to_move -> x_pos, character_to_move -> y_pos);
    heap_insert(h, m -> character_positions[character_to_move -> x_pos][character_to_move -> y_pos]);
  }
}



void move_wanderer(generated_map_t *m, character_t *wanderer_to_move, heap_t *h) {

  int current_x = wanderer_to_move -> x_pos;
  int current_y = wanderer_to_move -> y_pos;

  int last_x;
  int last_y;

  // Check to see if the move is valid first
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
    
    m -> character_positions[last_x][last_y] = NULL;
    free(m -> character_positions[last_x][last_y]);

  }

  // Always check for a new move or we will stop
  // If we find we can't move the assigned direction,
  // We will attempt to move a random direction that
  // Isn't the same direction we were just moving
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

}

void move_up_random(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> y_pos - 1 >= 0 ) {
	if(
	   (m -> generate_map[current_x][current_y - 1] != tree) &&
	   (m -> generate_map[current_x][current_y - 1] != boulder) &&
	   (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
	   (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
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

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
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
		 (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
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
		 (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
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

if( character_to_move -> y_pos + 1 < 21 ) {
	if(
	   (m -> generate_map[current_x][current_y + 1] != tree) &&
	   (m -> generate_map[current_x][current_y + 1] != boulder) &&
	   (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
	   (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
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


	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y - 1] != tree) &&
		 (m -> generate_map[current_x][current_y - 1] != boulder) &&
		 (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
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
		 (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
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
		 (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
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
	   (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
	   (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
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

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
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
		 (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
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
		 (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
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

if( character_to_move -> x_pos - 1 >= 0 ) {
	if(
	   (m -> generate_map[current_x - 1][current_y] != tree) &&
	   (m -> generate_map[current_x - 1][current_y] != boulder) &&
	   (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
	   (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
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

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
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
		 (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
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
		 (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
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

  int current_x = walker_to_move -> x_pos;
  int current_y = walker_to_move -> y_pos;;

  int last_x;
  int last_y;

  // We only want to move if it's valid
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
    
    
    m -> character_positions[last_x][last_y] = NULL;
    free(m -> character_positions[last_x][last_y]);

  }

  // Always check for a new move or we will stop
  // Random walker moves are a like wanderer moves
  // But they don't check for same terrain
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
}

void move_left_walker(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y) {

if( character_to_move -> x_pos - 1 > 0 ) {
	if(
	   (m -> generate_map[current_x - 1][current_y] != tree) &&
	   (m -> generate_map[current_x - 1][current_y] != boulder) &&
	   (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
	   (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
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

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
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
		 (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
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
		 (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
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
	   (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
	   (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
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

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
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
		 (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
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
		 (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
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
	   (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
	   (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
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

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y + 1] != tree) &&
		 (m -> generate_map[current_x][current_y + 1] != boulder) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
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
		 (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
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
		 (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
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
	   (m -> generate_map[current_x][current_y + 1] != pokemon_mart) &&
	   (m -> generate_map[current_x][current_y + 1] != pokemon_center) &&
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

	    direction_to_move = rand() % 3;
	  
	    switch (direction_to_move) {
	      
	    case 0:
	      if(( m -> generate_map[current_x][current_y - 1] != tree) &&
		 (m -> generate_map[current_x][current_y - 1] != boulder) &&
		 (m -> generate_map[current_x][current_y - 1] != pokemon_mart) &&
		 (m -> generate_map[current_x][current_y - 1] != pokemon_center) &&
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
		 (m -> generate_map[current_x + 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x + 1][current_y] != pokemon_center) &&
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
		 (m -> generate_map[current_x - 1][current_y] != pokemon_mart) &&
		 (m -> generate_map[current_x - 1][current_y] != pokemon_center) &&
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

void attempt_move_PC(int x_move, int y_move, generated_map_t *m, heap_t *h) {
  char buffer[50];

  if(x_move >= 0
     && x_move < 80
     && y_move >= 0
     && y_move < 21) {
    if(m -> generate_map[x_move][y_move] != boulder &&
       m -> generate_map[x_move][y_move] != tree &&
       m -> character_positions[x_move][y_move] == NULL) {
      m -> character_positions[m -> PC_position_x][m -> PC_position_y] -> next_x = x_move;
      m -> character_positions[m -> PC_position_x][m -> PC_position_y] -> next_y = y_move;
      m -> character_positions[m -> PC_position_x][m -> PC_position_y] -> cost_to_move = m -> game_time + determine_cost_PC(m, x_move, y_move);

      int x = m -> PC_position_x;
      int y = m -> PC_position_y;
      
      heap_insert(h, m -> character_positions[x][y]);
    }
  }
  
}

void move_PC(character_t *player_char, generated_map_t *m) {

  if(m -> character_positions[player_char -> next_x][player_char -> next_y] == NULL) {

    int prev_x = player_char -> x_pos;
    int prev_y = player_char -> y_pos;

    int next_x_move = player_char -> next_x;
    int next_y_move = player_char -> next_y;

    m -> character_positions[next_x_move][next_y_move] = malloc(sizeof(character_t));
    m -> character_positions[next_x_move][next_y_move] -> player_type = PC;
    
    m -> character_positions[next_x_move][next_y_move] -> x_pos = next_x_move;
    m -> character_positions[next_x_move][next_y_move] -> y_pos = next_y_move;

    m -> PC_position_x = next_x_move;
    m -> PC_position_y = next_y_move;

    m -> character_positions[prev_x][prev_y] = NULL;
    free(m -> character_positions[prev_x][prev_y]);
  }
  
}
