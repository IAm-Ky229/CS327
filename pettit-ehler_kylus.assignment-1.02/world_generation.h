#define WORLD_X_LENGTH 399
#define WORLD_Y_LENGTH 399
#define WORLD_X_START 199
#define WORLD_Y_START 199


void generate_new_map(generated_map_t *map_data,
		      int exit_bottom,
		      int exit_right,
		      int exit_left,
		      int exit_top);

void check_exits(generated_map_t *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH],
		 int x_explore_position,
		 int y_explore_position,
		 int *exit_bottom,
		 int *exit_right,
		 int *exit_left,
		 int *exit_top);
