#define WORLD_X_LENGTH 400
#define WORLD_Y_LENGTH 400
#define WORLD_X_START 199
#define WORLD_Y_START 199

#include "heap.h"


typedef struct costs {
  heap_node_t *hn;
  int cost;
  int x_pos;
  int y_pos;
  int visited;
  int next_x;
  int next_y;
} cost_t;

// Given the parameters, generate a new map for our world
void generate_new_map(generated_map_t *map_data,
		      heap_t * h,
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
		      int *first_map);

// Check and see if our exits should be random, or they already exist
void check_exits(generated_map_t *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH],
		 int x_explore_position,
		 int y_explore_position,
		 int *exit_bottom,
		 int *exit_right,
		 int *exit_left,
		 int *exit_top);

// Check to see if we should make any of the edges all boulders
void check_edge_cases(generated_map_t *map_data, int y_explore_position, int x_explore_position);

// Choose random spot on the road for dijkstra computation
void choose_random_road_spot(generated_map_t *map_data, int *chosen_spot_x, int *chosen_spot_y);

// Determine the cost we should assign, based on the terrain type
int determine_cost_rival(generated_map_t *map_data, int x_dim, int y_dim);

// Determine the cost we should assign, based on the terrain type
int determine_cost_hiker(generated_map_t *map_data, int x_dim, int y_dim);

// Actually navigate through the map
static void dijkstra_path_rival(generated_map_t *m, cost_t dijkstra[HORIZONTAL][VERTICAL], int from_x, int from_y);

// Actually navigate through the map
static void dijkstra_path_hiker(generated_map_t *m, cost_t dijkstra[HORIZONTAL][VERTICAL],  int from_x, int from_y);

// Place characters into the map
void place_characters(generated_map_t *m, heap_t *h, cost_t distance_hiker[HORIZONTAL][VERTICAL], cost_t distance_rival[HORIZONTAL][VERTICAL], int numtrainers);

// Function for moving a pacer character
void move_pacer(generated_map_t *m, character_t *pacer_to_move, heap_t *h);

// Function for moving a character via a gradient
void move_via_shortest_path(generated_map_t *m, cost_t dijkstra[HORIZONTAL][VERTICAL], character_t *character_to_move, heap_t * h);

// Move the specified character up
void move_up(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Move the specified character down
void move_down(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Move the specified character right
void move_right(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Move the specified character left
void move_left(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Move wanderer
void move_wanderer(generated_map_t *m, character_t *wanderer_to_move, heap_t *h);

// Move the specified character up
void move_up_random(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_down_random(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_right_random(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_left_random(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Move random walker
void move_random_walker(generated_map_t *m, character_t *wanderer_to_move, heap_t *h);

// Move the specified character up
void move_up_walker(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_down_walker(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_right_walker(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Move the specified character up
void move_left_walker(generated_map_t *m, heap_t *h, character_t *character_to_move, int current_x, int current_y);

// Attempt to move the PC, assign next move
void attempt_move_PC(int x_move, int y_move, generated_map_t *m, heap_t *h);

// Actually move the PC
void move_PC(character_t *PC, generated_map_t *m);

// Determine the cost we should assign, based on the terrain type
int determine_cost_PC(generated_map_t *map_data, int x_dim, int y_dim);

// Update the list of characters printed
void update_list(generated_map_t *m, character_t *list_copy, int window, int size);

// Placeholder for pokemon battle
void engage_battle();
