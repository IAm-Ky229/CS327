#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "file_parsing.h"

using String = std::string;
using ifstream = std::ifstream;

void file_reader::read_in_file(String file_to_open,
			       std::vector<experience> &exp,
			       std::vector<moves> &mv,
			       std::vector<pokemon> &pkmn,
			       std::vector<pokemon_moves> &pkmn_mv,
			       std::vector<pokemon_species> &pkmn_spc,
			       std::vector<type_names> &typ_nm,
			       std::vector<pokemon_stats> &pkmn_st,
			       std::vector<pokemon_types> &pkmn_typ) {

  ifstream my_input_file;
  String home_env(getenv("HOME"));
  
  my_input_file.open(file_to_open);

  // Check local share directory
  if(my_input_file.is_open()) {

    int num_lines;
    num_lines = determine_num_lines(my_input_file);

    if(file_to_open.compare("share/cs327/experience.csv") == 0) {
      read_lines_experience(my_input_file, num_lines, exp);
    }
    else if(file_to_open.compare("share/cs327/moves.csv") == 0) {
      read_lines_moves(my_input_file, num_lines, mv);
    }
    else if(file_to_open.compare("share/cs327/pokemon.csv") == 0) {
      read_lines_pokemon(my_input_file, num_lines, pkmn);
    }
    else if(file_to_open.compare("share/cs327/pokemon_moves.csv") == 0) {
      read_lines_pokemon_moves(my_input_file, num_lines, pkmn_mv);
    }
    else if(file_to_open.compare("share/cs327/pokemon_species.csv") == 0) {
      read_lines_pokemon_species(my_input_file, num_lines, pkmn_spc);
    }
    else if(file_to_open.compare("share/cs327/type_names.csv") == 0) {
      read_lines_type_names(my_input_file, num_lines, typ_nm);
    }
    else if(file_to_open.compare("share/cs327/pokemon_stats.csv") == 0) {
      read_lines_pokemon_stats(my_input_file, num_lines, pkmn_st);
    }
    else if(file_to_open.compare("share/cs327/pokemon_types.csv") == 0) {
      read_lines_pokemon_types(my_input_file, num_lines, pkmn_typ);
    }
    
  }

  // Check home ENV
  if(my_input_file.is_open()) {

    int num_lines;
    num_lines = determine_num_lines(my_input_file);
    
    if(file_to_open.compare(home_env + "/.poke327/experience.csv") == 0) {
      read_lines_experience(my_input_file, num_lines, exp);
    }
    else if(file_to_open.compare(home_env + "/.poke327/moves.csv") == 0) {
      read_lines_moves(my_input_file, num_lines, mv);
    }
    else if(file_to_open.compare(home_env + "/.poke327/pokemon.csv") == 0) {
      read_lines_pokemon(my_input_file, num_lines, pkmn);
    }
    else if(file_to_open.compare(home_env + "/.poke327/pokemon_moves.csv") == 0) {
      read_lines_pokemon_moves(my_input_file, num_lines, pkmn_mv);
    }
    else if(file_to_open.compare(home_env + "/.poke327/pokemon_species.csv") == 0) {
      read_lines_pokemon_species(my_input_file, num_lines, pkmn_spc);
    }
    else if(file_to_open.compare(home_env + "/.poke327/type_names.csv") == 0) {
      read_lines_type_names(my_input_file, num_lines, typ_nm);
    }
    else if(file_to_open.compare(home_env + "/.poke327/pokemon_stats.csv") == 0) {
      read_lines_pokemon_stats(my_input_file, num_lines, pkmn_st);
    }
    else if(file_to_open.compare(home_env + "/.poke327/pokemon_types.csv") == 0) {
      read_lines_pokemon_types(my_input_file, num_lines, pkmn_typ);
    }

  }
  else {
    std::cout << "No file found\n";
  }

  // I only did those two, the last one was optional

  my_input_file.close();
  
}

void file_reader::read_lines_experience(ifstream &input_file, int num_lines, std::vector<experience> &exp)  {

  // Skip first line
  String line;
  std::getline(input_file, line);
  
  for(int i = 0; i < num_lines; i++) {
    std::getline(input_file, line);
    std::istringstream lineStream(line);

    String read1;
    String read2;
    String read3;

    std::getline(lineStream, read1, ',');
    std::getline(lineStream, read2, ',');
    std::getline(lineStream, read3, ',');

    check_if_null(&read1);
    check_if_null(&read2);
    check_if_null(&read3);

    experience temp;
    
    temp.growth_rate_id = read1;
    temp.level = read2;
    temp.experience = read3;

    exp.push_back(temp);

  }
}

