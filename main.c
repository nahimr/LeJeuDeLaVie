#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define HEIGHT 1000
#define WIDTH 1000
#define N 100
#define TITLE "Le jeu de la vie"

enum TYPE_OF_LIFE
{
    UNDEFINED = 0, // Non instancié
    ALIVE = 1,     // Cellules en cours de vie
    APPEARING = 2, // Cellules naissantes
    DYING = 3,     // Cellules Mourantes
    ONEGENONLY = 4 // Cellules ne vivant qu’une seule génération
};

typedef struct Life
{
    enum TYPE_OF_LIFE type;
    Uint8 neighbourAlive; // Voisins en vie € {0,...8}
} Life;

typedef struct Objects
{
    SDL_Rect **rects; // Affichage Rect
    Life **matrix;    // Vies € {0,1,2,3,4}
    Uint8 n;          // n = max(i,j)
} Objects;

void InitSDL();
void DestroySDL();
void InitGrid(int n, SDL_Color color);
void AddRectangle(Objects *objs, int x, int y, enum TYPE_OF_LIFE type);
SDL_Rect MakeRectangle(int x, int y, int w, int h);
void Render(Objects *objs);
void ChangeRectangle(Objects *objs, int x, int y, enum TYPE_OF_LIFE type);
int isObjectNotNullAtPos(Objects *objs, int x, int y);
void UpdateNeighbourCount(Objects *objs, int x, int y);
void LimitFPS(Uint32 n);
void cleanResources(SDL_Window *w, SDL_Renderer *r)
{
    if (r != NULL)
    {
        SDL_DestroyRenderer(r);
    }
    else if (w != NULL)
    {
        SDL_DestroyWindow(w);
    }
}
void PrintErrorLog(const char *value)
{
    if (value != "")
    {
        printf("Error: %s\n and %s", value, SDL_GetError());
    }
    else
    {
        printf("Error: %s\n", SDL_GetError());
    }
}
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
const Uint8 grid = 1; const Uint8 debug = 0;
const SDL_Color gridColor = {82, 82, 82, 255};
const SDL_Color cellAlive = {200, 200, 200, 255};
const SDL_Color cellAppearing = {76, 209, 55, 255};
const SDL_Color cellDying = {232, 65, 24, 255};
const SDL_Color oneGenCell = {251, 197, 49, 255};
int main()
{
    int isRunning = 1; int isPaused = 1; Uint8 speed = 10; Uint16 generation = 0;
    Objects objs;
    InitSDL(&window, &renderer);

    objs.n = N + 1;

    objs.rects = (SDL_Rect **)calloc(objs.n, sizeof(SDL_Rect *));
    objs.matrix = (Life **)calloc(objs.n, sizeof(Life *));
    for (int i = 0; i < objs.n; i++)
    {
        objs.rects[i] = (SDL_Rect *)calloc(objs.n, sizeof(SDL_Rect));
        objs.matrix[i] = (Life *)calloc(objs.n, sizeof(Life));
        for (int j = 0; j < objs.n; j++)
        {
            AddRectangle(&objs,i,j,UNDEFINED);
        }
    }

    int x = 10; int y = 10;

    ChangeRectangle(&objs, x + 3, y + 4, ALIVE);
    ChangeRectangle(&objs, x + 3, y + 5, ALIVE);
    ChangeRectangle(&objs, x + 3, y + 6, ALIVE);
    ChangeRectangle(&objs, x + 3, y + 7, ALIVE);
    ChangeRectangle(&objs, x + 3, y + 8, ALIVE);
    ChangeRectangle(&objs, x + 5, y + 4, ALIVE);
    ChangeRectangle(&objs, x + 5, y + 8, ALIVE);
    ChangeRectangle(&objs, x + 7, y + 4, ALIVE);
    ChangeRectangle(&objs, x + 7, y + 5, ALIVE);
    ChangeRectangle(&objs, x + 7, y + 6, ALIVE);
    ChangeRectangle(&objs, x + 7, y + 7, ALIVE);
    ChangeRectangle(&objs, x + 7, y + 8, ALIVE);

    Render(&objs);
    while (isRunning)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = 0;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_SPACE)
                {
                    isPaused = !isPaused;
                    if(isPaused) 
                        SDL_Log("Game Paused");
                    else{
                         SDL_Log("Game Resume");
                    }
                } else if(event.key.keysym.sym == SDLK_PLUS 
                    || event.key.keysym.sym == SDLK_p){
                    Uint8 increment = 10;
                    if((speed + increment) <= 100){
                        speed+= increment;
                        SDL_Log("Speed UP +10");
                    }
                    
                } else if(event.key.keysym.sym == SDLK_MINUS 
                    || event.key.keysym.sym == SDLK_m){
                        
                    Uint8 increment = 10;
                    if((speed - increment) > 0){
                        speed-= increment;
                        SDL_Log("Speed Down -10");
                    }
                }
            }
        }
        LimitFPS(1000/speed);
        
        if(!isPaused){
            SDL_Log("Generation: %i", generation);
            for(int i = 0; i < objs.n; i++){
                for(int j = 0; j< objs.n; j++){
                    if(objs.matrix[i][j].neighbourAlive == 3){
                        if(objs.matrix[i][j].type == UNDEFINED){
                            ChangeRectangle(&objs,i,j,APPEARING);
                        }
                    } else if(objs.matrix[i][j].neighbourAlive < 2 || objs.matrix[i][j].neighbourAlive > 3){
                        if(objs.matrix[i][j].type == ALIVE){
                            ChangeRectangle(&objs,i,j,DYING);
                        }
                    }
                }
            }
            Render(&objs);

            for(int i = 0; i < objs.n; i++){
                for(int j = 0; j< objs.n; j++){                    
                    if(objs.matrix[i][j].type == APPEARING){
                        ChangeRectangle(&objs,i,j,ALIVE);
                    }
                    if(objs.matrix[i][j].type == DYING){
                        ChangeRectangle(&objs,i,j, UNDEFINED);
                    }
                } 
            }
            Render(&objs);
            generation++;
        }
    }

    cleanResources(window, renderer);
    SDL_Quit();
    exit(EXIT_SUCCESS);
}


void InitSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Init failed!!");
        exit(EXIT_FAILURE);
    }
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) != 0)
    {
        PrintErrorLog("Error Window & Renderer on initialisation");
        cleanResources(NULL, NULL);
        exit(EXIT_FAILURE);
    }

    SDL_SetWindowTitle(window, "Le jeu de la vie");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void InitGrid(int n, SDL_Color color)
{
    // (SIZE - 1) Permet que la dernière ligne soit parfaitement sur le bord
    int casesVertical = HEIGHT / n;
    int casesHorizontal = WIDTH / n;
    int sumVertial = 0;
    int sumHorizontal = 0;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    do
    {
        if (SDL_RenderDrawLine(renderer, sumVertial, 0, sumVertial, HEIGHT) != 0)
        {
            PrintErrorLog("RenderDrawLine");
            exit(EXIT_FAILURE);
        }
        sumVertial += casesVertical;
    } while (sumVertial < WIDTH);

    do
    {
        if (SDL_RenderDrawLine(renderer, 0, sumHorizontal, WIDTH, sumHorizontal) != 0)
        {
            PrintErrorLog("RenderDrawLine");
            exit(EXIT_FAILURE);
        }
        sumHorizontal += casesHorizontal;
    } while (sumHorizontal < HEIGHT);
}

