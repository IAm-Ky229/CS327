#define WORLD_X_LENGTH 399
#define WORLD_Y_LENGTH 399
#define WORLD_X_START 199
#define WORLD_Y_START 199

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
