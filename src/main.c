// Breakout SDL2 by Radon Therapy
// 2023-06-08 - 2023-06-15

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
#define BALL_SPEED 5

#define PADDLE_W 120
#define PADDLE_H 20
#define PADDLE_SPEED 5

#define COUNT_COLUMN 10
#define COUNT_ROW 10

// - SDL variables
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;

// - Game variables
bool gExit = false;
bool gFinished = false;
bool gWin = false;

typedef struct {
    SDL_Rect rect;
    int dx, dy;
} Object;

Object player, ball;

typedef struct {
    SDL_Rect rect;
    bool visible;
} Brick;

Brick bricks[COUNT_ROW][COUNT_COLUMN];

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

bool check_collision(SDL_Rect rect1, SDL_Rect rect2) {
    if (rect1.x < rect2.x + rect2.w &&
        rect1.x + rect1.w > rect2.x &&
        rect1.y < rect2.y + rect2.h &&
        rect1.y + rect1.h > rect2.y) {
        return true;
    }
    return false;
}

void breakout_init()
{
    // Game variables
    gFinished = false;
    gWin = false;

    // Player and ball
    player = (Object) {(SDL_Rect){(WINDOW_WIDTH - PADDLE_W) / 2, WINDOW_HEIGHT - PADDLE_H - 20, PADDLE_W, PADDLE_H}, 0, 0};
    ball = (Object) {(SDL_Rect){(WINDOW_WIDTH - BALL_SIZE) / 2, WINDOW_HEIGHT - BALL_SIZE - PADDLE_H - 20 - 20, BALL_SIZE, BALL_SIZE}, BALL_SPEED, -BALL_SPEED};

    // Bricks
    int brickWidth = WINDOW_WIDTH / COUNT_COLUMN;
    int brickHeight = (WINDOW_HEIGHT - 400) / COUNT_ROW;

    for (int i = 0; i < COUNT_ROW; i++) {
        for (int j = 0; j < COUNT_COLUMN; j++) {
            bricks[i][j].rect.x = j * brickWidth + 2;
            bricks[i][j].rect.y = i * brickHeight + 2;
            bricks[i][j].rect.w = brickWidth;
            bricks[i][j].rect.h = brickHeight;
            bricks[i][j].visible = true;
        }
    }
}

void handle_input()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                gExit = true;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_a:
                        player.dx = -PADDLE_SPEED;
                        break;

                    case SDLK_RIGHT:
                    case SDLK_d:
                        player.dx = PADDLE_SPEED;
                        break;

                    case SDLK_SPACE:
                        breakout_init();
                        break;

                    default:
                        break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_a:
                    case SDLK_RIGHT:
                    case SDLK_d:
                        player.dx = 0;
                        break;

                    default:
                        break;
                }

            default:
                break;
        }
    }
}

void game_update()
{
    if (gFinished) return;

    // Move paddle
    player.rect.x += player.dx;
    player.rect.y += player.dy;

    if (player.rect.x + player.rect.w > WINDOW_WIDTH) {
        player.rect.x = WINDOW_WIDTH - player.rect.w;
    } else if (player.rect.x < 0) {
        player.rect.x = 0;
    }

    // Move ball
    ball.rect.x += ball.dx;
    ball.rect.y += ball.dy;

    ball.dx = ball.rect.x < 0 || ball.rect.x > WINDOW_WIDTH - ball.rect.w ? -ball.dx : ball.dx;
    ball.dy = ball.rect.y < 0 || ball.rect.y > WINDOW_HEIGHT - ball.rect.h ? -ball.dy : ball.dy;

    ball.dy = check_collision(player.rect, ball.rect) ? -ball.dy : ball.dy;

    // Game finish win/lose
    if (ball.rect.y < 0) {
        gFinished = true;
        gWin = true;
    }

    if (ball.rect.y > WINDOW_HEIGHT - ball.rect.h) {
        gFinished = true;
        gWin = false;
    }

    // Bricks
    for (int i = 0; i < COUNT_ROW; i++) {
        for (int j = 0; j < COUNT_COLUMN; j++) {
            if (bricks[i][j].visible && check_collision(ball.rect, bricks[i][j].rect)) {
                bricks[i][j].visible = false;
                ball.dy = -ball.dy;
            }
        }
    }
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

    // Bricks
    for (int i = 0; i < COUNT_ROW; i++) {
        for (int j = 0; j < COUNT_COLUMN; j++) {
            if (bricks[i][j].visible) SDL_RenderFillRect(gRenderer, &bricks[i][j].rect);
        }
    }

    // Win/lose text
    SDL_Color textColor = { 255, 255, 255, 255 };

    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, "You win! Press <SPACE> to restart.", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    SDL_Rect textRect = (SDL_Rect) {WINDOW_WIDTH / 2 - 20 - textSurface->w, WINDOW_HEIGHT / 2, textSurface->w, textSurface->h };

    // Draw win text
    if (gFinished) {
        if (gWin) {
            SDL_RenderCopy(gRenderer, textTexture, NULL, &textRect);
        } else {
            textSurface = TTF_RenderText_Solid(gFont, "You lose. Press <SPACE> to restart.", textColor);
            textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
            textRect = (SDL_Rect) {WINDOW_WIDTH / 2 + 20, WINDOW_HEIGHT / 2, textSurface->w, textSurface->h };
            SDL_RenderCopy(gRenderer, textTexture, NULL, &textRect);
        }
    }
    

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);


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