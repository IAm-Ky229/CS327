#include "file_parsing.h"
#include <cstdio> 
#include <algorithm>
#include <vector>

#define t_normal 1
#define t_fighting 2
#define t_flying 3
#define t_poison 4
#define t_ground 5
#define t_rock 6
#define t_bug 7
#define t_ghost 8
#define t_steel 9
#define t_fire 10
#define t_water 11
#define t_grass 12
#define t_electric 13
#define t_psychic 14
#define t_ice 15
#define t_dragon 16
#define t_dark 17
#define t_fairy 18

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
  int base_speed;
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
  std::vector<int> type_ids;
  int current_HP;

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
    base_speed = 0;
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
    current_HP = 0;

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
  void set_base_speed(int val) { base_speed = val; }
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
  void add_type(int val) { type_ids.push_back(val); }
  void set_curr_HP(int val) { current_HP = val; }

  int get_id() { return id; }
  int get_level() { return level; }
  int get_HP() { return HP; }
  int get_attack() { return attack; }
  int get_defense() { return defense; }
  int get_special_attack() { return special_attack; }
  int get_special_defense() { return special_defense; }
  int get_base_speed() { return base_speed; }
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
  std::vector<int> get_type_ids() { return type_ids; }
  int get_curr_HP() { return current_HP; }
  
};

class item {

private:

  String item_name;
  int item_ID;
  int item_UID;

public:

  void set_item_name(String val) { item_name = val; }
  void set_item_ID(int val) { item_ID = val; }
  void set_UID(int val) { item_UID = val; }

  String get_item_name() { return item_name; }
  int get_item_ID() { return item_ID; }
  int get_UID() { return item_UID; }

  bool operator==(const item &i) const;
  
};

class PC_state {

private:

  std::vector<in_game_pokemon> PC_pokemon;
  std::vector<item> bag;
  int money;
  int PC_fainted_pokemon;
  int item_UID_counter;

public:

  PC_state() {
    PC_fainted_pokemon = 0;
    money = 1000;
    item_UID_counter = 25;
  }
  
  void addPokemon(in_game_pokemon p) { PC_pokemon.push_back(p); }
  std::vector<in_game_pokemon> &getPokemon() { return PC_pokemon; }

  void addItem(item i) { bag.push_back(i); }
  void removeItem(item i) { bag.erase(std::remove(bag.begin(), bag.end(), i), bag.end()); }
  std::vector<item> &get_items() { return bag; }

  void increment_PC_fainted() { PC_fainted_pokemon++; }
  void decrement_PC_fainted() { PC_fainted_pokemon--; }
  int get_PC_fainted() { return PC_fainted_pokemon; }

  void increment_money(int val) { money += val; }
  void decrement_money(int val) { money -= val; }
  int get_money() { return money; }

  void increment_UID_counter() { item_UID_counter += 1; }
  int get_UID_counter() { return item_UID_counter; }
  
};


class battle {
  
public:

  int determine_battle();
  void engage_battle_wild(PC_state &PC_s, std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, std::vector<pokemon_types> pkmn_typ, int manhattan_x, int manhattan_y);
  in_game_pokemon generate_pokemon(std::vector<pokemon> pkmn_list, std::vector<pokemon_stats> pkmn_st, std::vector<pokemon_moves> pkmn_mv, std::vector<moves> mv, std::vector<pokemon_types> pkmn_typ, int manhattan_x, int manhattan_y);
  void get_pokemon_stats(in_game_pokemon &pkmn, std::vector<pokemon_stats> pkmn_st);
  void get_pokemon_moves(in_game_pokemon &pkmn, std::vector<pokemon_moves> pkmn_mv);
  void resolve_pokemon_move_names(in_game_pokemon &pkmn, std::vector<moves> mv);
  void get_types(in_game_pokemon &pkmn, std::vector<pokemon_types> pkmn_typ);
  void choose_random_pokemon(in_game_pokemon &pkmn, std::vector<pokemon> pkmn_list);
  void assign_ivs(in_game_pokemon &pkmn);
  int generate_HP_lv_up(int base_HP, int HP_iv, int level);
  int generate_otherstat_lv_up(int base_stat, int base_iv, int level);
  void level_up(in_game_pokemon &pkmn, int manhattan_x, int manhattan_y);
  
  int enter_bag(PC_state &PC_s, in_game_pokemon opposing_pokemon, int pokeballs_usable, int *PC_fainted);
  int view_pokemon_in_battle(PC_state &PC_s, int *PC_active_pokemon, int must_choose);
  int process_move(PC_state &PC_s, int PC_active_pokemon);
  void process_attacks(PC_state &PC_s, in_game_pokemon &NPC_s, std::vector<moves> mv, int PC_move_id, int PC_active_pokemon);
  int calculate_damage(in_game_pokemon attacking, in_game_pokemon defending, int move_id, std::vector<moves> mv);
  void print_bag(std::vector<item> bag_copy, int in_battle, int window);
  void print_pokemon(PC_state &PC_s);
  void use_potion(PC_state &PC_s, int pokemon_rec, item itm);
  void use_revive(PC_state &PC_s, int pokemon_rec, item itm, int *PC_fainted);
  int use_pokeball(PC_state &PC_s, in_game_pokemon wild_pkmn, item itm);
  int run(PC_state &PC_s, int PC_active_pokemon, in_game_pokemon opposing_pokemon, int *attempts_to_run);
  double determine_type_advantage(in_game_pokemon defending, int move_id, std::vector<moves> mv);
  double determine_type_effectiveness(int attacking_move_type_id, int defending_type);
  
};
