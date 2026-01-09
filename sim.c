// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "particle_sim.h"

// #define GRAVITY_STEPS 9
// #define PARTICLE_SPAWN_RADIUS 10
// #define PARTICLE_GENERATION_RATE 300
// #define SPREAD_ITERATIONS 4

// CellType grid[GRID_HEIGHT][GRID_WIDTH];
// CellType nextGrid[GRID_HEIGHT][GRID_WIDTH];
// int particle_count = 0;
// static int lastMouseX = -1;
// static int lastMouseY = -1;

// void add_obstacle(int mouseX, int mouseY, int isDragging) {
//     int gridX = mouseX / CELL_SIZE;
//     int gridY = mouseY / CELL_SIZE;
//     int brushSize = 2;

//     // Apply brush at the current position
//     for (int y = gridY - brushSize; y <= gridY + brushSize; y++) {
//         for (int x = gridX - brushSize; x <= gridX + brushSize; x++) {
//             if (y >= 0 && y < GRID_HEIGHT && x >= 0 && x < GRID_WIDTH) {
//                 if (grid[y][x] == WATER) particle_count--;
//                 grid[y][x] = OBSTACLE;
//             }
//         }
//     }

//     // If dragging and there’s a valid previous position, connect with a line
//     if (isDragging && lastMouseX != -1 && lastMouseY != -1) {
//         int lastGridX = lastMouseX / CELL_SIZE;
//         int lastGridY = lastMouseY / CELL_SIZE;

//         // Bresenham's line algorithm to fill gaps between last and current position
//         int dx = abs(gridX - lastGridX);
//         int dy = abs(gridY - lastGridY);
//         int sx = (lastGridX < gridX) ? 1 : -1;
//         int sy = (lastGridY < gridY) ? 1 : -1;
//         int err = dx - dy;

//         int x = lastGridX;
//         int y = lastGridY;

//         while (1) {
//             // Apply brush at each point along the line (excluding the endpoint, already drawn)
//             if (x != gridX || y != gridY) {  // Avoid overwriting the current position
//                 for (int by = y - brushSize; by <= y + brushSize; by++) {
//                     for (int bx = x - brushSize; bx <= x + brushSize; bx++) {
//                         if (by >= 0 && by < GRID_HEIGHT && bx >= 0 && bx < GRID_WIDTH) {
//                             if (grid[by][bx] == WATER) particle_count--;
//                             grid[by][bx] = OBSTACLE;
//                         }
//                     }
//                 }
//             }

//             if (x == gridX && y == gridY) break;

//             int e2 = 2 * err;
//             if (e2 > -dy) {
//                 err -= dy;
//                 x += sx;
//             }
//             if (e2 < dx) {
//                 err += dx;
//                 y += sy;
//             }
//         }
//     }

//     // Update last position
//     lastMouseX = mouseX;
//     lastMouseY = mouseY;
// }

// void add_water(int mouseX, int mouseY) {
//     int gridX = mouseX / CELL_SIZE;
//     int gridY = mouseY / CELL_SIZE;
//     for (int i = 0; i < PARTICLE_GENERATION_RATE; i++) {
//         int offsetX = (rand() % (2 * PARTICLE_SPAWN_RADIUS + 1)) - PARTICLE_SPAWN_RADIUS;
//         int offsetY = (rand() % (2 * PARTICLE_SPAWN_RADIUS + 1)) - PARTICLE_SPAWN_RADIUS;
//         int newX = gridX + offsetX;
//         int newY = gridY + offsetY;
//         if (newY >= 0 && newY < GRID_HEIGHT && newX >= 0 && newX < GRID_WIDTH &&
//             grid[newY][newX] == EMPTY) {
//             grid[newY][newX] = WATER;
//             particle_count++;
//         }
//     }
// }

