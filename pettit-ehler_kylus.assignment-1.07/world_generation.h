#define WORLD_X_LENGTH 400
#define WORLD_Y_LENGTH 400
#define WORLD_X_START 199
#define WORLD_Y_START 199

#define GRASS_PAIR 1
#define PATH_PAIR 2
#define PLAYER_PAIR 3
#define BOULDER_PAIR 4
#define TREE_PAIR 5

#include "heap.h"
#include "wild_battle.h"
#include "character_movement.h"

using String = std::string;

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

class displayList {

public:
  
  // Update the list of characters printed
  void update_list(generatedMap *m, NPC_char *list_copy, int window, int size);
  
};

