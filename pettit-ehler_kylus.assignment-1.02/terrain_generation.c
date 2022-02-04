#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "terrain_generation.h"

int main(int argc, char *argv[]) {

  // Random seed 
  srand(time(NULL));

  // Function calls need to happen in this order
  place_clearings();
  place_tall_grass();
  place_border_boulders();
  place_exits();
  place_paths();
  place_buildings();
  fill_blank_space();

  print_map();
  return 0;
}

void print_map() {

  int i;
  int j;

  for(i = 0; i < VERTICAL; i++) {
    for(j = 0; j < HORIZONTAL; j++) {

      // Switch statement for every enumeration
      switch(map_data.generate_map[j][i]) {
      case boulder :
	printf("%%");
	break;
      case clearing :
	printf(".");
	break;
      case tall_grass :
	printf(",");
	break;
      case border_exit :
	printf("#");
	break;
      case path :
	printf("#");
	break;
      case pokemon_center :
	printf("C");
	break;
      case pokemon_mart :
	printf("M");
	break;
      case nothing :
	printf(":");
	break;
      case tree :
	printf("^");
	break;
      default :
	printf(" ");
	break;
      }
    }
    printf("\n");
  }

}

void place_border_boulders() {

  int i;

  // Boulders go along the edges
  for(i = 0; i < HORIZONTAL; i++) {
    map_data.generate_map[i][0] = boulder;
  }
  
  for(i = 0; i < HORIZONTAL; i++) {
  map_data.generate_map[i][VERTICAL - 1] = boulder;
  }
  
  for(i = 0; i < VERTICAL; i++) {
  map_data.generate_map[0][i] = boulder;
  }
 
  for(i = 0; i < VERTICAL; i++) {
  map_data.generate_map[HORIZONTAL - 1][i] = boulder;
  }
  
}

void place_clearings() {
  
  int j;
  int k;
  int x_pos;
  int y_pos;
  int length;
  int height;
  int number_clearings = 0;
  int overlap = 0;

  // While we have less than 2 non-overlapping clearings
  while(number_clearings < 2) {
    overlap = 0;

    x_pos = rand() % 80;
    y_pos = rand() % 21;
    
    length = (rand() % (MAXLENGTH - MINLENGTH + 1)) + MINLENGTH;
    
    height = (rand() % (MAXHEIGHT - MINHEIGHT + 1)) + MINHEIGHT;

    // Check and see if we are overlapping
    for(j = y_pos - height; j < y_pos + height; j++) {
      for(k = x_pos - length; k < x_pos + height; k++) {
	if(map_data.generate_map[k][j] == clearing) {
	  overlap = 1;
	}
      }
    }

    // If we did not detect overlap
    if(overlap != 1) {
      for(j = y_pos - height; j < y_pos + height; j++) {
	for(k = x_pos - length; k < x_pos + height; k++) {
	  if((j > 0) && (j < 21) && (k > 0) && (k < 80)
	     && map_data.generate_map[k+1][j] != clearing) {
	    map_data.generate_map[k][j] = clearing;
	  }
	}
      }
    }
    
    // Else, we overlapped (1 big clearing),
    // this clearing doesn't count
    else {
      number_clearings--;
    }
    number_clearings++;
  }
}
    

void place_tall_grass() {
    
  int j;
  int k;
  int x_pos;
  int y_pos;
  int length;
  int height;
  int number_tall_grasses = 0;
  int overlap = 0;

  // While we don't have 2 non-overlapping tall grasses
  while(number_tall_grasses < 2) {
    overlap = 0;

    x_pos = rand() % 80;
    y_pos = rand() % 21;

    // Generate length in a range
    length = (rand() % (MAXLENGTH - MINLENGTH + 1)) + MINLENGTH;

    // Generate height in a range
    height = (rand() % (MAXHEIGHT - MINHEIGHT + 1)) + MINHEIGHT;

    // Check and see if we are overlapping (one big tall grass region)
    for(j = y_pos - height; j < y_pos + height; j++) {
      for(k = x_pos - length; k < x_pos + height; k++) {
	if(map_data.generate_map[k][j] == tall_grass) {
	  overlap = 1;
	}
      }
    }

    // If we are not overlapping already
    if(overlap != 1) {
      for(j = y_pos - height; j < y_pos + height; j++) {
	for(k = x_pos - length; k < x_pos + height; k++) {
	  if((j > 0) && (j < 21) && (k > 0) && (k < 80)
	     && map_data.generate_map[k+1][j] != tall_grass) {
	    map_data.generate_map[k][j] = tall_grass;
	  }
	}
      }
    }
    
    // Else, we found overlap, and need to try again to make
    // two distinct regions
    else {
      number_tall_grasses--;
    }
    number_tall_grasses++;
  }
}

void place_buildings() {

  int selected_row = (rand() % (18 - 3 + 1)) + 3;
  int selected_column = (rand() % (77 - 3 + 1)) + 3;

  // We don't want to place a building on the literal exit row/column
  while(selected_row == map_data.exit_right
	|| selected_row == map_data.exit_left) {
    selected_row = (rand() % (18 - 3 + 1)) + 3;
  }

  // We don't want to place a building on the literal exit row/column
  // We also don't want the mart and the center to overlap
  while(selected_column == map_data.exit_top
	|| selected_column == map_data.exit_bottom
	|| ((selected_column - selected_row < 3)
	    && (selected_column - selected_row > -3))) {
    selected_column = (rand() % (77 - 3 + 1)) + 3;
  }

  place_pokemart(selected_row);
  place_pokecenter(selected_column);

}

