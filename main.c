#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>


#define HEIGHT 1000
#define WIDTH 1000
#define N 100
#define TITLE "Le jeu de la vie"
typedef struct Objects
{
    SDL_Rect **rects;
    uint8_t** matrix;
    uint8_t n;
} Objects;

void InitSDL();
void DestroySDL();
void InitGrid(int n, SDL_Color color);
void AddRectangle(Objects *objs, int x, int y);
SDL_Rect MakeRectangle(int x, int y, int w, int h);
void Render(Objects *objs);
void DeleteRectangle(Objects *objs, int x, int y);
int isObjectExistAtPos(Objects *objs, int x, int y);
void MoveRectangle(Objects *objs, int x, int y, int xNewPos, int yNewPos);
void reallocObjects(Objects *objs, int n);
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
    if(value != ""){
        printf("Error: %s\n and %s", value, SDL_GetError());
    } else {
        printf("Error: %s\n", SDL_GetError());
    }
}
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
const SDL_Color cellAlive = {39, 60, 117, 255};
const SDL_Color cellBorning = {76, 209, 55, 255};
const SDL_Color cellDying = {232, 65, 24, 255};
const SDL_Color oneGenCell = {251, 197, 49, 255};
int main()
{
    int isRunning = 1;
    Objects objs;
    InitSDL(&window, &renderer);
       
    objs.n = 2;
    
    objs.rects = (SDL_Rect **)malloc(sizeof(SDL_Rect *) * objs.n);
    objs.matrix = (uint8_t **)malloc(sizeof(uint8_t*) * objs.n);
    for (int i = 0; i < objs.n; i++)
    {
        objs.rects[i] = (SDL_Rect *)malloc(sizeof(SDL_Rect) * objs.n);
        objs.matrix[i] = (uint8_t *)malloc(sizeof(uint8_t) * objs.n);
        for(int j = 0; j < objs.n; j++){
            objs.matrix[i][j] = 0;
        }
    }
    
    AddRectangle(&objs, 1, 1);
    int x = 0; int y = 0; 
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
                    AddRectangle(&objs, 2, 2);
                    
                }
                Render(&objs);
            }
        }
    }

    cleanResources(window,renderer);
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
    if(SDL_CreateWindowAndRenderer(WIDTH,HEIGHT,SDL_WINDOW_SHOWN, &window, &renderer) != 0){
        PrintErrorLog("Error Window & Renderer on initialisation");
        cleanResources(NULL, NULL);
        exit(EXIT_FAILURE);
    }

    SDL_SetWindowTitle(window,"Le jeu de la vie");

}


void InitGrid(int n, SDL_Color color)
{   
    // (SIZE - 1) Permet que la dernière ligne soit parfaitement sur le bord
    int casesVertical = (HEIGHT - 1) / n; 
    int casesHorizontal = (WIDTH - 1) / n;
    int sumVertial = 0; int sumHorizontal = 0;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    do{
        if(SDL_RenderDrawLine(renderer, sumVertial, 0, sumVertial, HEIGHT)!=0){
            PrintErrorLog("RenderDrawLine");
            exit(EXIT_FAILURE);
        }
        sumVertial += casesVertical;
    }while(sumVertial < WIDTH);

    do{
        if(SDL_RenderDrawLine(renderer, 0, sumHorizontal, WIDTH, sumHorizontal) != 0)
        {
            PrintErrorLog("RenderDrawLine");
            exit(EXIT_FAILURE);
        }
        sumHorizontal += casesHorizontal;
    }while(sumHorizontal < HEIGHT);
}

void AddRectangle(Objects *objs, int x, int y)
{
    if((x > N - 1 || y > N - 1) && (x < 0 || y < 0)) return;
    int w = 0;
    int h = 0;
    
    SDL_GetWindowSize(window, &w, &h);
    int hS = h / N; int wS = w / N;
    if ((x * wS) > w || (y * hS) > h)
    {
        return;
    }
    int maxXorY = 0;
    if(x>y){
        maxXorY = x;
    } else if(x < y){
        maxXorY = y;
    } else {
        maxXorY = x;
    }

    reallocObjects(objs, maxXorY);
    SDL_Rect rect;
    rect.x = 0 + wS * x;
    rect.y = 0 + y * hS;
    rect.w = h / N;
    rect.h = w / N;
    objs->rects[x][y] = rect;
    objs->matrix[x][y] = 1;

}

void DeleteRectangle(Objects *objs, int x, int y)
{
    if(!isObjectExistAtPos(objs, x, y)) return;
    objs->rects[x][y] = (SDL_Rect){-1, -1, -1, -1};
    objs->matrix[x][y] = 0;
}

void MoveRectangle(Objects *objs, int x, int y, int xNewPos, int yNewPos){
    if(!isObjectExistAtPos(objs, x, y)) return;
    if(objs->matrix[xNewPos][yNewPos] == 1) return;
    if((xNewPos > N - 1|| yNewPos > N - 1) && (xNewPos < 0 || yNewPos < 0)) return;
    DeleteRectangle(objs, x, y);
    AddRectangle(objs, xNewPos, yNewPos);
}

int isObjectExistAtPos(Objects *objs, int x, int y){
    if(objs->matrix[x][y] == 1){
        return 1;
    } else {
        return 0;
    }
}

void Render(Objects *objs)
{
        
    SDL_SetRenderDrawColor(renderer, 26, 26, 26, 255);
    SDL_RenderClear(renderer);

    InitGrid(N, (SDL_Color){82, 82, 82, 255});

    SDL_SetRenderDrawColor(renderer, cellAlive.r, cellAlive.g, cellAlive.b, cellAlive.a);
    for (int i = 0; i < objs->n; i++)
    {
        for (int j = 0; j < objs->n; j++)
        {
            if (objs->matrix[i][j] == 1)
            {
                SDL_RenderFillRect(renderer,&objs->rects[i][j]);
                printf("Rectangle en: %i,%i\n",i,j);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void reallocObjects(Objects *objs, int n){
    int newValue = objs->n + n;
    printf("Log: ReallocObject: Old Value: %i, New Value %i\n", objs->n, newValue);
    
    SDL_Rect **ptrRect = (SDL_Rect **)realloc(objs->rects, newValue * sizeof(SDL_Rect *));
    u_int8_t **ptrInt = (uint8_t **)realloc(objs->matrix, newValue * sizeof(uint8_t *));
    
    if(ptrRect){objs->rects = ptrRect;}else{PrintErrorLog("On rects reallocate");}
    if(ptrInt){objs->matrix = ptrInt;}else{PrintErrorLog("On matrix reallocate");}


    for (int i = 0; i < newValue; i++)
    {
        SDL_Rect * rects = (SDL_Rect *)realloc(objs->rects[i],newValue * sizeof(SDL_Rect));
        uint8_t * matrix = (uint8_t *)realloc(objs->matrix[i],newValue * sizeof(uint8_t));
        
        if(rects){objs->rects[i] = rects;}else{PrintErrorLog("On rects(i) reallocate");}
        if(matrix){objs->matrix[i] = matrix;}else{PrintErrorLog("On matrix(i) reallocate");}
        for(int j = objs->n; j < newValue; j++){
            objs->matrix[i][j] = 0;
        }
    }
    objs->n += n;
    printf("Log: ReallocObject: Realloc Success\n");  

}

SDL_Rect MakeRectangle(int x, int y, int w, int h)
{
    return (SDL_Rect){x, y, w, h};
}
