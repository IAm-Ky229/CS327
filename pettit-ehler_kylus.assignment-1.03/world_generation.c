#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <limits.h>

#include "terrain_generation.h"
#include "world_generation.h"
#include "heap.h"

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef uint8_t pair_t[num_dims];
#define mappair(pair) (m->generate_map[pair[dim_x]][pair[dim_y]])
#define mapxy(x, y) (m->generate_map[x][y])
#define heightpair(pair) (m->height[pair[dim_x]][pair[dim_y]])
#define heightxy(x, y) (m->height[x][y])

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

static int32_t edge_penalty(uint8_t x, uint8_t y)
{
  return (x == 1 || y == 1 || x == HORIZONTAL - 2 || y == VERTICAL - 2) ? 2 : 1;
}

static void dijkstra_path(generated_map_t *m, pair_t from, pair_t to)
{
  static path_t path[VERTICAL][HORIZONTAL], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < VERTICAL; y++) {
      for (x = 0; x < HORIZONTAL; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < VERTICAL; y++) {
    for (x = 0; x < HORIZONTAL; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;
  
  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < VERTICAL - 1; y++) {
    for (x = 1; x < HORIZONTAL - 1; x++) {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }
      
  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        heightxy(x, y) = 0;
      }
      heap_delete(&h);
      return;
    }

    printf("dim y: %d, dim x: %d\n", p->pos[dim_y], p->pos[dim_x]);
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1)))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)));
      	 printf("cost assigned: %d\n", path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost);
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
        ((p->cost + heightpair(p->pos)));
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
        ((p->cost + heightpair(p->pos)));
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1)))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)));
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }


  printf("gonna iterate");
  for (y = 0; y < VERTICAL; y++) {
    for (x = 0; x < HORIZONTAL; x++) {
      printf("iterating\n");
      printf("%" PRIu32 " ", path[y][x].cost); 
    }
    printf("\n");
  }

    
}

