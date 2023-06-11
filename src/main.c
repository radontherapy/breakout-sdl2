// Breakout SDL2 by Radon Therapy
// 2023-06-08 - 

// Header files
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Variables and constants

// - Window settings
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FRAME_DELAY_MS (1000 / 60) // 60 FPS

// - Game settings
#define BALL_SIZE 20
#define BALL_SPEED 2

#define PADDLE_W 120
#define PADDLE_H 20
#define PADDLE_SPEED 3

// - SDL variables
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;

// - Game variables
bool gExit = false;

typedef struct {
    SDL_Rect rect;
    int dx, dy;
} Object;

Object player, ball;

// Functions
bool game_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("[error] Couldn't initialize SDL2: %s\n", SDL_GetError());
        return false;
    }

    if (TTF_Init() != 0) {
        printf("[error] Couldn't initialize SDL2_TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return false;
    }

    gWindow = SDL_CreateWindow("Breakout", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!gWindow) {
        printf("[error] Couldn't create window: %s\n", SDL_GetError());
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!gRenderer) {
        printf("[error] Couldn't create renderer: %s\n", SDL_GetError());
        return false;
    }

    gFont = TTF_OpenFont("assets/BebasNeue-Regular.ttf", 28);
    if (!gFont) {
        printf("[error] Couldn't load font: %s\n", TTF_GetError());
        return false;
    }

    return true;
}

void game_exit()
{
    TTF_CloseFont(gFont);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    TTF_Quit();
    SDL_Quit();
}

void breakout_init()
{
    player = (Object) {(SDL_Rect){(WINDOW_WIDTH - PADDLE_W) / 2, WINDOW_HEIGHT - PADDLE_H - 20, PADDLE_W, PADDLE_H}, PADDLE_SPEED, PADDLE_SPEED};
    ball = (Object) {(SDL_Rect){(WINDOW_WIDTH - BALL_SIZE) / 2, WINDOW_HEIGHT - BALL_SIZE - PADDLE_H - 20 - 20, BALL_SIZE, BALL_SIZE}, BALL_SPEED, BALL_SPEED};
}

void handle_input()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                gExit = true;
                break;

            default:
                break;
        }
    }
}

void game_update()
{
}

void game_render()
{
    // Clean up screen
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    // White
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

    // Paddle
    SDL_RenderFillRect(gRenderer, &player.rect);

    // Ball
    SDL_RenderFillRect(gRenderer, &ball.rect);

    // Render
    SDL_RenderPresent(gRenderer);
}

// Main function
int main(int argc, char* argv[])
{
    if (!game_init()) {
        return 1;
    }

    // Initialize game
    breakout_init();

    // Initialize fps cap
    Uint32 frameStart, frameTime;

    while(!gExit) {
        // Set start time for fps cap
        frameStart = SDL_GetTicks();

        handle_input(); // Input handling
        game_update(); // Handle game dynamics

        // Rendering
        game_render();

        // Delay
        frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY_MS > frameTime) {
            SDL_Delay(FRAME_DELAY_MS - frameTime);
        }
    }

    game_exit();
    return 0;
}