void file_reader::read_lines_moves(ifstream &input_file, int num_lines, std::vector<moves> &mv) {

  // Skip first line
  String line;
  std::getline(input_file, line);
  
  for(int i = 0; i < num_lines; i++) {
    std::getline(input_file, line);
    std::istringstream lineStream(line);

    String read1;
    String read2;
    String read3;
    String read4;
    String read5;
    String read6;
    String read7;
    String read8;
    String read9;
    String read10;
    String read11;
    String read12;
    String read13;
    String read14;
    String read15;

    std::getline(lineStream, read1, ',');
    std::getline(lineStream, read2, ',');
    std::getline(lineStream, read3, ',');
    std::getline(lineStream, read4, ',');
    std::getline(lineStream, read5, ',');
    std::getline(lineStream, read6, ',');
    std::getline(lineStream, read7, ',');
    std::getline(lineStream, read8, ',');
    std::getline(lineStream, read9, ',');
    std::getline(lineStream, read10, ',');
    std::getline(lineStream, read11, ',');
    std::getline(lineStream, read12, ',');
    std::getline(lineStream, read13, ',');
    std::getline(lineStream, read14, ',');
    std::getline(lineStream, read15, ',');

    check_if_null(&read1);
    check_if_null(&read2);
    check_if_null(&read3);
    check_if_null(&read4);
    check_if_null(&read5);
    check_if_null(&read6);
    check_if_null(&read7);
    check_if_null(&read8);
    check_if_null(&read9);
    check_if_null(&read10);
    check_if_null(&read11);
    check_if_null(&read12);
    check_if_null(&read13);
    check_if_null(&read14);
    check_if_null(&read15);

    moves temp;
    
    temp.id = read1;
    temp.identifier = read2;
    temp.generation_id = read3;
    temp.type_id = read4;
    temp.power = read5;
    temp.pp = read6;
    temp.accuracy = read7;
    temp.priority = read8;
    temp.target_id = read9;
    temp.damage_class_id = read10;
    temp.effect_id = read11;
    temp.effect_chance = read12;
    temp.contest_type_id = read13;
    temp.contest_effect_id = read14;
    temp.super_contest_effect_id = read15;

    mv.push_back(temp);
    
  }
}

void file_reader::read_lines_pokemon(ifstream &input_file, int num_lines, std::vector<pokemon> &pkmn) {

  // Skip first line
  String line;
  std::getline(input_file, line);
  
  for(int i = 0; i < num_lines; i++) {
    std::getline(input_file, line);
    std::istringstream lineStream(line);

    String read1;
    String read2;
    String read3;
    String read4;
    String read5;
    String read6;
    String read7;
    String read8;

    std::getline(lineStream, read1, ',');
    std::getline(lineStream, read2, ',');
    std::getline(lineStream, read3, ',');
    std::getline(lineStream, read4, ',');
    std::getline(lineStream, read5, ',');
    std::getline(lineStream, read6, ',');
    std::getline(lineStream, read7, ',');
    std::getline(lineStream, read8, ',');

    check_if_null(&read1);
    check_if_null(&read2);
    check_if_null(&read3);
    check_if_null(&read4);
    check_if_null(&read5);
    check_if_null(&read6);
    check_if_null(&read7);
    check_if_null(&read8);

    pokemon temp;
    
    temp.id = read1;
    temp.identifier = read2;
    temp.species_id = read3;
    temp.height = read4;
    temp.weight = read5;
    temp.base_experience = read6;
    temp.order = read7;
    temp.is_default = read8;

    pkmn.push_back(temp);
  }
}

void file_reader::read_lines_pokemon_moves(ifstream &input_file, int num_lines, std::vector<pokemon_moves> &pkmn_mv) {
  
  // Skip first line
  String line;
  std::getline(input_file, line);
  
  for(int i = 0; i < num_lines; i++) {
    
    std::getline(input_file, line);
    std::istringstream lineStream(line);

    String read1;
    String read2;
    String read3;
    String read4;
    String read5;
    String read6;
    
    std::getline(lineStream, read1, ',');
    std::getline(lineStream, read2, ',');
    std::getline(lineStream, read3, ',');
    std::getline(lineStream, read4, ',');
    std::getline(lineStream, read5, ',');
    std::getline(lineStream, read6, ',');
    
    check_if_null(&read1);
    check_if_null(&read2);
    check_if_null(&read3);
    check_if_null(&read4);
    check_if_null(&read5);
    check_if_null(&read6);

    pokemon_moves temp;
    
    temp.pokemon_id = read1;
    temp.version_group_id = read2;
    temp.move_id = read3;
    temp.pokemon_move_method_id = read4;
    temp.level = read5;
    temp.order = read6;
    
    pkmn_mv.push_back(temp);
    
  }
}

