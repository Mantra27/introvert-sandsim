
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "particle_sim.h"


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;


void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void render() {

    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255); // dark blue-gray background
    SDL_RenderClear(renderer);

    // Render water particles
    if (grid != NULL) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (grid[y][x] == WATER) {
                // Use a nice blue color for water
                SDL_SetRenderDrawColor(renderer, 64, 164, 223, 255);
                SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            } else if (grid[y][x] == OBSTACLE) {
                // Use a brown/gray color for obstacles
                SDL_SetRenderDrawColor(renderer, 120, 100, 80, 255);
                SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
        }
    }

    // Display counts
    if (font) {
        char text[64];
        sprintf(text, "Particles: %d", particle_count);
        render_text(renderer, font, text, 10, 10);
        render_text(renderer, font, "Left Click: Add Water | Right Click: Add Obstacle", 10, 35);
    }

    SDL_RenderPresent(renderer);
}

void cleanup() {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    free_grid();
}

// static SDL_Window *window = NULL;
// static SDL_Renderer *renderer = NULL;
// static TTF_Font *font = NULL;
static int lastMouseX = -1;
static int lastMouseY = -1;


int user_GRID_WIDTH = DEFAULT_GRID_WIDTH;
int user_GRID_HEIGHT = DEFAULT_GRID_HEIGHT;
int user_GRAVITY_STEPS = 6;
int user_PARTICLE_SPAWN_RADIUS = 10;
int user_PARTICLE_GENERATION_RATE = 300;
int user_SPREAD_ITERATIONS = 3;

void cleanup(void);
int init_sdl(int width, int height);
void render(void);
void cleanup();
void init_simulation();
int init_sdl(int width, int height);
void init_grid(int width, int height);
void free_grid(void);

// --- Ask for simulation parameters before SDL starts ---
void get_simulation_config() {
    printf("Enter grid width (default 300): ");
    scanf("%d", &user_GRID_WIDTH);
    printf("Enter grid height (default 300): ");
    scanf("%d", &user_GRID_HEIGHT);
    printf("Enter gravity steps (default 6): ");
    scanf("%d", &user_GRAVITY_STEPS);
    printf("Enter particle spawn radius (default 10): ");
    scanf("%d", &user_PARTICLE_SPAWN_RADIUS);
    printf("Enter particle generation rate (default 300): ");
    scanf("%d", &user_PARTICLE_GENERATION_RATE);
    printf("Enter spread iterations (default 3): ");
    scanf("%d", &user_SPREAD_ITERATIONS);

    if (user_GRID_WIDTH <= 0) user_GRID_WIDTH = 300;
    if (user_GRID_HEIGHT <= 0) user_GRID_HEIGHT = 300;
    if (user_GRAVITY_STEPS <= 0) user_GRAVITY_STEPS = 6;
    if (user_PARTICLE_SPAWN_RADIUS <= 0) user_PARTICLE_SPAWN_RADIUS = 10;
    if (user_PARTICLE_GENERATION_RATE <= 0) user_PARTICLE_GENERATION_RATE = 300;
    if (user_SPREAD_ITERATIONS <= 0) user_SPREAD_ITERATIONS = 3;
}

//Reset grid
void reset_simulation_grid(int newWidth, int newHeight) {
    printf("Window resized to %dx%d, resetting simulation.\n", newWidth, newHeight);
    if (grid != NULL) {
        init_grid(newWidth, newHeight);
    }
}

int init_sdl(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("SDL init failed: %s\n", SDL_GetError());
        return -1;
    }
    window = SDL_CreateWindow(
        "2D Fluid Simulation",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return -1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        return -1;
    }
    font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Courier New.ttf", 20);
    if (!font) font = TTF_OpenFont("arial.ttf", 20);
    if (!font) {
        printf("Font loading failed: %s\n", TTF_GetError());
        return -1;
    }
    return 0;
}

void init_simulation() {
    if (grid == NULL) {
        init_grid(GRID_WIDTH, GRID_HEIGHT);
        return;
    }
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            grid[y][x] = EMPTY;
    particle_count = 0;
}

int main() {
    // Ask user for simulation parameters
    get_simulation_config();

    // Initialize grid with user-specified dimensions
    init_grid(user_GRID_WIDTH, user_GRID_HEIGHT);

    if (init_sdl(user_GRID_WIDTH, user_GRID_HEIGHT) != 0) {
        cleanup();
        return -1;
    }

    srand(time(NULL));
    init_simulation();

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_RESIZED) {
                reset_simulation_grid(event.window.data1, event.window.data2);
            }

            if (event.type == SDL_MOUSEMOTION) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                    add_water(x, y);
                }
                if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                    add_obstacle(x, y, 1);
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                add_water(x, y);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                lastMouseX = x;
                lastMouseY = y;
                add_obstacle(x, y, 0);
            }
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT) {
                lastMouseX = -1;
                lastMouseY = -1;
            }
        }

        update_simulation();
        render();
        SDL_Delay(16);
    }

    cleanup();
    return 0;
}