int main(int argc, char * argv[]) {

  // Used for dijkstra calculation
  pair_t from, to;
  
  // Keep track of place in world
  int x_explore_position;
  int y_explore_position;

  // Used to select some road spot
  int road_spot_x;
  int road_spot_y;

  // The scanned input character
  char scanned;

  // Map we are going to traverse
  generated_map_t *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH];

  // Keep track of exits
  int exit_bottom = -1;
  int exit_right = -1;
  int exit_left = -1;
  int exit_top = -1;

  // Track x and y distance away from center
  // I honestly didn't need to include these, but I did just for clarity
  int manhattan_x = 0;
  int manhattan_y = 0;

  // Be sure that the world is initialized to be completely null
  int i;
  int j;

  // Initialize map
  for(i = 0; i < WORLD_Y_LENGTH; i++) {
    for(j = 0; j < WORLD_X_LENGTH; j++) {
      map_exploration[i][j] = NULL;
    }
  }

 // Random seed 
 srand(time(NULL));

 // Start at (199,199)
 x_explore_position = WORLD_X_START;
 y_explore_position = WORLD_Y_START;

 // Make first map
 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
 generate_new_map(map_exploration[y_explore_position][x_explore_position],
		  exit_bottom,
		  exit_right,
		  exit_left,
		  exit_top,
		  manhattan_x,
		  manhattan_y);
 
 // Continually get buffered input till we quit
 while(1) {
   // Assume we don't have any neighbors
   exit_bottom = -1;
   exit_right = -1;
   exit_top = -1;
   exit_left = -1;
   
   scanf(" %c", &scanned);

   // In general:
   // Check for an existing map. If no map, generate one
   // Update movement
   // Allocate space for map
   // Check for exits that need to connect to the map we will make
   // Generate the new map using the found exits and building spawn rate

   switch(scanned) {
     
   case 'n':
     printf("going north\n");
     if ((y_explore_position + 1) < WORLD_Y_LENGTH) {
       if (map_exploration[y_explore_position + 1][x_explore_position] == NULL) {
	 y_explore_position++;
	 manhattan_y++;
	 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
	 
	 check_exits(map_exploration,
		     x_explore_position,
		     y_explore_position,
		     &exit_bottom, &exit_right, &exit_left, &exit_top);
	 generate_new_map(map_exploration[y_explore_position][x_explore_position],
			  exit_bottom,
			  exit_right,
			  exit_left,
			  exit_top,
			  manhattan_x,
			  manhattan_y);
       }
       else {
	 y_explore_position++;
	 manhattan_y++;
	 print_map(map_exploration[y_explore_position][x_explore_position]);
       }
     }
     else {
       printf("that is not a valid move\n");
     }
     break;
     
   case 's':
     printf("going south\n");
     if ((y_explore_position - 1) > -1) {
       if (map_exploration[y_explore_position - 1][x_explore_position] == NULL) {
	 y_explore_position--;
	 manhattan_y--;
	 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
	 
	 check_exits(map_exploration,
		     x_explore_position,
		     y_explore_position,
		     &exit_bottom, &exit_right, &exit_left, &exit_top);
	 generate_new_map(map_exploration[y_explore_position][x_explore_position],
			  exit_bottom,
			  exit_right,
			  exit_left,
			  exit_top,
			  manhattan_x,
			  manhattan_y);
       }
       else {
	 y_explore_position--;
	 manhattan_y--;
	 print_map(map_exploration[y_explore_position][x_explore_position]);
       }
     }
     else {
       printf("that is not a valid move\n");
     }     
     break;
     
   case 'e':
     printf("going east\n");
     if ((x_explore_position + 1) < WORLD_X_LENGTH) {
       if (map_exploration[y_explore_position][x_explore_position + 1] == NULL) {
	 x_explore_position++;
	 manhattan_x++;
	 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
	 
	 check_exits(map_exploration,
		     x_explore_position,
		     y_explore_position,
		     &exit_bottom, &exit_right, &exit_left, &exit_top);
	 generate_new_map(map_exploration[y_explore_position][x_explore_position],
			  exit_bottom,
			  exit_right,
			  exit_left,
			  exit_top,
			  manhattan_x,
			  manhattan_y);
       }
       else {
	 x_explore_position++;
	 manhattan_x++;
	 print_map(map_exploration[y_explore_position][x_explore_position]);
       }
     }
     else {
       printf("that is not a valid move\n");
     }     
     break;
     
   case 'w':
     printf("going west\n");
     if ((x_explore_position - 1) > -1) {
       if (map_exploration[y_explore_position][x_explore_position - 1] == NULL) {
	 x_explore_position--;
	 manhattan_x--;
	 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
	 
	 check_exits(map_exploration,
		     x_explore_position,
		     y_explore_position,
		     &exit_bottom, &exit_right, &exit_left, &exit_top);
	 generate_new_map(map_exploration[y_explore_position][x_explore_position],
			  exit_bottom,
			  exit_right,
			  exit_left,
			  exit_top,
			  manhattan_x,
			  manhattan_y);
       }
       else {
	 x_explore_position--;
	 manhattan_x--;
	 print_map(map_exploration[y_explore_position][x_explore_position]);
       }
     }
     else {
       printf("that is not a valid move\n");
     }     
     break;
     
   case 'f':
     printf("enter an X and a Y to fly to:\n");
     int x_explore_new;
     int y_explore_new;
     
     scanf("%d", &x_explore_new);
     scanf("%d", &y_explore_new);
     printf("flying to (%d, %d)\n", x_explore_new, y_explore_new);
     
     // Check for in bounds fly
     if(x_explore_new < WORLD_X_LENGTH && x_explore_new > -1
	&& y_explore_new < WORLD_Y_LENGTH && y_explore_new > -1) {
       
       x_explore_position = x_explore_new;
       y_explore_position = y_explore_new;
       
       // Set correct manhattan position
       manhattan_x = x_explore_position - 199;
       manhattan_y = y_explore_position - 199;
       
       if (map_exploration[y_explore_position][x_explore_position] == NULL) {
	 map_exploration[y_explore_position][x_explore_position] = malloc(sizeof(generated_map_t));
	 check_exits(map_exploration,
		     x_explore_position,
		     y_explore_position,
		     &exit_bottom, &exit_right, &exit_left, &exit_top);
	 
	 generate_new_map(map_exploration[y_explore_position][x_explore_position],
			  exit_bottom,
			  exit_right,
			  exit_left,
			  exit_top,
			  manhattan_x,
			  manhattan_y);
       }
       else {
	 print_map(map_exploration[y_explore_position][x_explore_position]);
       }
     }
     else {
       printf("that is not a valid move\n");
     }     
     break;
     
   case 'q':
     printf("quitting game\n");
     return 0;
     
   default:
     printf("that is not a valid command\n");
     break;
   }

   choose_random_road_spot(map_exploration[y_explore_position][x_explore_position], &road_spot_x, &road_spot_y);

   from[dim_x] = road_spot_x;
   from[dim_y] = road_spot_y;
   
       to[dim_x] = 10;
       to[dim_y] = 10;

       printf("X: %d\n", road_spot_x);
       printf("Y: %d\n", road_spot_y);
       dijkstra_path(map_exploration[y_explore_position][x_explore_position], from, to);
     

 }
 
 return 0;
}