// void update_simulation() {
//     for (int step = 0; step < GRAVITY_STEPS; step++) {
//         memset(nextGrid, EMPTY, sizeof(nextGrid));
//         for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
//             for (int x = 0; x < GRID_WIDTH; x++) {
//                 if (grid[y][x] == WATER) {
//                     if (y + 1 < GRID_HEIGHT && grid[y + 1][x] != OBSTACLE && nextGrid[y + 1][x] == EMPTY) {
//                         nextGrid[y + 1][x] = WATER;
//                     } else {
//                         nextGrid[y][x] = WATER;
//                     }
//                 } else if (grid[y][x] == OBSTACLE) {
//                     nextGrid[y][x] = OBSTACLE;
//                 }
//             }
//         }
//         memcpy(grid, nextGrid, sizeof(grid));
//     }

//     for (int iter = 0; iter < SPREAD_ITERATIONS; iter++) {
//         memcpy(nextGrid, grid, sizeof(nextGrid));
//         for (int y = 0; y < GRID_HEIGHT; y++) {
//             for (int x = 0; x < GRID_WIDTH; x++) {
//                 if (grid[y][x] == WATER && (y == GRID_HEIGHT - 1 || grid[y + 1][x] != EMPTY)) {
//                     int dir = (rand() % 2) ? 1 : -1;
//                     int target_x = x + dir;
//                     if (target_x >= 0 && target_x < GRID_WIDTH && grid[y][target_x] != OBSTACLE && 
//                         nextGrid[y][target_x] == EMPTY) {
//                         nextGrid[y][target_x] = WATER;
//                         nextGrid[y][x] = EMPTY;
//                     } else {
//                         dir = -dir;
//                         target_x = x + dir;
//                         if (target_x >= 0 && target_x < GRID_WIDTH && grid[y][target_x] != OBSTACLE && 
//                             nextGrid[y][target_x] == EMPTY) {
//                             nextGrid[y][target_x] = WATER;
//                             nextGrid[y][x] = EMPTY;
//                         }
//                     }
//                 } else if (grid[y][x] == OBSTACLE) {
//                     nextGrid[y][x] = OBSTACLE;
//                 }
//             }
//         }
//         memcpy(grid, nextGrid, sizeof(grid));
//     }
// }
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "particle_sim.h"

#define GRAVITY_STEPS 8
#define PARTICLE_SPAWN_RADIUS 8
#define PARTICLE_GENERATION_RATE 200
#define SPREAD_ITERATIONS 4
#define MAX_SPREAD_DISTANCE 6
#define STACK_THRESHOLD 3
#define INITIAL_ACTIVE_CAPACITY 1000
#define DENSITY_RADIUS 3  // For broader surface awareness
#define NEAR_DENSITY_RADIUS 1  // For cohesion
#define NEAR_DENSITY_FACTOR 0.6f  // Weight for near-density in flattening
#define HEIGHT_FACTOR 1.2f  // Weight for height difference in flattening

// Grid dimensions (initialized at runtime)
int GRID_WIDTH = DEFAULT_GRID_WIDTH;
int GRID_HEIGHT = DEFAULT_GRID_HEIGHT;
CellType** grid = NULL;
int particle_count = 0;
static int lastMouseX = -1;
static int lastMouseY = -1;
ActiveCell* active_cells = NULL;
int active_count = 0;
int active_capacity = 0;

// Initialize the grid
void init_grid(int width, int height) {
    // Free existing grid if any
    free_grid();
    
    GRID_WIDTH = width;
    GRID_HEIGHT = height;
    
    // Allocate grid as a 2D array (array of pointers to rows)
    grid = (CellType**)malloc(GRID_HEIGHT * sizeof(CellType*));
    if (grid == NULL) {
        printf("Error: Failed to allocate memory for grid rows\n");
        exit(1);
    }
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
        grid[y] = (CellType*)malloc(GRID_WIDTH * sizeof(CellType));
        if (grid[y] == NULL) {
            printf("Error: Failed to allocate memory for grid row %d\n", y);
            // Free already allocated rows
            for (int i = 0; i < y; i++) {
                free(grid[i]);
            }
            free(grid);
            grid = NULL;
            exit(1);
        }
        // Initialize to EMPTY
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = EMPTY;
        }
    }
    
    particle_count = 0;
}

