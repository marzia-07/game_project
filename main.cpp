#include <bits/stdc++.h>
#include <SDL2/SDL.h>

using namespace std;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int GRID_SIZE = 20;
const int INITIAL_LENGTH = 5;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

struct Snake {
    vector<pair<int, int>> body;
    char direction;
};

struct Food {
    int x, y;
};

Snake snake;
Food food;

void initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    snake.body.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
    snake.direction = 'R';

    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
}

void close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void generateFood() {
    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
}

void update() {
    pair<int, int> newHead = snake.body.front();

    switch (snake.direction) {
        case 'U':
            newHead.second -= GRID_SIZE;
            break;
        case 'D':
            newHead.second += GRID_SIZE;
            break;
        case 'L':
            newHead.first -= GRID_SIZE;
            break;
        case 'R':
            newHead.first += GRID_SIZE;
            break;
    }

    snake.body.insert(snake.body.begin(), newHead);

    if (newHead.first == food.x && newHead.second == food.y) {
        generateFood();
    } else {
        snake.body.pop_back();
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 10, 100, 200, 255);
    SDL_RenderClear(renderer);

    // Render Snake
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (const auto& segment : snake.body) {
        SDL_Rect rect = {segment.first, segment.second, GRID_SIZE, GRID_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }

    // Render Food
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect foodRect = {food.x, food.y, GRID_SIZE, GRID_SIZE};
    SDL_RenderFillRect(renderer, &foodRect);

    SDL_RenderPresent(renderer);
}
#ifdef _WIN32
int SDL_main(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
 initialize();

    SDL_Event e;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (snake.direction != 'D') snake.direction = 'U';
                        break;
                    case SDLK_DOWN:
                        if (snake.direction != 'U') snake.direction = 'D';
                        break;
                    case SDLK_LEFT:
                        if (snake.direction != 'R') snake.direction = 'L';
                        break;
                    case SDLK_RIGHT:
                        if (snake.direction != 'L') snake.direction = 'R';
                        break;
                }
            }
        }

        update();
        render();

        SDL_Delay(200);
    }

    close();
    return 0;
}