#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "file_parsing.h"

using String = std::string;
using ifstream = std::ifstream;

void file_reader::read_in_file(String file_to_open) {

  ifstream my_input_file;
  String home_env(getenv("HOME"));

  std::cout << "Before: " << file_to_open << "\n";
  
  my_input_file.open(file_to_open);

  // Check local share directory
  if(my_input_file.is_open()) {

    int num_lines;
    num_lines = determine_num_lines(my_input_file);

    if(file_to_open.compare("share/cs327/experience.csv") == 0) {
      read_lines_experience(my_input_file, num_lines);
    }
    else if(file_to_open.compare("share/cs327/moves.csv") == 0) {
      read_lines_moves(my_input_file, num_lines);
    }
    else if(file_to_open.compare("share/cs327/pokemon.csv") == 0) {
      read_lines_pokemon(my_input_file, num_lines);
    }
    else if(file_to_open.compare("share/cs327/pokemon_moves.csv") == 0) {
      read_lines_pokemon_moves(my_input_file, num_lines);
    }
    else if(file_to_open.compare("share/cs327/pokemon_species.csv") == 0) {
      read_lines_pokemon_species(my_input_file, num_lines);
    }
    else if(file_to_open.compare("share/cs327/type_names.csv") == 0) {
      read_lines_type_names(my_input_file, num_lines);
    }
    
  }

  // Check home ENV
  if(my_input_file.is_open()) {

    int num_lines;
    num_lines = determine_num_lines(my_input_file);
    
    if(file_to_open.compare(home_env + "/.poke327/experience.csv") == 0) {
      read_lines_experience(my_input_file, num_lines);
    }
    else if(file_to_open.compare(home_env + "/.poke327/moves.csv") == 0) {
      read_lines_moves(my_input_file, num_lines);
    }
    else if(file_to_open.compare(home_env + "/.poke327/pokemon.csv") == 0) {
      read_lines_pokemon(my_input_file, num_lines);
    }
    else if(file_to_open.compare(home_env + "/.poke327/pokemon_moves.csv") == 0) {
      read_lines_pokemon_moves(my_input_file, num_lines);
    }
    else if(file_to_open.compare(home_env + "/.poke327/pokemon_species.csv") == 0) {
      read_lines_pokemon_species(my_input_file, num_lines);
    }
    else if(file_to_open.compare(home_env + "/.poke327/type_names.csv") == 0) {
      read_lines_type_names(my_input_file, num_lines);
    }

  }
  else {
    std::cout << "No file found\n";
  }

  // I only did those two, the last one was optional

  my_input_file.close();
  
}

void file_reader::read_lines_experience(ifstream &input_file, int num_lines)  {
  experience exp[num_lines];

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

    exp[i].growth_rate_id = read1;
    exp[i].level = read2;
    exp[i].experience = read3;

    experience to_print;
    
    format_for_printing(exp[i].growth_rate_id, &to_print.growth_rate_id);
    format_for_printing(exp[i].level, &to_print.level);
    format_for_printing(exp[i].experience, &to_print.experience);

    std::cout << "growth_rate_id: " << to_print.growth_rate_id << ", "
	      << "level: " << to_print.level << ", "
	      << "experience: " << to_print.experience << "\n";
  }
}

