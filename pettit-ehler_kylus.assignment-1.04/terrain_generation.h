#include <stdint.h>

// Max length of the GUI
#define HORIZONTAL 80

// Max height of the GUI
#define VERTICAL 21

// Max height of generated clearings / tall grass
#define MAXHEIGHT 12

// Max length of generated clearings / tall grass
#define MAXLENGTH 12

// Min height of generated clearings / tall grass
#define MINHEIGHT 3

// Min length of generated clearings / tall grass
#define MINLENGTH 3

// Enumerations that will populate our 2-D array
typedef enum __attribute__ ((__packed__)) terrain {
	      nothing,
	      pokemon_center,
	      path,
	      pokemon_mart,
	      tall_grass,
	      clearing,
	      boulder,
	      tree,
	      border_exit
} terrain_type_t;

typedef enum char_type {
			hiker,
			rival,
			pacer,
			wanderer,
			stationary,
			random_walker,
			PC
} char_type_t;

typedef enum direction {
			up,
			down,
			right,
			left
} direction_t;

typedef struct character {
  terrain_type_t terrain_type;
  char_type_t player_type;
  direction_t direction;
  int cost_to_move;
  int x_pos;
  int y_pos;
  int next_x;
  int next_y;
} character_t;

typedef struct generated_map {

  // 2-D array that will be used to construct the GUI
  enum terrain generate_map[HORIZONTAL][VERTICAL];

  // Used to calculate Dijkstra distances
  uint8_t height[VERTICAL][HORIZONTAL];

  // Store positions of characters in maps
  character_t *character_positions[HORIZONTAL][VERTICAL];

  // Remember where the left exit is
  int exit_left;
  // Remember when the top exit is
  int exit_top;
  // Remember where the right exit is
  int exit_right;
  // Remember where the bottom exit is
  int exit_bottom;
  
} generated_map_t;

// Place the bordering boulders within the 2-D array
void place_border_boulders(generated_map_t *map_data);

// Place the 2 clearings within the 2-D array
void place_clearings(generated_map_t *map_data);

// Place the 2 tall grass regions within the 2-D array
void place_tall_grass(generated_map_t *map_data);

// Place the pokemart and pokecenter within the 2-D array
void place_buildings(generated_map_t *map_data, int building_spawn_rate);

// Place the 4 exits on the edges of the 2-D array
void place_exits(generated_map_t *map_data,
		 int exit_bottom,
		 int exit_right,
		 int exit_left,
		 int exit_top);

// Place the paths that connect the 4 edges
void place_paths(generated_map_t *map_data);

// Iterate over the entire 2-D array, and print out what each element should be
void print_map(generated_map_t *map_data);

// Place a pokecenter in the 2-D array
void place_pokecenter(int selected_column, generated_map_t *map_data);

// Place a pokemart in the 2-D array
void place_pokemart(int selected_row, generated_map_t *map_data);

// Finalize any spaces that were left uninitialized
void fill_blank_space(generated_map_t *map_data);