void place_pokemart(int selected_row) {
  
  int j = 0;

  // Iterate over entire row
    while(j < HORIZONTAL) {

      // If we find a path
      if(map_data.generate_map[j][selected_row] == path) {
	
	// We don't need to check bounds, as roads cannot be less
	// Than 2 units away from the edges
	// Also, we are going to the first instance of a road, so
	// we can just place the building behind us and avoid checking anything
	map_data.generate_map[j-2][selected_row] = pokemon_mart;
	map_data.generate_map[j-1][selected_row] = pokemon_mart;
	map_data.generate_map[j-1][selected_row - 1] = pokemon_mart;
	map_data.generate_map[j-2][selected_row - 1] = pokemon_mart;
	break;
      }
      j++;
    }
}

void place_pokecenter(int selected_column) {
  
  int j = 0;
  
    while(j < VERTICAL) {
      if(map_data.generate_map[selected_column][j] == path) {
	
	// We don't need to check bounds, as roads cannot be less
	// Than 2 units away from the edges
	// Also, we are going to the first instance of a road, so
	// we can just place the building behind us and avoid checking anything
	map_data.generate_map[selected_column][j-2] = pokemon_center;
	map_data.generate_map[selected_column][j-1] = pokemon_center;
	map_data.generate_map[selected_column - 1][j-1] = pokemon_center;
	map_data.generate_map[selected_column - 1][j-2] = pokemon_center;
	break;
	}  
      j++;
    }
  
}

void place_exits() {

  // Place all exits within the range 3 - 17 or 3 - 76
  // (leave space for buildings to be on either side of path
  map_data.exit_right = (rand() % (17 - 3 + 1)) + 3;
  map_data.exit_bottom = (rand() % (76 -32 + 1)) + 3;
  map_data.exit_left = (rand() % (17 - 3 + 1)) + 3;
  map_data.exit_top = (rand() % (76 - 3 + 1)) + 3;

  map_data.generate_map[HORIZONTAL - 1][map_data.exit_right] = border_exit;
  map_data.generate_map[map_data.exit_bottom][VERTICAL - 1] = border_exit;
  map_data.generate_map[0][map_data.exit_left] = border_exit;
  map_data.generate_map[map_data.exit_top][0] = border_exit;

}

void place_paths() {

  // Choose random spot to connect top and bottom paths
  int connect_top_bottom = (rand() % (17 - 3 + 1)) + 3;
  int connect_right_left = (rand() % (76 - 3 + 1)) + 3;
  int connect_top_bottom_counter = 0;
  int connect_right_left_counter = 0;
  int connect_paths;

  // Build top path down
  while (connect_top_bottom_counter < connect_top_bottom) {
    map_data.generate_map[map_data.exit_top][connect_top_bottom_counter] = path;
    connect_top_bottom_counter++;
  }

  connect_top_bottom_counter = VERTICAL - 1;

  // Build bottom path up
  while(connect_top_bottom_counter > connect_top_bottom - 1) {
    map_data.generate_map[map_data.exit_bottom][connect_top_bottom_counter] = path;
    connect_top_bottom_counter--;
  }

  // If the top exit is farther right, add a connecting road to the bottom path
  if(map_data.exit_top > map_data.exit_bottom) {
    connect_paths = map_data.exit_bottom;
    while(connect_paths < map_data.exit_top + 1) {
      map_data.generate_map[connect_paths][connect_top_bottom_counter] = path;
      connect_paths++;
    }
  }
  // Else, add a connecting road to the top path
  else {
      connect_paths = map_data.exit_top;
      while(connect_paths < map_data.exit_bottom + 1) {
	map_data.generate_map[connect_paths][connect_top_bottom_counter] = path;
	connect_paths++;
      }
  }

  // Build out a road from the left path
  while (connect_right_left_counter < connect_right_left) {
    map_data.generate_map[connect_right_left_counter][map_data.exit_left] = path;
    connect_right_left_counter++;
  }

  connect_right_left_counter = HORIZONTAL - 1;

  // Build out a road from the right path
  while(connect_right_left_counter > connect_right_left - 1) {
    map_data.generate_map[connect_right_left_counter][map_data.exit_right] = path;
    connect_right_left_counter--;
  }

  // If the right exit is above the left exit
  if(map_data.exit_right < map_data.exit_left) {
    connect_paths = map_data.exit_right;

    // Build the connecting path down from the left exit
    while(connect_paths < map_data.exit_left + 1) {
      map_data.generate_map[connect_right_left_counter][connect_paths] = path;
      connect_paths++;
    }
  }
  // Otherwise, build the connecting path down from the right exit
    else {
      connect_paths = map_data.exit_left;
      while(connect_paths < map_data.exit_right + 1) {
	map_data.generate_map[connect_right_left_counter][connect_paths] = path;
	connect_paths++;
      }
    }
  

}

void fill_blank_space() {

  int i;
  int j;
  int random_number;

  // Iterate over entire 2-D array
  for(i = 0; i < VERTICAL; i++) {
    for (j = 0; j < HORIZONTAL; j++) {

      // If we find unassigned area
      if(map_data.generate_map[j][i] == nothing) {

	random_number = rand() % 100;

	// 90% of time, we will just get nothing back
	if(random_number < 90) {
	  map_data.generate_map[j][i] = nothing;
	}

	// 7% of time, we wil get a tree
	else if(random_number < 97) {
	  map_data.generate_map[j][i] = tree;
	}

	// 3% of time, we will get a boulder
	else {
	  map_data.generate_map[j][i] = boulder;
	}
      }

    }
  }
}
