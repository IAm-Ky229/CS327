#include "file_parsing.h"

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

  String shiny;
  String gender;

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

    shiny = "Not shiny";
    gender = "UNASSIGNED";
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
  void set_shiny(String val) { shiny = val; }
  void set_gender(String val) { gender = val; }

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
  String get_shiny() { return shiny; }
  String get_gender() { return gender; }
  
};

class wild_pokemon_battle {
  
public:

  int determine_battle();
  void engage_battle_wild(std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, int manhattan_x, int manhattan_y);
  in_game_pokemon generate_pokemon(std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, int manhattan_x, int manhattan_y);
  void get_pokemon_stats(in_game_pokemon &pkmn, std::vector<pokemon_stats> pkmn_st);
  void get_pokemon_moves(in_game_pokemon &pkmn, std::vector<pokemon_moves> pkmn_mv);
  void resolve_pokemon_move_names(in_game_pokemon &pkmn, std::vector<moves> mv);
  void choose_random_pokemon(in_game_pokemon &pkmn, std::vector<pokemon> pkmn_list);
  void assign_ivs(in_game_pokemon &pkmn);
  int generate_HP_lv_up(int base_HP, int HP_iv, int level);
  int generate_otherstat_lv_up(int base_stat, int base_iv, int level);
  void level_up(in_game_pokemon &pkmn, int manhattan_x, int manhattan_y);
  
};
