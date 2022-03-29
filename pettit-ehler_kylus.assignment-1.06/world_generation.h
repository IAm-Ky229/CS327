#define WORLD_X_LENGTH 400
#define WORLD_Y_LENGTH 400
#define WORLD_X_START 199
#define WORLD_Y_START 199

#include "heap.h"

class movementCosts {

public:

  heap_node_t *hn;
  int x_pos;
  int y_pos;
  int visited;
  int next_x;
  int next_y;

  int cost;

  // Determine the cost we should assign, based on the terrain type
  int determine_cost_PC(generatedMap *map_data, int x_dim, int y_dim);
  
  // Determine the cost we should assign, based on the terrain type
  int determine_cost_rival(generatedMap *map_data, int x_dim, int y_dim);

  // Determine the cost we should assign, based on the terrain type
  int determine_cost_hiker(generatedMap *map_data, int x_dim, int y_dim);
 
};

// This stuff has to be static, so it can't be a member function

// Compare to form a path
static int32_t path_cmp(const void *key, const void *with);

// Compare to determine the minimum cost
static int32_t move_cost_cmp(const void *key, const void *with);

// Actually navigate through the map
static void dijkstra_path_rival(generatedMap *m, movementCosts dijkstra[HORIZONTAL][VERTICAL], int from_x, int from_y);

// Actually navigate through the map
static void dijkstra_path_hiker(generatedMap *m, movementCosts dijkstra[HORIZONTAL][VERTICAL],  int from_x, int from_y);

class mapGen {

 private:
  
 public:
  
  // Given the parameters, generate a new map for our world
  void generate_new_map(generatedMap *map_data,
			heap_t * h,
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
			int *first_map);

  // Check and see if our exits should be random, or they already exist
  void check_exits(generatedMap *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH],
		   int x_explore_position,
		   int y_explore_position,
		   int *exit_bottom,
		   int *exit_right,
		   int *exit_left,
		   int *exit_top);

  // Check to see if we should make any of the edges all boulders
  void check_edge_cases(generatedMap *map_data, int y_explore_position, int x_explore_position);

  // Choose random spot on the road for dijkstra computation
  void choose_random_road_spot(generatedMap *map_data, int *chosen_spot_x, int *chosen_spot_y);

};

class characterLogic {

 private:

 public:
// Place characters into the map
void place_characters(generatedMap *m, heap_t *h, movementCosts distance_hiker[HORIZONTAL][VERTICAL], movementCosts distance_rival[HORIZONTAL][VERTICAL], int numtrainers);

// Function for moving a pacer character
void move_pacer(generatedMap *m, NPC_char *pacer_to_move, heap_t *h);

// Function for moving a character via a gradient
void move_via_shortest_path(generatedMap *m, movementCosts dijkstra[HORIZONTAL][VERTICAL], NPC_char *character_to_move, heap_t * h);

// Move the specified character up
void move_up(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Move the specified character down
void move_down(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Move the specified character right
void move_right(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Move the specified character left
void move_left(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Move wanderer
void move_wanderer(generatedMap *m, NPC_char *wanderer_to_move, heap_t *h);

// Move the specified character up
void move_up_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_down_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_right_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_left_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Move random walker
void move_random_walker(generatedMap *m, NPC_char *wanderer_to_move, heap_t *h);

// Move the specified character up
void move_up_walker(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_down_walker(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_right_walker(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_left_walker(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

// Attempt to move the PC, assign next move
void attempt_move_PC(int x_move, int y_move, generatedMap *m, heap_t *h, int *PC_added_to_heap);

// Actually move the PC
void move_PC(PC_char *PC, generatedMap *m);

 // Placeholder for pokemon battle
void engage_battle();
};


class displayList {

 public:
  
// Update the list of characters printed
void update_list(generatedMap *m, NPC_char *list_copy, int window, int size);
  
};


