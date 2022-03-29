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

  // I only did those two, the last one seemed optional

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

    std::cout << "growth_rate_id: " << exp[i].growth_rate_id << ", "
	      << "level: " << exp[i].level << ", "
	      << "experience: " << exp[i].experience << "\n";
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

    std::cout << "id: " << mv[i].id << ", "
	      << "identifier: " << mv[i].identifier << ", "
	      << "generation_id: " << mv[i].generation_id << ", "
	      << "type_id: " << mv[i].type_id << ", "
	      << "power: " << mv[i].power << ", "
	      << "pp: " << mv[i].pp << ", "
	      << "accuracy: " << mv[i].accuracy << ", "
	      << "priority: " << mv[i].priority << ", "
	      << "target_id: " << mv[i].target_id << ", "
	      << "damage_class_id: " << mv[i].damage_class_id << ", "
	      << "effect_id: " << mv[i].effect_id << ", "
	      << "effect_chance: " << mv[i].effect_chance << ", "
	      << "contest_type_id: " << mv[i].contest_type_id << ", "
	      << "contest_effect_id: " << mv[i].contest_effect_id << ", "
	      << "super_contest_effect_id: " << mv[i].super_contest_effect_id << "\n";
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

    std::cout << "id: " << pkmn[i].id << ", "
	      << "identifier: " << pkmn[i].identifier << ", "
	      << "species_id: " << pkmn[i].species_id << ", "
	      << "height: " << pkmn[i].height << ", "
	      << "weight: " << pkmn[i].weight << ", "
	      << "base_experience: " << pkmn[i].base_experience << ", "
	      << "order: " << pkmn[i].order << ", "
	      << "is_default: " << pkmn[i].is_default << "\n";
      }
}

void file_reader::read_lines_pokemon_moves(ifstream &input_file, int num_lines) {
  //std::cout << "Num_lines / 1000: " << (num_lines / 1000) << "\n";
  //std::cout << "Dimension 2: " << 1000 << "\n";
  //pokemon_moves pkmn_mv[num_lines / 1000][1000];

  std::vector<pokemon_moves> v;
  
  // Skip first line
  String line;
  std::getline(input_file, line);

  std::cout << "got into read lines pokemon moves\n";
  
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
    
    std::cout << "id: " << v[i].pokemon_id << ", "
	      << "version_group_id: " << v[i].version_group_id << ", "
	      << "move_id: " << v[i].move_id << ", "
	      << "pokemon_move_method_id: " << v[i].pokemon_move_method_id << ", "
	      << "level: " << v[i].level << ", "
	      << "order: " << v[i].order << "\n";
    
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

    std::cout << "id: " << pkmn_spc[i].id << ", "
	      << "identifier: " << pkmn_spc[i].identifier << ", "
	      << "generation_id: " << pkmn_spc[i].generation_id << ", "
	      << "evolves_from_species_id: " << pkmn_spc[i].evolves_from_species_id << ", "
	      << "evolution_chain_id: " << pkmn_spc[i].evolution_chain_id << ", "
	      << "color_id: " << pkmn_spc[i].color_id << "\n"
	      << "shape_id: " << pkmn_spc[i].shape_id << ", "
	      << "habitat_id: " << pkmn_spc[i].habitat_id << ", "
	      << "gender_rate: " << pkmn_spc[i].gender_rate << ", "
	      << "capture_rate: " << pkmn_spc[i].capture_rate << ", "
	      << "base_happiness: " << pkmn_spc[i].base_happiness << ", "
	      << "is_baby: " << pkmn_spc[i].is_baby << "\n"
	      << "hatch_counter: " << pkmn_spc[i].hatch_counter << ", "
	      << "has_gender_differences: " << pkmn_spc[i].has_gender_differences << ", "
	      << "growth_rate_id: " << pkmn_spc[i].growth_rate_id << ", "
	      << "forms_switchable: " << pkmn_spc[i].forms_switchable << ", "
	      << "is_legendary: " << pkmn_spc[i].is_legendary << ", "
	      << "is_mythical: " << pkmn_spc[i].is_mythical << "\n"
	      << "order: " << pkmn_spc[i].order << ", "
	      << "conquest_order: " << pkmn_spc[i].conquest_order << "\n";

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

    std::cout << "type_id: " << typ_nm[i].type_id << ", "
	      << "local_language_id: " << typ_nm[i].local_language_id << ", "
	      << "name: " << typ_nm[i].name << "\n";
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