void file_reader::read_lines_moves(ifstream &input_file, int num_lines) {
  moves mv[num_lines];

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

    mv[i].id = read1;
    mv[i].identifier = read2;
    mv[i].generation_id = read3;
    mv[i].type_id = read4;
    mv[i].power = read5;
    mv[i].pp = read6;
    mv[i].accuracy = read7;
    mv[i].priority = read8;
    mv[i].target_id = read9;
    mv[i].damage_class_id = read10;
    mv[i].effect_id = read11;
    mv[i].effect_chance = read12;
    mv[i].contest_type_id = read13;
    mv[i].contest_effect_id = read14;
    mv[i].super_contest_effect_id = read15;

    moves to_print;
    
    format_for_printing(mv[i].id, &to_print.id);
    format_for_printing(mv[i].identifier, &to_print.identifier);
    format_for_printing(mv[i].generation_id, &to_print.generation_id);
    format_for_printing(mv[i].type_id, &to_print.type_id);
    format_for_printing(mv[i].power, &to_print.power);
    format_for_printing(mv[i].pp, &to_print.pp);
    format_for_printing(mv[i].accuracy, &to_print.accuracy);
    format_for_printing(mv[i].priority, &to_print.priority);
    format_for_printing(mv[i].target_id, &to_print.target_id);
    format_for_printing(mv[i].damage_class_id, &to_print.damage_class_id);
    format_for_printing(mv[i].effect_id, &to_print.effect_id);
    format_for_printing(mv[i].effect_chance, &to_print.effect_chance);
    format_for_printing(mv[i].contest_type_id, &to_print.contest_type_id);
    format_for_printing(mv[i].contest_effect_id, &to_print.contest_effect_id);
    format_for_printing(mv[i].super_contest_effect_id, &to_print.super_contest_effect_id);

    std::cout << "id: " << to_print.id << ", "
	      << "identifier: " << to_print.identifier << ", "
	      << "generation_id: " << to_print.generation_id << ", "
	      << "type_id: " << to_print.type_id << ", "
	      << "power: " << to_print.power << ", "
	      << "pp: " << to_print.pp << ", "
	      << "accuracy: " << to_print.accuracy << ", "
	      << "priority: " << to_print.priority << ", "
	      << "target_id: " << to_print.target_id << ", "
	      << "damage_class_id: " << to_print.damage_class_id << ", "
	      << "effect_id: " << to_print.effect_id << ", "
	      << "effect_chance: " << to_print.effect_chance << ", "
	      << "contest_type_id: " << to_print.contest_type_id << ", "
	      << "contest_effect_id: " << to_print.contest_effect_id << ", "
	      << "super_contest_effect_id: " << to_print.super_contest_effect_id << "\n";
  }
}

void file_reader::read_lines_pokemon(ifstream &input_file, int num_lines) {
  pokemon pkmn[num_lines];

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

    pkmn[i].id = read1;
    pkmn[i].identifier = read2;
    pkmn[i].species_id = read3;
    pkmn[i].height = read4;
    pkmn[i].weight = read5;
    pkmn[i].base_experience = read6;
    pkmn[i].order = read7;
    pkmn[i].is_default = read8;

    pokemon to_print;

    format_for_printing(pkmn[i].id, &to_print.id);
    format_for_printing(pkmn[i].identifier, &to_print.identifier);
    format_for_printing(pkmn[i].species_id, &to_print.species_id);
    format_for_printing(pkmn[i].height, &to_print.height);
    format_for_printing(pkmn[i].weight, &to_print.weight);
    format_for_printing(pkmn[i].base_experience, &to_print.base_experience);
    format_for_printing(pkmn[i].order, &to_print.order);
    format_for_printing(pkmn[i].is_default, &to_print.is_default);

    std::cout << "id: " << to_print.id << ", "
	      << "identifier: " << to_print.identifier << ", "
	      << "species_id: " << to_print.species_id << ", "
	      << "height: " << to_print.height << ", "
	      << "weight: " << to_print.weight << ", "
	      << "base_experience: " << to_print.base_experience << ", "
	      << "order: " << to_print.order << ", "
	      << "is_default: " << to_print.is_default << "\n";
      }
}

void file_reader::read_lines_pokemon_moves(ifstream &input_file, int num_lines) {
  std::vector<pokemon_moves> v;
  
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
    
    v.push_back(temp);

    pokemon_moves to_print;
    
    format_for_printing(v[i].pokemon_id, &to_print.pokemon_id);
    format_for_printing(v[i].version_group_id, &to_print.version_group_id);
    format_for_printing(v[i].move_id, &to_print.move_id);
    format_for_printing(v[i].pokemon_move_method_id, &to_print.pokemon_move_method_id);
    format_for_printing(v[i].level, &to_print.level);
    format_for_printing(v[i].order, &to_print.order);
    
    std::cout << "id: " << to_print.pokemon_id << ", "
	      << "version_group_id: " << to_print.version_group_id << ", "
	      << "move_id: " << to_print.move_id << ", "
	      << "pokemon_move_method_id: " << to_print.pokemon_move_method_id << ", "
	      << "level: " << to_print.level << ", "
	      << "order: " << to_print.order << "\n";
    
  }
}

