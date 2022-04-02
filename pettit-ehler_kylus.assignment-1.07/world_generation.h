#define WORLD_X_LENGTH 400
#define WORLD_Y_LENGTH 400
#define WORLD_X_START 199
#define WORLD_Y_START 199

#include "heap.h"
#include "file_parsing.h"

using String = std::string;

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
  void move_PC(PC_char *PC, generatedMap *m, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv);

  // Placeholder for NPC pokemon battle
  void engage_battle();
};

class displayList {

public:
  
  // Update the list of characters printed
  void update_list(generatedMap *m, NPC_char *list_copy, int window, int size);
  
};

// I actually made a proper class for once
class in_game_pokemon {

private:
  
  int id;
  int level;
  int HP;
  int attack;
  int defense;
  int special_attack;
  int special_defense;
  int speed;
  int accuracy;
  int evasion;

  int HP_iv;
  int attack_iv;
  int defense_iv;
  int special_attack_iv;
  int special_defense_iv;
  int speed_iv;

  String move1;
  String move2;
  int move_id_1;
  int move_id_2;

  String name;

public:

  in_game_pokemon() {
    id = 0;
    level = 0;
    HP = 0;
    attack = 0;
    defense = 0;
    special_attack = 0;
    special_defense = 0;
    speed = 0;
    accuracy = 0;
    evasion = 0;

    HP_iv = 0;
    attack_iv = 0;
    defense_iv = 0;
    special_attack_iv = 0;
    special_defense_iv = 0;
    speed_iv = 0;

    move1 = "UNASSIGNED";
    move2 = "UNASSIGNED";

    move_id_1 = -1;
    move_id_2 = -1;

    name = "UNASSIGNED";
  }

  void set_id(int val) { id = val; }
  void set_level(int val) { level = val; }
  void set_HP(int val) { HP = val; }
  void set_attack(int val) { attack = val; }
  void set_defense(int val) { defense = val; }
  void set_special_attack(int val) { special_attack = val; }
  void set_special_defense(int val) { special_defense = val; }
  void set_speed(int val) { speed = val; }
  void set_accuracy(int val) { accuracy = val; }
  void set_evasion(int val) { evasion = val; }
  void set_HP_iv(int val) { HP_iv = val; }
  void set_attack_iv(int val) { attack_iv = val; }
  void set_defense_iv(int val) { defense_iv = val; }
  void set_special_attack_iv(int val) { special_attack_iv = val; }
  void set_special_defense_iv(int val) { special_defense_iv = val; }
  void set_speed_iv(int val) { speed_iv = val; }
  void set_move_1(String val) { move1 = val; }
  void set_move_2(String val) { move2 = val; }
  void set_move_id_1(int val) { move_id_1 = val; }
  void set_move_id_2(int val) { move_id_2 = val; }
  void set_name(String val) { name = val; }

  int get_id() { return id; }
  int get_level() { return level; }
  int get_HP() { return HP; }
  int get_attack() { return attack; }
  int get_defense() { return defense; }
  int get_special_attack() { return special_attack; }
  int get_special_defense() { return special_defense; }
  int get_speed() { return speed; }
  int get_accuracy() { return accuracy; }
  int get_evasion() { return evasion; }
  int get_HP_iv() { return HP_iv; }
  int get_attack_iv() { return attack_iv; }
  int get_defense_iv() { return defense_iv; }
  int get_special_attack_iv() { return special_attack_iv; }
  int get_special_defense_iv() { return special_defense_iv; }
  int get_speed_iv() { return speed_iv; }
  String get_move_1() { return move1; }
  String get_move_2() { return move2; }
  int get_move_id_1() { return move_id_1; }
  int get_move_id_2() { return move_id_2; }
  String get_name() { return name; }
  
};

class wild_pokemon_battle {
  
public:

  int determine_battle();
  void engage_battle_wild(std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv);
  in_game_pokemon generate_pokemon(std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv);
  void get_pokemon_stats(in_game_pokemon &pkmn, std::vector<pokemon_stats> pkmn_st);
  void get_pokemon_moves(in_game_pokemon &pkmn, std::vector<pokemon_moves> pkmn_mv);
  void resolve_pokemon_move_names(in_game_pokemon &pkmn, std::vector<moves> mv);
  void choose_random_pokemon(in_game_pokemon &pkmn, std::vector<pokemon> pkmn_list);
  void assign_ivs(in_game_pokemon &pkmn);
  int generate_HP_lv_up(int base_HP, int HP_iv, int level);
  int generate_otherstat_lv_up(int base_stat, int base_iv, int level);
  void level_up(in_game_pokemon &pkmn);
  
};


