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

class characterLogic {

private:

public:
  // Place characters into the map
  void place_characters(generatedMap *m, heap_t *h, movementCosts distance_hiker[HORIZONTAL][VERTICAL], movementCosts distance_rival[HORIZONTAL][VERTICAL], int numtrainers);

  // Function for moving a pacer character
  void move_pacer(generatedMap *m, NPC_char *pacer_to_move, heap_t *h, PC_state &PC_s, int manhattan_x, int manhattan_y, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv);

  // Function for moving a character via a gradient
  void move_via_shortest_path(generatedMap *m, movementCosts dijkstra[HORIZONTAL][VERTICAL], NPC_char *character_to_move, heap_t * h, PC_state &PC_s, int manhattan_x, int manhattan_y, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv);

  // Move the specified character up
  void move_up(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

  // Move the specified character down
  void move_down(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

  // Move the specified character right
  void move_right(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

  // Move the specified character left
  void move_left(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

  // Move wanderer
  void move_wanderer(generatedMap *m, NPC_char *wanderer_to_move, heap_t *h, PC_state &PC_s, int manhattan_x, int manhattan_y, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv);

  // Move the specified character up
  void move_up_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

  // Move the specified character up
  void move_down_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

  // Move the specified character up
  void move_right_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

  // Move the specified character up
  void move_left_random(generatedMap *m, heap_t *h, NPC_char *character_to_move, int current_x, int current_y);

  // Move random walker
  void move_random_walker(generatedMap *m, NPC_char *wanderer_to_move, heap_t *h, PC_state &PC_s, int manhattan_x, int manhattan_y, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv);

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
  void move_PC(PC_char *PC, generatedMap *m, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, int manhattan_x, int manhattan_y, PC_state &PC_s); 

  // Placeholder for NPC pokemon battle
  void engage_battle(PC_state &PC_s, int manhattan_x, int manhattan_y, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv);
};
