#include <string>
#include <fstream>
#include <vector>

using String = std::string;
using ifstream = std::ifstream;

class experience {

 private:

 public:

  String growth_rate_id;
  String level;
  String experience;
  
};


class moves {

 private:

 public:

  String id;
  String identifier;
  String generation_id;
  String type_id;
  String power;
  String pp;
  String accuracy;
  String priority;
  String target_id;
  String damage_class_id;
  String effect_id;
  String effect_chance;
  String contest_type_id;
  String contest_effect_id;
  String super_contest_effect_id;
  
};

class pokemon {

 private:

 public:

  String id;
  String identifier;
  String species_id;
  String height;
  String weight;
  String base_experience;
  String order;
  String is_default;
  
};

class pokemon_moves {

 private:

 public:

  String pokemon_id;
  String version_group_id;
  String move_id;
  String pokemon_move_method_id;
  String level;
  String order;
  
};

class pokemon_species {

 private:

 public:

  String id;
  String identifier;
  String generation_id;
  String evolves_from_species_id;
  String evolution_chain_id;
  String color_id;
  String shape_id;
  String habitat_id;
  String gender_rate;
  String capture_rate;
  String base_happiness;
  String is_baby;
  String hatch_counter;
  String has_gender_differences;
  String growth_rate_id;
  String forms_switchable;
  String is_legendary;
  String is_mythical;
  String order;
  String conquest_order;
  
};

class type_names {

private:

public:

  String type_id;
  String local_language_id;
  String name;
  
};

class pokemon_stats {
private:

public:

  String pokemon_id;
  String stat_id;
  String base_stat;
  String effort;
};

class file_reader {

 public:

  void read_in_file(String file_to_open,
		    std::vector<experience> &exp,
		    std::vector<moves> &mv,
		    std::vector<pokemon> &pkmn,
		    std::vector<pokemon_moves> &pkmn_mv,
		    std::vector<pokemon_species> &pkmn_spc,
		    std::vector<type_names> &typ_nm,
		    std::vector<pokemon_stats> &pkmn_st);
  void read_lines_experience(ifstream &input_file, int num_lines, std::vector<experience> &exp);
  void read_lines_moves(ifstream &input_file, int num_lines, std::vector<moves> &mv);
  void read_lines_pokemon(ifstream &input_file, int num_lines, std::vector<pokemon> &pkmn);
  void read_lines_pokemon_moves(ifstream &input_file, int num_lines, std::vector<pokemon_moves> &pkmn_mv);
  void read_lines_pokemon_species(ifstream &input_file, int num_lines, std::vector<pokemon_species> &pkmn_spc);
  void read_lines_type_names(ifstream &input_file, int num_lines, std::vector<type_names> &typ_nm);
  void read_lines_pokemon_stats(ifstream &input_file, int num_lines, std::vector<pokemon_stats> &pkmn_st);
  void check_if_null(String *value);
  void format_for_printing(String value, String *to_print);
  int determine_num_lines(ifstream &input_file);
  
};
