#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "terrain_generation.h"

int main(int argc, char * argv[]) {

  int x_explore_position;
  int y_explore_position;

char scanned;

  generated_map_t *map_exploration[399][399];

 // Random seed 
 srand(time(NULL));

// Start at (199,199)
x_explore_position = 199;
y_explore_position = 199;

map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));

// Continually get buffered input till we quit
while(1) {
  // scanf reads in a character and a newline without this space before it
  scanf(" %c", &scanned);

  switch(scanned) {

  case 'n':
    printf("going north\n");
    break;
  case 's':
    printf("going south\n");
    break;
  case 'e':
    printf("going east\n");
    break;
  case 'w':
    printf("going west\n");
    break;
  case 'f':
    printf("enter an X and a Y to fly to:\n");
    scanf("%d", &x_explore_position);
    scanf("%d", &y_explore_position);
    printf("flying to (%d, %d)\n", x_explore_position, y_explore_position);
    break;
  case 'q':
    printf("quitting game\n");
    return 0;
  default:
    printf("that is not a valid command\n");
    break;
  }

}

return 0;
}
