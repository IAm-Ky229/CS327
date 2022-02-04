
// Place the bordering boulders within the 2-D array
void place_border_boulders();

// Place the 2 clearings within the 2-D array
void place_clearings();

// Place the 2 tall grass regions within the 2-D array
void place_tall_grass();

// Place the pokemart and pokecenter within the 2-D array
void place_buildings();

// Place the 4 exits on the edges of the 2-D array
void place_exits();

// Place the paths that connect the 4 edges
void place_paths();

// Iterate over the entire 2-D array, and print out what each element should be
void print_map();

// Place a pokecenter in the 2-D array
void place_pokecenter(int selected_column);

// Place a pokemart in the 2-D array
void place_pokemart(int selected_row);

// Finalize any spaces that were left uninitialized
void fill_blank_space();

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
	      border_exit
};

typedef struct generated_map {

// 2-D array that will be used to construct the GUI
enum terrain generate_map[HORIZONTAL][VERTICAL];

// Remember where the left exit is
int exit_left;
// Remember when the top exit is
int exit_top;
// Remember where the right exit is
int exit_right;
// Remember where the bottom exit is
int exit_bottom;
  
} generated_map_t;

// Struct to store individual maps
generated_map_t map_data;