void file_reader::read_lines_pokemon_species(ifstream &input_file, int num_lines) {
  pokemon_species pkmn_spc[num_lines];
 
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

    pkmn_spc[i].id = read1;
    pkmn_spc[i].identifier = read2;
    pkmn_spc[i].generation_id = read3;
    pkmn_spc[i].evolves_from_species_id = read4;
    pkmn_spc[i].evolution_chain_id = read5;
    pkmn_spc[i].color_id = read6;
    pkmn_spc[i].shape_id = read7;
    pkmn_spc[i].habitat_id = read8;
    pkmn_spc[i].gender_rate = read9;
    pkmn_spc[i].capture_rate = read10;
    pkmn_spc[i].base_happiness = read11;
    pkmn_spc[i].is_baby = read12;
    pkmn_spc[i].hatch_counter = read13;
    pkmn_spc[i].has_gender_differences = read14;
    pkmn_spc[i].growth_rate_id = read15;
    pkmn_spc[i].forms_switchable = read16;
    pkmn_spc[i].is_legendary = read17;
    pkmn_spc[i].is_mythical = read18;
    pkmn_spc[i].order = read19;
    pkmn_spc[i].conquest_order = read20;

    pokemon_species to_print;
    
    format_for_printing(pkmn_spc[i].id, &to_print.id);
    format_for_printing(pkmn_spc[i].identifier, &to_print.identifier);
    format_for_printing(pkmn_spc[i].generation_id, &to_print.generation_id);
    format_for_printing(pkmn_spc[i].evolves_from_species_id, &to_print.evolves_from_species_id);
    format_for_printing(pkmn_spc[i].evolution_chain_id, &to_print.evolution_chain_id);
    format_for_printing(pkmn_spc[i].color_id, &to_print.color_id);
    format_for_printing(pkmn_spc[i].shape_id, &to_print.shape_id);
    format_for_printing(pkmn_spc[i].habitat_id, &to_print.habitat_id);
    format_for_printing(pkmn_spc[i].gender_rate, &to_print.gender_rate);
    format_for_printing(pkmn_spc[i].capture_rate, &to_print.capture_rate);
    format_for_printing(pkmn_spc[i].base_happiness, &to_print.base_happiness);
    format_for_printing(pkmn_spc[i].is_baby, &to_print.is_baby);
    format_for_printing(pkmn_spc[i].hatch_counter, &to_print.hatch_counter);
    format_for_printing(pkmn_spc[i].has_gender_differences, &to_print.has_gender_differences);
    format_for_printing(pkmn_spc[i].growth_rate_id, &to_print.growth_rate_id);
    format_for_printing(pkmn_spc[i].forms_switchable, &to_print.forms_switchable);
    format_for_printing(pkmn_spc[i].is_legendary, &to_print.is_legendary);
    format_for_printing(pkmn_spc[i].is_mythical, &to_print.is_mythical);
    format_for_printing(pkmn_spc[i].order, &to_print.order);
    format_for_printing(pkmn_spc[i].conquest_order, &to_print.conquest_order);
    

    std::cout << "id: " << to_print.id << ", "
	      << "identifier: " << to_print.identifier << ", "
	      << "generation_id: " << to_print.generation_id << ", "
	      << "evolves_from_species_id: " << to_print.evolves_from_species_id << ", "
	      << "evolution_chain_id: " << to_print.evolution_chain_id << ", "
	      << "color_id: " << to_print.color_id << "\n"
	      << "shape_id: " << to_print.shape_id << ", "
	      << "habitat_id: " << to_print.habitat_id << ", "
	      << "gender_rate: " << to_print.gender_rate << ", "
	      << "capture_rate: " << to_print.capture_rate << ", "
	      << "base_happiness: " << to_print.base_happiness << ", "
	      << "is_baby: " << to_print.is_baby << "\n"
	      << "hatch_counter: " << to_print.hatch_counter << ", "
	      << "has_gender_differences: " << to_print.has_gender_differences << ", "
	      << "growth_rate_id: " << to_print.growth_rate_id << ", "
	      << "forms_switchable: " << to_print.forms_switchable << ", "
	      << "is_legendary: " << to_print.is_legendary << ", "
	      << "is_mythical: " << to_print.is_mythical << "\n"
	      << "order: " << to_print.order << ", "
	      << "conquest_order: " << to_print.conquest_order << "\n";

      }
}

void file_reader::read_lines_type_names(ifstream &input_file, int num_lines) {
  type_names typ_nm[num_lines];

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

    typ_nm[i].type_id = read1;
    typ_nm[i].local_language_id = read2;
    typ_nm[i].name = read3;

    type_names to_print;
    
    format_for_printing(typ_nm[i].type_id, &to_print.type_id);
    format_for_printing(typ_nm[i].local_language_id, &to_print.local_language_id);
    format_for_printing(typ_nm[i].name, &to_print.name);

    std::cout << "type_id: " << to_print.type_id << ", "
	      << "local_language_id: " << to_print.local_language_id << ", "
	      << "name: " << to_print.name << "\n";
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

void file_reader::format_for_printing(String value, String *to_print) {
  
  if(value == "-1") {
    *to_print = "";
  } else {
    *to_print = value;
  }
 
}