// Free the grid
void free_grid(void) {
    if (grid != NULL) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            free(grid[y]);
        }
        free(grid);
        grid = NULL;
    }
    GRID_WIDTH = 0;
    GRID_HEIGHT = 0;
    particle_count = 0;
}

// Initialize the active cells list
void init_active_cells() {
    if (active_cells == NULL) {
        active_capacity = INITIAL_ACTIVE_CAPACITY;
        active_cells = (ActiveCell*)malloc(active_capacity * sizeof(ActiveCell));
        if (active_cells == NULL) {
            printf("Error: Failed to allocate memory for active cells\n");
            exit(1);
        }
        active_count = 0;
    }
}

// Add a cell to the active list
void add_active_cell(int x, int y) {
    if (active_count >= active_capacity) {
        active_capacity *= 2;
        active_cells = (ActiveCell*)realloc(active_cells, active_capacity * sizeof(ActiveCell));
        if (active_cells == NULL) {
            printf("Error: Failed to reallocate memory for active cells\n");
            exit(1);
        }
    }
    active_cells[active_count].x = x;
    active_cells[active_count].y = y;
    active_count++;
}

// Count water particles in a 2D neighborhood
int count_neighbors(int y, int x, int radius) {
    int count = 0;
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int ny = y + dy;
            int nx = x + dx;
            if (ny >= 0 && ny < GRID_HEIGHT && nx >= 0 && nx < GRID_WIDTH &&
                grid[ny][nx] == WATER) {
                count++;
            }
        }
    }
    return count;
}

// Count water particles in the vertical stack above
int count_stack(int y, int x) {
    int count = 0;
    for (int ny = y - 1; ny >= 0; ny--) {
        if (grid[ny][x] != WATER) break;
        count++;
    }
    return count;
}

// Estimate surface height at position (x, y) by finding the topmost water particle
int get_surface_height(int x) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        if (grid[y][x] == WATER) return y;
        if (grid[y][x] == OBSTACLE) {
            // If we hit an obstacle, check for water on top
            for (int ny = y - 1; ny >= 0; ny--) {
                if (grid[ny][x] == WATER) return ny;
            }
            return GRID_HEIGHT; // No water above obstacle
        }
    }
    return GRID_HEIGHT; // No water in column
}

void add_obstacle(int mouseX, int mouseY, int isDragging) {
    if (grid == NULL) return;
    
    int gridX = mouseX / CELL_SIZE;
    int gridY = mouseY / CELL_SIZE;
    int brushSize = 2;

    for (int y = gridY - brushSize; y <= gridY + brushSize; y++) {
        for (int x = gridX - brushSize; x <= gridX + brushSize; x++) {
            if (y >= 0 && y < GRID_HEIGHT && x >= 0 && x < GRID_WIDTH) {
                if (grid[y][x] == WATER) {
                    particle_count--;
                }
                grid[y][x] = OBSTACLE;
            }
        }
    }

    if (isDragging && lastMouseX != -1 && lastMouseY != -1) {
        int lastGridX = lastMouseX / CELL_SIZE;
        int lastGridY = lastMouseY / CELL_SIZE;

        int dx = abs(gridX - lastGridX);
        int dy = abs(gridY - lastGridY);
        int sx = (lastGridX < gridX) ? 1 : -1;
        int sy = (lastGridY < gridY) ? 1 : -1;
        int err = dx - dy;

        int x = lastGridX;
        int y = lastGridY;

        while (1) {
            if (x != gridX || y != gridY) {
                for (int by = y - brushSize; by <= y + brushSize; by++) {
                    for (int bx = x - brushSize; bx <= x + brushSize; bx++) {
                        if (by >= 0 && by < GRID_HEIGHT && bx >= 0 && bx < GRID_WIDTH) {
                            if (grid[by][bx] == WATER) {
                                particle_count--;
                            }
                            grid[by][bx] = OBSTACLE;
                        }
                    }
                }
            }

            if (x == gridX && y == gridY) break;

            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x += sx;
            }
            if (e2 < dx) {
                err += dx;
                y += sy;
            }
        }
    }

    lastMouseX = mouseX;
    lastMouseY = mouseY;
}

