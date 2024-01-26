#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

using namespace std;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int GRID_SIZE = 20;
const int INITIAL_LENGTH = 5;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font= nullptr;

struct Snake {
    vector<pair<int, int>> body;
    char direction;
};

struct Food {
    int x, y;
};
struct BonusFood {
    int x, y;
    bool active;
};
struct Obstacle
{
  int x,y;
};
int score=0;
int foodEaten=0;
Uint32 bonusFoodTimer=0;
bool gameOver=false;
Snake snake;
Food food;
BonusFood bonusFood;
Obstacle obstacle1,obstacle2;

void initialize() {
    SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	font=TTF_OpenFont("marzia.ttf",24);

    snake.body.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
    snake.direction = 'U';

    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
	bonusFood.active=false;
	obstacle1.x=SCREEN_WIDTH/3;
	obstacle1.y=100;
	obstacle2.x=2*SCREEN_WIDTH/3;
	obstacle2.y=100;
	
}

void close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	TTF_Quit();
    SDL_Quit();
}

void generateFood() {
    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
}
bool foodObstacles()
{
    if (food.x < obstacle1.x + GRID_SIZE/2 &&
        food.x + GRID_SIZE > obstacle1.x &&
        food.y< obstacle1.y + SCREEN_HEIGHT - 200 &&
        food.y + GRID_SIZE > obstacle1.y) {
            return false;
    }

    if (food.x < obstacle2.x + GRID_SIZE/2 &&
        food.x + GRID_SIZE > obstacle2.x &&
        food.y< obstacle2.y + SCREEN_HEIGHT - 200 &&
        food.y + GRID_SIZE > obstacle2.y) {
            return false;
    }
    return true;

}

void updateBonusFoodTimer()
{
	if(bonusFood.active)
	{
		Uint32 currentTime=SDL_GetTicks();
		if(currentTime-bonusFoodTimer>=3000)
		{
			bonusFood.active=false;
			bonusFoodTimer=0;
		}
	}
	else
	{
		bonusFoodTimer=SDL_GetTicks();
	}
}
void generateBonusFood() {
    bonusFood.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
    bonusFood.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
    bonusFood.active = true;
}

void updateScore()
{
	if(bonusFood.active)
	score+=30;
	else
	score+=10;
	foodEaten++;
	if(foodEaten%3==0)
     generateBonusFood();
}
void renderScore()
{
	SDL_Color textColor={255,255,255,255};
	string scoreText="Score: "+ to_string(score);
	SDL_Surface* surface=TTF_RenderText_Solid(font,scoreText.c_str(),textColor);
	SDL_Texture* texture=SDL_CreateTextureFromSurface(renderer,surface);
	SDL_Rect scoreRect={10,10,surface->w,surface->h};
	SDL_RenderCopy(renderer,texture,NULL,&scoreRect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}
void renderBonusFood()
{
	if(bonusFood.active)
	{
		SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
        SDL_Rect bonusFoodRect = {bonusFood.x, bonusFood.y, GRID_SIZE*2, GRID_SIZE*2};
        SDL_RenderFillRect(renderer, &bonusFoodRect);
    }
}
void renderObstacles() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect obstacleRect1 = {obstacle1.x, obstacle1.y, GRID_SIZE/2, SCREEN_HEIGHT-200};
    SDL_RenderFillRect(renderer, &obstacleRect1);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect obstacleRect2 = {obstacle2.x, obstacle2.y, GRID_SIZE/2, SCREEN_HEIGHT-200};
    SDL_RenderFillRect(renderer, &obstacleRect2);
} 

