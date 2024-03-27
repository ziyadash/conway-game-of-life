#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 20 // Grid size

// Cell structure
typedef struct {
    bool alive;
} Cell;

// Function prototypes
void printGrid(Cell grid[SIZE][SIZE]);
void globalTick(Cell grid[SIZE][SIZE]);
void initializeGrid(Cell grid[SIZE][SIZE]);

int main() {
    // Initialize grid
    Cell grid[SIZE][SIZE] = {0};

    // Initialize grid with user input
    initializeGrid(grid);

    // Print initial grid
    printGrid(grid);

    // Simulation loop
    char input;
    while (true) {
        printf("Press 'i' to iterate, 'x' to exit: ");
        scanf(" %c", &input);

        if (input == 'x') {
            break;
        } else if (input == 'i') {
            // Advance one tick
            globalTick(grid);
            printGrid(grid);
        } else {
            printf("Invalid input. Try again.\n");
        }
    }

    return 0;
}

// Function to print the grid
void printGrid(Cell grid[SIZE][SIZE]) {
    system("clear"); // Clear the terminal screen

    // Print the grid
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", grid[i][j].alive ? '*' : '.');
        }
        printf("\n");
    }
}

// Function to update the grid for one tick
void globalTick(Cell grid[SIZE][SIZE]) {
    Cell nextGrid[SIZE][SIZE] = {0}; // Temporary grid to hold next state

    // Iterate over each cell in the grid
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            // Count live neighbors
            int liveNeighbors = 0;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    if (x == 0 && y == 0) continue; // Skip the current cell
                    int neighborX = i + x;
                    int neighborY = j + y;
                    if (neighborX >= 0 && neighborX < SIZE && neighborY >= 0 && neighborY < SIZE) {
                        if (grid[neighborX][neighborY].alive) {
                            liveNeighbors++;
                        }
                    }
                }
            }

            // Apply rules of the game
            if (grid[i][j].alive) {
                // Any live cell with fewer than two live neighbors dies
                // Any live cell with more than three live neighbors dies
                if (liveNeighbors < 2 || liveNeighbors > 3) {
                    nextGrid[i][j].alive = false;
                } else {
                    nextGrid[i][j].alive = true;
                }
            } else {
                // Any dead cell with exactly three live neighbors becomes a live cell
                if (liveNeighbors == 3) {
                    nextGrid[i][j].alive = true;
                }
            }
        }
    }

    // Update the original grid with the next state
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = nextGrid[i][j];
        }
    }
}

// Function to initialize the grid based on user input
void initializeGrid(Cell grid[SIZE][SIZE]) {
    int x, y;

    printf("Enter the coordinates of live cells (x y). Enter -1 to end.\n");
    while (true) {
        printf("Enter coordinates (x y): ");
        scanf("%d %d", &x, &y);

        if (x == -1 || y == -1) {
            break;
        }

        if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
            printf("Invalid coordinates. Please enter coordinates within the grid range (0-%d).\n", SIZE - 1);
            continue;
        }

        grid[x][y].alive = true;
    }
}