void add_water(int mouseX, int mouseY) {
    if (grid == NULL) return;
    
    int gridX = mouseX / CELL_SIZE;
    int gridY = mouseY / CELL_SIZE;
    
    // Ensure coordinates are within bounds
    if (gridX < 0) gridX = 0;
    if (gridX >= GRID_WIDTH) gridX = GRID_WIDTH - 1;
    if (gridY < 0) gridY = 0;
    if (gridY >= GRID_HEIGHT) gridY = GRID_HEIGHT - 1;
    
    // Add particles with better distribution - prioritize center, then spread outward
    int particles_added = 0;
    int max_attempts = PARTICLE_GENERATION_RATE * 2;
    int attempts = 0;
    
    // First, try to add particles near the center
    for (int radius = 0; radius <= PARTICLE_SPAWN_RADIUS && particles_added < PARTICLE_GENERATION_RATE; radius++) {
        for (int i = 0; i < PARTICLE_GENERATION_RATE && particles_added < PARTICLE_GENERATION_RATE && attempts < max_attempts; i++) {
            int offsetX = (rand() % (2 * radius + 1)) - radius;
            int offsetY = (rand() % (2 * radius + 1)) - radius;
            
            // Use distance check for more circular distribution
            if (offsetX * offsetX + offsetY * offsetY <= radius * radius) {
                int newX = gridX + offsetX;
                int newY = gridY + offsetY;
                
                if (newY >= 0 && newY < GRID_HEIGHT && newX >= 0 && newX < GRID_WIDTH &&
                    grid[newY][newX] == EMPTY) {
                    grid[newY][newX] = WATER;
                    particle_count++;
                    add_active_cell(newX, newY);
                    particles_added++;
                }
            }
            attempts++;
        }
    }
}