void renderGameOver()
{
	SDL_Color textColor={255,255,255,255};
	string gameOverText="Game Over! Score: "+to_string(score);
	SDL_Surface* surface=TTF_RenderText_Solid(font,gameOverText.c_str(),textColor);
	SDL_Texture* texture=SDL_CreateTextureFromSurface(renderer,surface);
	SDL_Rect gameOverRect={100,200,surface->w,surface->h};
	SDL_RenderCopy(renderer,texture,NULL,&gameOverRect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void update() {
	if(gameOver) return;
    pair<int, int> newHead = snake.body.front();

    switch (snake.direction) {
        case 'U':
            newHead.second -= GRID_SIZE;
            if (newHead.second < 0)  // If snake moves out of top side
                newHead.second = SCREEN_HEIGHT - GRID_SIZE; 
            break;
        case 'D':
            newHead.second += GRID_SIZE;
             if (newHead.second >= SCREEN_HEIGHT)  // If snake moves out of bottom side
                newHead.second = 0;
            break;
        case 'L':
            newHead.first -= GRID_SIZE;
            if (newHead.first < 0)  // If snake moves out of left side
                newHead.first = SCREEN_WIDTH - GRID_SIZE;
            break;
        case 'R':
            newHead.first += GRID_SIZE;
            if (newHead.first >= SCREEN_WIDTH)  // If snake moves out of right side
                newHead.first = 0; 
            break;
    }

	
	for(size_t i=1;i<snake.body.size();i++)
	{
		if(newHead.first==snake.body[i].first && newHead.second==snake.body[i].second)
		{
			gameOver=true;
			renderGameOver();
			break;
		}
	}
	
// Check collision with obstacles
    if (newHead.first < obstacle1.x + GRID_SIZE/2 &&
        newHead.first + GRID_SIZE > obstacle1.x &&
        newHead.second < obstacle1.y + SCREEN_HEIGHT - 200 &&
        newHead.second + GRID_SIZE > obstacle1.y) {
            gameOver = true;
            renderGameOver();
            return;
    }

    if (newHead.first < obstacle2.x + GRID_SIZE/2 &&
        newHead.first + GRID_SIZE > obstacle2.x &&
        newHead.second < obstacle2.y + SCREEN_HEIGHT - 200 &&
        newHead.second + GRID_SIZE > obstacle2.y) {
            gameOver = true;
            renderGameOver();
            return;
    }
   
	updateBonusFoodTimer();

    if (!gameOver) {
        snake.body.insert(snake.body.begin(), newHead);

        if (newHead.first == food.x && newHead.second == food.y) {
            while(1){
            if(foodObstacles()){
            generateFood();
            break;
            }

            }
            updateScore();
        } 
		else if(bonusFood.active && newHead.first==bonusFood.x && newHead.second==bonusFood.y)
		{
			updateScore();
			bonusFood.active=false;
		}
		else {
            snake.body.pop_back();
        }
    }
}

void render() {
	if(gameOver)
    {SDL_SetRenderDrawColor(renderer, 10, 100, 200, 255);
    SDL_RenderClear(renderer);
	renderGameOver();
		SDL_RenderPresent(renderer);
		SDL_Delay(2000);
		return ;
	}

    // Render Snake
	else
	{

	  SDL_SetRenderDrawColor(renderer, 10, 100, 200, 255);
      SDL_RenderClear(renderer);
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      for (const auto& segment : snake.body) {
        SDL_Rect rect = {segment.first, segment.second, GRID_SIZE, GRID_SIZE};
        SDL_RenderFillRect(renderer, &rect);
      }
      // Render Food
      SDL_SetRenderDrawColor(renderer,  0,255, 0, 255);
       SDL_Rect foodRect = {food.x, food.y, GRID_SIZE, GRID_SIZE};
       SDL_RenderFillRect(renderer, &foodRect);

      SDL_RenderPresent(renderer);
	
	   renderBonusFood();
	  renderObstacles();
	   renderScore();
	
	  SDL_RenderPresent(renderer);
	}
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
            } 
			else if (e.type == SDL_KEYDOWN) 
			{
				if(gameOver && e.key.keysym.sym==SDLK_RETURN)
				{
					gameOver=false;
					score=0;
					snake.body.clear();
					snake.body.push_back({SCREEN_WIDTH/2,SCREEN_HEIGHT/2});
					snake.direction='R';
					generateFood();
				}
			    else{
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
        }

        update();
        render();

        SDL_Delay(200);
    }

    close();
    return 0;
}