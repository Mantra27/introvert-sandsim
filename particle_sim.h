
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#ifndef PARTICLE_SIM_H
#define PARTICLE_SIM_H

#define DEFAULT_GRID_WIDTH 300
#define DEFAULT_GRID_HEIGHT 300
#define CELL_SIZE 1

typedef enum {
    EMPTY = 0,
    WATER = 1,
    OBSTACLE = 2
} CellType;

typedef struct {
    int x;
    int y;
} ActiveCell;

//(set at runtime)
extern int GRID_WIDTH;
extern int GRID_HEIGHT;
extern CellType** grid;
extern int particle_count;
extern ActiveCell* active_cells;
extern int active_count;
extern int active_capacity;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *font;


void add_obstacle(int mouseX, int mouseY, int isDragging);
void add_water(int mouseX, int mouseY);
void update_simulation(void);
void init_grid(int width, int height);
void free_grid(void);

#endif