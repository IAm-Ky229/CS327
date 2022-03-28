#include <stdint.h>

#include "heap.h"


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
enum terrain {
	      nothing,
	      pokemon_center,
	      path,
	      pokemon_mart,
	      tall_grass,
	      clearing,
	      boulder,
	      tree,
	      border_exit,
	      stationary_occupied
};

enum char_type {
		hiker,
		rival,
		pacer,
		wanderer,
		stationary,
		random_walker,
		PC
};

enum direction {
		up,
		down,
		right,
		left
};

class characterData {
  
 private:

 public:

  terrain terrain_type;
  char_type player_type;
  direction cur_direction;
  int cost_to_move;
  int x_pos;
  int y_pos;
  int next_x;
  int next_y;
  int battled;
  
};

class generatedMap {

 private:

 public:

  // Store positions of characters in maps
  characterData *character_positions[HORIZONTAL][VERTICAL];

  // 2-D array that will be used to construct the GUI
  enum terrain generate_map[HORIZONTAL][VERTICAL];

  // Store the player character's position
  int PC_position_x;

  // Store the player character's position
  int PC_position_y;

  // Store the game time of this map
  int game_time;

  // Keep track of characters in the map
  heap_t characters_to_move;

  // Remember where the left exit is
  int exit_left;
  // Remember when the top exit is
  int exit_top;
  // Remember where the right exit is
  int exit_right;
  // Remember where the bottom exit is
  int exit_bottom;

  // Place the bordering boulders within the 2-D array
  void place_border_boulders(generatedMap *map_data);

  // Place the 2 clearings within the 2-D array
  void place_clearings(generatedMap *map_data);

  // Place the 2 tall grass regions within the 2-D array
  void place_tall_grass(generatedMap *map_data);

  // Place the pokemart and pokecenter within the 2-D array
  void place_buildings(generatedMap *map_data, int building_spawn_rate);

  // Place the 4 exits on the edges of the 2-D array
  void place_exits(generatedMap *map_data,
		   int exit_bottom,
		   int exit_right,
		   int exit_left,
		   int exit_top);

  // Place the paths that connect the 4 edges
  void place_paths(generatedMap *map_data);

  // Iterate over the entire 2-D array, and print out what each element should be
  void print_map(generatedMap *map_data);

  // Place a pokecenter in the 2-D array
  void place_pokecenter(int selected_column, generatedMap *map_data);

  // Place a pokemart in the 2-D array
  void place_pokemart(int selected_row, generatedMap *map_data);

  // Finalize any spaces that were left uninitialized
  void fill_blank_space(generatedMap *map_data);
  
};