void file_reader::read_lines_pokemon_species(ifstream &input_file, int num_lines, std::vector<pokemon_species> &pkmn_spc) {
 
  // Skip first line
  String line;
  std::getline(input_file, line);
  
  for(int i = 0; i < num_lines; i++) {
    std::getline(input_file, line);
    std::istringstream lineStream(line);

    String read1;
    String read2;
    String read3;
    String read4;
    String read5;
    String read6;
    String read7;
    String read8;
    String read9;
    String read10;
    String read11;
    String read12;
    String read13;
    String read14;
    String read15;
    String read16;
    String read17;
    String read18;
    String read19;
    String read20;

    std::getline(lineStream, read1, ',');
    std::getline(lineStream, read2, ',');
    std::getline(lineStream, read3, ',');
    std::getline(lineStream, read4, ',');
    std::getline(lineStream, read5, ',');
    std::getline(lineStream, read6, ',');
    std::getline(lineStream, read7, ',');
    std::getline(lineStream, read8, ',');
    std::getline(lineStream, read9, ',');
    std::getline(lineStream, read10, ',');
    std::getline(lineStream, read11, ',');
    std::getline(lineStream, read12, ',');
    std::getline(lineStream, read13, ',');
    std::getline(lineStream, read14, ',');
    std::getline(lineStream, read15, ',');
    std::getline(lineStream, read16, ',');
    std::getline(lineStream, read17, ',');
    std::getline(lineStream, read18, ',');
    std::getline(lineStream, read19, ',');
    std::getline(lineStream, read20, ',');

    check_if_null(&read1);
    check_if_null(&read2);
    check_if_null(&read3);
    check_if_null(&read4);
    check_if_null(&read5);
    check_if_null(&read6);
    check_if_null(&read7);
    check_if_null(&read8);
    check_if_null(&read9);
    check_if_null(&read10);
    check_if_null(&read11);
    check_if_null(&read12);
    check_if_null(&read13);
    check_if_null(&read14);
    check_if_null(&read15);
    check_if_null(&read16);
    check_if_null(&read17);
    check_if_null(&read18);
    check_if_null(&read19);
    check_if_null(&read20);

    pokemon_species temp;
    
    temp.id = read1;
    temp.identifier = read2;
    temp.generation_id = read3;
    temp.evolves_from_species_id = read4;
    temp.evolution_chain_id = read5;
    temp.color_id = read6;
    temp.shape_id = read7;
    temp.habitat_id = read8;
    temp.gender_rate = read9;
    temp.capture_rate = read10;
    temp.base_happiness = read11;
    temp.is_baby = read12;
    temp.hatch_counter = read13;
    temp.has_gender_differences = read14;
    temp.growth_rate_id = read15;
    temp.forms_switchable = read16;
    temp.is_legendary = read17;
    temp.is_mythical = read18;
    temp.order = read19;
    temp.conquest_order = read20;

    pkmn_spc.push_back(temp);
    
  }
}

void file_reader::read_lines_type_names(ifstream &input_file, int num_lines, std::vector<type_names> &typ_nm) {

  // Skip first line
  String line;
  std::getline(input_file, line);
  
  for(int i = 0; i < num_lines; i++) {
    std::getline(input_file, line);
    std::istringstream lineStream(line);

    String read1;
    String read2;
    String read3;

    std::getline(lineStream, read1, ',');
    std::getline(lineStream, read2, ',');
    std::getline(lineStream, read3, ',');

    check_if_null(&read1);
    check_if_null(&read2);
    check_if_null(&read3);

    type_names temp;

    temp.type_id = read1;
    temp.local_language_id = read2;
    temp.name = read3;

    typ_nm.push_back(temp);
  }
}

void file_reader::read_lines_pokemon_stats(ifstream &input_file, int num_lines, std::vector<pokemon_stats> &pkmn_st) {

  // Skip first line
  String line;
  std::getline(input_file, line);
  
  for(int i = 0; i < num_lines; i++) {
    std::getline(input_file, line);
    std::istringstream lineStream(line);

    String read1;
    String read2;
    String read3;
    String read4;

    std::getline(lineStream, read1, ',');
    std::getline(lineStream, read2, ',');
    std::getline(lineStream, read3, ',');
    std::getline(lineStream, read4, ',');

    check_if_null(&read1);
    check_if_null(&read2);
    check_if_null(&read3);
    check_if_null(&read4);

    pokemon_stats temp;

    temp.pokemon_id = read1;
    temp.stat_id = read2;
    temp.base_stat = read3;
    temp.effort = read4;

    pkmn_st.push_back(temp);
  }
}

void file_reader::read_lines_pokemon_types(ifstream &input_file, int num_lines, std::vector<pokemon_types> &pkmn_typ) {

  // Skip first line
  String line;
  std::getline(input_file, line);
  
  for(int i = 0; i < num_lines; i++) {
    std::getline(input_file, line);
    std::istringstream lineStream(line);

    String read1;
    String read2;
    String read3;

    std::getline(lineStream, read1, ',');
    std::getline(lineStream, read2, ',');
    std::getline(lineStream, read3, ',');

    check_if_null(&read1);
    check_if_null(&read2);
    check_if_null(&read3);

    pokemon_types temp;

    temp.pokemon_id = read1;
    temp.type_id = read2;
    temp.slot = read3;

    pkmn_typ.push_back(temp);
  }
}

void file_reader::check_if_null(String *value) {
  if(*value == "") {
    *value = "-1";
  }
}

int file_reader::determine_num_lines(ifstream &input_file) {

  int count = 0;
  String line;

  std::getline(input_file, line);
  std::istringstream lineStream(line);

  while(!input_file.eof()) {
    std::getline(input_file, line);
    std::istringstream lineStream(line);
    count++;
  }

  input_file.clear();
  input_file.seekg(0);

  // We counted 1 more line than there actually is
  return count - 1;
}