void update_simulation() {
    if (grid == NULL) return;
    
    init_active_cells();

    // Rebuild active list
    active_count = 0;
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x] == WATER) {
                add_active_cell(x, y);
            }
        }
    }

    // Gravity phase: Process only active cells
    for (int step = 0; step < GRAVITY_STEPS; step++) {
        for (int i = active_count - 1; i >= 0; i--) {
            int x = active_cells[i].x;
            int y = active_cells[i].y;
            if (grid[y][x] != WATER) continue;

            int moved = 0;
            int stack_count = count_stack(y, x);
            int directions[3][2] = {{1, 0}, {1, -1}, {1, 1}};
            for (int j = 0; j < 3 && !moved; j++) {
                int dy = directions[j][0];
                int dx = directions[j][1];
                int ny = y + dy;
                int nx = x + dx;
                if (ny < GRID_HEIGHT && nx >= 0 && nx < GRID_WIDTH &&
                    grid[ny][nx] == EMPTY) {
                    grid[ny][nx] = WATER;
                    grid[y][x] = EMPTY;
                    active_cells[i].x = nx;
                    active_cells[i].y = ny;
                    moved = 1;
                    if (stack_count >= STACK_THRESHOLD) {
                        int spaces_below = 0;
                        for (int check_y = ny; check_y < GRID_HEIGHT; check_y++) {
                            if (grid[check_y][nx] != EMPTY) break;
                            spaces_below++;
                        }
                        int stack_y = y - 1;
                        int stack_moved = 0;
                        while (stack_y >= 0 && grid[stack_y][x] == WATER &&
                               stack_moved < stack_count && stack_moved < spaces_below - 1) {
                            int target_y = ny + stack_moved + 1;
                            if (target_y < GRID_HEIGHT && grid[target_y][nx] == EMPTY) {
                                grid[target_y][nx] = WATER;
                                grid[stack_y][x] = EMPTY;
                                for (int k = 0; k < active_count; k++) {
                                    if (active_cells[k].x == x && active_cells[k].y == stack_y) {
                                        active_cells[k].x = nx;
                                        active_cells[k].y = target_y;
                                        break;
                                    }
                                }
                                stack_moved++;
                            } else {
                                break;
                            }
                            stack_y--;
                        }
                    }
                }
            }
            if (!moved) {
                // Try to fall straight down as last resort
                int ny = y + 1;
                if (ny < GRID_HEIGHT && grid[ny][x] == EMPTY) {
                    grid[ny][x] = WATER;
                    grid[y][x] = EMPTY;
                    active_cells[i].x = x;
                    active_cells[i].y = ny;
                }
            }
        }
    }

    // Spreading phase: Enhanced for realistic flattening with better flow
    for (int iter = 0; iter < SPREAD_ITERATIONS; iter++) {
        // Process in random order to avoid directional bias
        for (int idx = 0; idx < active_count; idx++) {
            int i = (idx + iter * 7) % active_count; // Pseudo-randomize order
            int x = active_cells[i].x;
            int y = active_cells[i].y;
            if (grid[y][x] != WATER) continue;

            // Only spread if particle is on a surface (can't fall further)
            if (y == GRID_HEIGHT - 1 || grid[y + 1][x] != EMPTY) {
                int density = count_neighbors(y, x, DENSITY_RADIUS);
                int near_density = count_neighbors(y, x, NEAR_DENSITY_RADIUS);
                int current_height = get_surface_height(x);
                int best_target_x = x;
                float best_score = density + NEAR_DENSITY_FACTOR * near_density +
                                  HEIGHT_FACTOR * current_height;

                // Check both directions with alternating priority for natural flow
                int dir = (iter % 2 == 0) ? -1 : 1; // Alternate direction each iteration
                
                // Check in preferred direction first
                for (int dx = 1; dx <= MAX_SPREAD_DISTANCE; dx++) {
                    int tx = x + dir * dx;
                    if (tx < 0 || tx >= GRID_WIDTH || grid[y][tx] != EMPTY) break;
                    int target_density = count_neighbors(y, tx, DENSITY_RADIUS);
                    int target_near_density = count_neighbors(y, tx, NEAR_DENSITY_RADIUS);
                    int target_height = get_surface_height(tx);
                    float score = target_density + NEAR_DENSITY_FACTOR * target_near_density +
                                 HEIGHT_FACTOR * target_height;
                    if (score < best_score) {
                        best_target_x = tx;
                        best_score = score;
                        break; // Take first good option for faster flow
                    }
                }

                // Check opposite direction if no good option found
                if (best_target_x == x) {
                    for (int dx = 1; dx <= MAX_SPREAD_DISTANCE; dx++) {
                        int tx = x - dir * dx;
                        if (tx < 0 || tx >= GRID_WIDTH || grid[y][tx] != EMPTY) break;
                        int target_density = count_neighbors(y, tx, DENSITY_RADIUS);
                        int target_near_density = count_neighbors(y, tx, NEAR_DENSITY_RADIUS);
                        int target_height = get_surface_height(tx);
                        float score = target_density + NEAR_DENSITY_FACTOR * target_near_density +
                                     HEIGHT_FACTOR * target_height;
                        if (score < best_score) {
                            best_target_x = tx;
                            best_score = score;
                            break;
                        }
                    }
                }

                if (best_target_x != x) {
                    grid[y][best_target_x] = WATER;
                    grid[y][x] = EMPTY;
                    active_cells[i].x = best_target_x;
                }
            }
        }
    }

    // Verify and correct particle count
    int actual_count = 0;
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x] == WATER) actual_count++;
        }
    }
    particle_count = actual_count;
}
