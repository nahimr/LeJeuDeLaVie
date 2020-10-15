#include <stdio.h> 
#include <stdlib.h> 
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> 
#include <SDL2/SDL_timer.h> 
#define HEIGHT 1000
#define WIDTH 1000
#define N 20
typedef struct Objects{
    SDL_Rect ** rects;
    int number;
} Objects;

void DestroySDL(SDL_Renderer * renderer, SDL_Window * window);
void InitGrid(SDL_Renderer * renderer, int n, SDL_Color color);
void AddRectangle(SDL_Renderer * renderer, SDL_Window * window, Objects * objs, int x, int y, int s);
SDL_Rect MakeRectangle(int x, int y, int w, int h);
void Update(SDL_Renderer * renderer, SDL_Window * window, Objects * objs);
void DeleteRectangle(Objects * objs, int x, int y);
int main() 
{ 
    int isRunning = 1;
    Objects objs;
    objs.rects = (SDL_Rect**) malloc(sizeof(SDL_Rect*) * N);
    for(int i = 0; i < N; i++){
        objs.rects[i] = (SDL_Rect*) malloc(sizeof(SDL_Rect) * N);
    }

    SDL_Window * window = NULL;
    SDL_Renderer * renderer = NULL;
    
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        SDL_Log("Init failed!!");
    }

    if(SDL_CreateWindowAndRenderer(WIDTH,HEIGHT,SDL_WINDOW_SHOWN, &window, &renderer))
        SDL_Log("Error Window");
   

    SDL_SetWindowTitle(window, "Le jeu de la vie");
   
    for(int i = 0; i < 10; i++){
        AddRectangle(renderer,window, &objs, i,i, HEIGHT/20);
    }
    
    DeleteRectangle(&objs, 0,0);
    
    Update(renderer, window, &objs);

    SDL_RenderPresent(renderer); 
    
    SDL_Event event;
    while(isRunning){
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT){
                isRunning = 0;
            }
        }
        
        SDL_UpdateWindowSurface(window);
    }

    DestroySDL(renderer, window);
    return EXIT_SUCCESS; 
} 

void DestroySDL(SDL_Renderer * renderer, SDL_Window * window){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(EXIT_SUCCESS);
}

void InitGrid(SDL_Renderer * renderer,int n,  SDL_Color color){
    int casesVertical = HEIGHT / n;
    int casesHorizontal = WIDTH / n;

    for(int i = 0; i < casesVertical; i++){
        SDL_RenderDrawLine(renderer, i * casesVertical, 0, i*casesVertical, HEIGHT);
        SDL_SetRenderDrawColor(renderer, color.r,color.g,color.b,color.a);
    }

    for(int i = 0; i < casesHorizontal; i++){
        SDL_RenderDrawLine(renderer,0,i*casesHorizontal,WIDTH,i*casesHorizontal);
        SDL_SetRenderDrawColor(renderer, color.r,color.g,color.b,color.a);
    }
}

void AddRectangle(SDL_Renderer * renderer, SDL_Window * window, Objects * objs, int x, int y, int s){
    int w = 0; int h = 0;
    SDL_GetWindowSize(window,&w, &h);
    if(x*s > w || y*s > h){ return; }
    SDL_Rect rect;
    rect.x = 0 + s * x;
    rect.y = 0 + y * s;
    rect.w = s;
    rect.h = s;
    objs->rects[x][y] = rect;
    objs->number++;
}


void DeleteRectangle(Objects * objs, int x, int y){
    objs->rects[x][y] = (SDL_Rect){-1,-1,-1,-1};
    objs->number--;
}

void Update(SDL_Renderer * renderer, SDL_Window * window, Objects * objs){
    SDL_RenderClear(renderer);
    InitGrid(renderer, N, (SDL_Color){255,255,255,255});
    for(int i = 0; i < objs->number; i++){
        for(int j = 0; j < objs->number; j++){
            if(&objs->rects[i][j] != NULL){
                SDL_SetRenderDrawColor(renderer, 255,255,255,255);
                SDL_RenderFillRect(renderer, &objs->rects[i][j]);
            }

        }
    }
}

SDL_Rect MakeRectangle(int x, int y, int w, int h){
    return (SDL_Rect){x,y,w,h};
}