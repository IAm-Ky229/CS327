#include <stdio.h>

// Function to recursively move the knight
int moveKnight(int x, int y, int stepcount, int backtrack[5][5]);

// Check if this move is good or not
int checkValidity(int x, int y, int board[5][5]);

// Setup the numbered playing board
void setupBoard();

// Setup the board that allows us to see if we've visited a space yet
void setupBacktrack(int backtrack[5][5]);

// Print out the path we have stored
void printPath();

// The playing board to populate with numbers 1 - 25
int board[5][5];

// All possible moves for a knight to take
int moveX[8] = {1, 1, -1, -1, 2, 2, -2, -2};
int moveY[8] = {2, -2, 2, -2, 1, -1, 1, -1};

// Store all recursive moves
int storedMoves[25];

// I used this to verify the number of output lines
int globalCounter = 0;

int main() {

  int i;
  int j;
  int backtrack[5][5];
  int stepcount = 0;

  // Setup the board
  setupBoard();
  setupBacktrack(backtrack);

  // Start from every space and take all possible moves
  for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {

      // Mark the starting space
      backtrack[i][j] = 0;
      moveKnight(i, j, stepcount + 1, backtrack);

      // Clear backtrack board for a new iteration
      setupBacktrack(backtrack);
    }
  }
  
  return 0;
}

int moveKnight(int x, int y, int stepcount, int backtrack[5][5]) {
  
  // Check and see if the board is full. If it is, print out the path
  if(stepcount == 25) {
    printf("\n");
    storedMoves[stepcount-1] = board[x][y];
    printPath();
    //printf("LINES: %d\n", globalCounter);
    //globalCounter++;
  }

  int i;
  int nextXMove;
  int nextYMove;

  for(i = 0; i < 8; i++) {

    // Assign new move
    nextXMove = x + moveX[i];
    nextYMove = y + moveY[i];

    // If this next move is valid, add it to the moves taken
    if(checkValidity(nextXMove, nextYMove, backtrack)) {
      storedMoves[stepcount - 1] = board[x][y];
      backtrack[nextXMove][nextYMove] = stepcount;
      
      if (moveKnight(nextXMove, nextYMove, stepcount + 1, backtrack)) {
	// Don't backtrack
      }
      else
	{
	  // We can't do anything, backtrack
	  backtrack[nextXMove][nextYMove] = -1;
	}
      
    }
  }
    // We can't do anything
    return 0;
}

int checkValidity(int x, int y, int backtrack[5][5]) {

  // X and y must be within range 0 - 4 AND we can't have visited this space
  // on the backtrack board
  return (x <= 4) && (x >= 0)
    && (y <= 4) && (y >= 0) && backtrack[x][y] == -1;
}

void setupBoard() {
  int i;
  int j;
  int counter = 1;

  for(i = 0; i < 5; i++) {
    for(j = 0; j < 5; j++) {
      board[i][j] = counter;
      counter++;
	}
  }
}

void setupBacktrack(int backtrack[5][5]) {
  int i;
  int j;

  for(i = 0; i < 5; i++) {
    for(j = 0; j < 5 ; j++) {
      backtrack[i][j] = -1;
    }
  }
}

void printPath() {
  int i;

  for (i = 0; i < 25; i++) {
    printf("%d, ", storedMoves[i]);
  }
}
