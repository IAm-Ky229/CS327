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
} cost_t;

// Given the parameters, generate a new map for our world
void generate_new_map(generated_map_t *map_data,
		      int exit_bottom,
		      int exit_right,
		      int exit_left,
		      int exit_top,
		      int manhattan_x,
		      int manhattan_y);

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
static void dijkstra_path_rival(generated_map_t *m, int from_x, int from_y);

// Actually navigate through the map
static void dijkstra_path_hiker(generated_map_t *m, int from_x, int from_y);

// Place characters into the map
void place_characters(generated_map_t *m);
