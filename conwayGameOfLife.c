// compile instructions: use the below command. ensure the SDL library is installed
// gcc -o gameOfLife gameOfLife.c $(sdl2-config --cflags --libs)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define SIZE 40 // Grid size
#define CELL_SIZE 20 // Size of each cell in pixels
#define WINDOW_SIZE (SIZE * CELL_SIZE)

// Cell structure
typedef struct {
    bool alive;
} Cell;

// Function prototypes
void printGrid(SDL_Renderer *renderer, Cell grid[SIZE][SIZE]);
void globalTick(Cell grid[SIZE][SIZE]);
void handleInput(bool *running, int *numTicks, Cell grid[SIZE][SIZE], bool *startSimulation);
void printInstructions();
void drawGridLines(SDL_Renderer *renderer);
void toggleCellState(Cell grid[SIZE][SIZE], int x, int y);

int main() {
    // Print instructions
    printInstructions();

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window *window = SDL_CreateWindow("Game of Life",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_SIZE,
                                          WINDOW_SIZE,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize grid
    Cell grid[SIZE][SIZE] = {0};

    // Main loop
    bool running = true;
    bool startSimulation = false; // Simulation is initially stopped
    int numTicks = 1; // Initial number of ticks per iteration
    while (running) {
        handleInput(&running, &numTicks, grid, &startSimulation);

        if (startSimulation) {
            for (int i = 0; i < numTicks; i++) {
                globalTick(grid);
            }
        }

        printGrid(renderer, grid);
        drawGridLines(renderer);

        SDL_Delay(200); // Delay for animation speed
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Function to print the grid using SDL
void printGrid(SDL_Renderer *renderer, Cell grid[SIZE][SIZE]) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Draw cells
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j].alive) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            }
            SDL_Rect cell = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_RenderFillRect(renderer, &cell);
        }
    }

    // Present the updated grid
    SDL_RenderPresent(renderer);
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

                    // Adjust neighbor indices for wrapping around the grid
                    int neighborX = (i + x + SIZE) % SIZE;
                    int neighborY = (j + y + SIZE) % SIZE;

                    if (grid[neighborX][neighborY].alive) {
                        liveNeighbors++;
                    }
                }
            }

            // Apply rules of the game
            if (grid[i][j].alive) {
                nextGrid[i][j].alive = liveNeighbors == 2 || liveNeighbors == 3;
            } else {
                nextGrid[i][j].alive = liveNeighbors == 3;
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

// Function to handle SDL events (e.g., keyboard and mouse input)
void handleInput(bool *running, int *numTicks, Cell grid[SIZE][SIZE], bool *startSimulation) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    *running = false;
                    break;
                case SDLK_UP:
                    (*numTicks)++;
                    break;
                case SDLK_DOWN:
                    if (*numTicks > 1) (*numTicks)--;
                    break;
                case SDLK_s:
                    *startSimulation = !*startSimulation; // Toggle simulation state
                    break;
                default:
                    break;
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x / CELL_SIZE;
            int y = event.button.y / CELL_SIZE;
            toggleCellState(grid, y, x); // Toggle the state of the clicked cell
        }
    }
}

// Function to toggle the state of a cell (alive/dead)
void toggleCellState(Cell grid[SIZE][SIZE], int x, int y) {
    if (x >= 0 && x < SIZE && y >= 0 && y < SIZE) {
        grid[x][y].alive = !grid[x][y].alive;
    }
}

// Function to print instructions for the user
void printInstructions() {
    printf("Welcome to the Game of Life!\n");
    printf("The grid is %d x %d cells. Each cell can be either alive or dead.\n", SIZE, SIZE);
    printf("You can initialize the grid by clicking on cells to toggle their state.\n");
    printf("During the simulation, you can use the following controls:\n");
    printf("  - UP arrow key: Increase the number of ticks per iteration\n");
    printf("  - DOWN arrow key: Decrease the number of ticks per iteration\n");
    printf("  - ESC key: Quit the simulation\n");
    printf("  - S key: Start or pause the simulation\n");
    printf("  - Mouse Click: Toggle the state of a cell (alive/dead)\n");
    printf("Press ENTER to start...\n");
    getchar(); // Wait for the user to press ENTER
}

// Function to draw grid lines
void drawGridLines(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE); // Light gray for grid lines
    for (int i = 0; i <= SIZE; i++) {
        // Vertical lines
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, WINDOW_SIZE);
        // Horizontal lines
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, WINDOW_SIZE, i * CELL_SIZE);
    }
    SDL_RenderPresent(renderer);
}