void UpdateNeighbourCount(Objects *objs, int x, int y)
{
    if (!isObjectNotNullAtPos(objs, x, y))
    {
        return;
    }
    Uint8 nbTemp = 0;
    Uint8 n = objs->n;
    if ((x-1) > 0 && (y-1) > 0)
    {
        if (objs->matrix[x - 1][y - 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((y-1 > 0))
    {
        if (objs->matrix[x][y - 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((x+1) < N && (y-1) > 0)
    {
        if (objs->matrix[x + 1][y - 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((x+1) < N)
    {
        if (objs->matrix[x + 1][y].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((x+1) < N && (y+1) < N)
    {
        if (objs->matrix[x + 1][y + 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((y+1) < N)
    {
        if (objs->matrix[x][y + 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((x-1) > 0 && (y+1) < N)
    {
        if (objs->matrix[x - 1][y + 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((x-1) > 0)
    {
        if (objs->matrix[x - 1][y].type != UNDEFINED)
        {
            nbTemp++;
        }
    }
    objs->matrix[x][y].neighbourAlive = nbTemp;
}

void AddRectangle(Objects *objs, int x, int y, enum TYPE_OF_LIFE type)
{
    if (x >= N || y >= N || x < 0 || y < 0)
    {
        return;
    }
    const int hS = (HEIGHT / N);
    const int wS = (WIDTH / N);
    if ((x * wS) > WIDTH || (y * hS) > HEIGHT)
    {
        return;
    }
    SDL_Rect rect;
    rect.x = hS * x;
    rect.y = y * wS;
    rect.w = wS;
    rect.h = hS;

    objs->rects[x][y] = rect;
    objs->matrix[x][y].type = type;
}

void ChangeRectangle(Objects *objs, int x, int y, enum TYPE_OF_LIFE type)
{
    objs->matrix[x][y].type = type;
}

int isObjectNotNullAtPos(Objects *objs, int x, int y)
{
    //if (objs->matrix[x][y].type != UNDEFINED)
    if (x < 0 || y < 0 || x > N || y > N)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void Render(Objects *objs)
{

    SDL_SetRenderDrawColor(renderer, 26, 26, 26, 255);
    SDL_RenderClear(renderer);

    if(grid){
        InitGrid(N, gridColor);
    }
        
    if(debug){
        printf("=========\n");
    }
        
    for (int i = 0; i < objs->n; i++)
    {
        for (int j = 0; j < objs->n; j++)
        {
            switch (objs->matrix[i][j].type)
            {
                case UNDEFINED:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    
                    break;
                case ALIVE:
                    SDL_SetRenderDrawColor(renderer, cellAlive.r, cellAlive.g, cellAlive.b, cellAlive.a);
                    break;
                case APPEARING:
                    SDL_SetRenderDrawColor(renderer, cellAppearing.r, cellAppearing.g, cellAppearing.b, cellAppearing.a);
                    break;
                case DYING:
                    SDL_SetRenderDrawColor(renderer, cellDying.r, cellDying.g, cellDying.b, cellDying.a);
                    break;
                case ONEGENONLY:
                    SDL_SetRenderDrawColor(renderer, oneGenCell.r, oneGenCell.g, oneGenCell.b, oneGenCell.a);
                default:
                   
                    break;
            }

            if(objs->matrix[i][j].type == UNDEFINED){
                SDL_RenderDrawRect(renderer, &objs->rects[i][j]);
            } else {
                SDL_RenderFillRect(renderer, &objs->rects[i][j]);
            }
            
            UpdateNeighbourCount(objs, i, j);
            if(debug){
                printf("Type of Life: %i\n",objs->matrix[i][j].type);
                printf("Life en: %i,%i\n", i, j);
                printf("{x:%i,y%i,w%i,h%i}\n", objs->rects[i][j].x, objs->rects[i][j].y, objs->rects[i][j].w, objs->rects[i][j].h);
                printf("NeigboursCount:%i\n", objs->matrix[i][j].neighbourAlive);
            }

        }
    }
    if(debug){
        printf("=========\n");
    }
    
    SDL_RenderPresent(renderer);
    
}

SDL_Rect MakeRectangle(int x, int y, int w, int h)
{
    return (SDL_Rect){x, y, w, h};
}

void LimitFPS(Uint32 n){
    Uint32 time = SDL_GetTicks();
    if ((SDL_GetTicks() - time) < n) {
        SDL_Delay(n);
    }
}