void check_edge_cases(generated_map_t *map_data, int y_explore_position, int x_explore_position) {

  int i;

  if(y_explore_position == 0) {
    for(i = 0; i < HORIZONTAL; i++) {
      map_data -> generate_map[i][VERTICAL - 1] = boulder;
    }
  }

  if(y_explore_position == 399) {
    for(i = 0; i < HORIZONTAL; i++) {
      map_data -> generate_map[i][0] = boulder;
    }
  }

  if(x_explore_position == 0) {
    for(i = 0; i < VERTICAL; i++) {
      map_data -> generate_map[0][i] = boulder;
    }
  }

  if(x_explore_position == 399) {
    for(i = 0; i < VERTICAL; i++) {
      map_data -> generate_map[HORIZONTAL - 1][i] = boulder;
    }
  }
  
}

void generate_new_map(generated_map_t *map_data,
		      int exit_bottom,
		      int exit_right,
		      int exit_left,
		      int exit_top,
		      int manhattan_x,
		      int manhattan_y) {
  
  // I was getting some weird memory problems without initializing
  // everything to nothing
  int i;
  int j;
  
  for(i = 0; i < VERTICAL; i++) {
    for(j = 0; j < HORIZONTAL; j++) {
      map_data -> generate_map[j][i] = nothing;
    }
  }


  // Calculate manhattan distance
  int manhattan_distance = abs(manhattan_x) + abs(manhattan_y);

  // Do the actual spawn rate conversion (I don't divide by 100)
  int building_spawn_rate = (((-45 * manhattan_distance) / 200) + 50);

  if (building_spawn_rate < 5) {
    building_spawn_rate = 5;
  }
  
  // Function calls need to happen in this order
  place_clearings(map_data);
  place_tall_grass(map_data);
  place_border_boulders(map_data);
  place_exits(map_data, exit_bottom, exit_right, exit_left, exit_top);
  place_paths(map_data);
  place_buildings(map_data, building_spawn_rate);
  fill_blank_space(map_data);

  // To avoid passing in y and x explore, I just add 199 to the manhattan counters
  check_edge_cases(map_data, manhattan_y + 199, manhattan_x + 199);
  print_map(map_data);
  
}

void check_exits(generated_map_t *map_exploration[WORLD_Y_LENGTH][WORLD_X_LENGTH],
		 int x_explore,
		 int y_explore,
		 int *exit_bottom,
		 int *exit_right,
		 int *exit_left,
		 int *exit_top) {

  if(y_explore + 1 < 400) {
    if(map_exploration[y_explore + 1][x_explore] != NULL) {
      *exit_top = map_exploration[y_explore + 1][x_explore] -> exit_bottom;
    }
  }

  if(x_explore + 1 < 400) {
    if(map_exploration[y_explore][x_explore + 1] != NULL) {
      *exit_right = map_exploration[y_explore][x_explore + 1] -> exit_left;
    }
  }

  if(y_explore - 1 > -1) {
    if(map_exploration[y_explore - 1][x_explore] != NULL) {
      *exit_bottom = map_exploration[y_explore - 1][x_explore] -> exit_top;
    }
  }

  if(x_explore - 1 > -1) {
    if(map_exploration[y_explore][x_explore - 1] != NULL) {
      *exit_left = map_exploration[y_explore][x_explore - 1] -> exit_right;
    }
  }
  
}

void choose_random_road_spot(generated_map_t *map_data, int *chosen_spot_x, int *chosen_spot_y) {

  int i;
  int j;

  for (i = (rand() % (19 - 2 + 1)) + 2; i < VERTICAL; i++) {
    for(j = (rand() % (78 - 2 + 1)) + 2; j < HORIZONTAL; j++) {
      if(map_data -> generate_map[j][i] == path) {
	*chosen_spot_x = j;
	*chosen_spot_y = i;
	return;
      }
    }
  }
  
}
