#include<stdio.h> 
#include<stdlib.h> 
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> 
#include <SDL2/SDL_timer.h> 
#define HEIGHT 1000
#define WIDTH 1000
typedef struct Objects{
    SDL_Rect * rects;
    int number;
} Objects;

void DestroySDL(SDL_Renderer * renderer, SDL_Window * window);
void InitGrid(SDL_Renderer * renderer, int n, SDL_Color color);
void AddRectangle(SDL_Renderer * renderer, SDL_Window * window, Objects * objs, int x, int y, int s);
SDL_Rect MakeRectangle(int x, int y, int w, int h);
void Update(SDL_Renderer * renderer, SDL_Window * window, Objects objs);
void DeleteRectangle(Objects * objs, int pos);
int main() 
{ 
    int isRunning = 1;
    int n = 20;
    Objects objs;
    objs.rects = (SDL_Rect*) malloc(sizeof(SDL_Rect) * n);

    objs.rects[0] = MakeRectangle(1,2,HEIGHT/20, WIDTH/20);
    SDL_Window * window = NULL;
    SDL_Renderer * renderer = NULL;
    
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        SDL_Log("Init failed!!");
    }

    if(SDL_CreateWindowAndRenderer(WIDTH,HEIGHT,SDL_WINDOW_SHOWN, &window, &renderer))
        SDL_Log("Error Window");
   
    SDL_SetWindowTitle(window, "Le jeu de la vie");
    
    SDL_Color color; color.r = 255; color.g = 255; color.b = 255; color.a = 255;
    
    InitGrid(renderer, n, color);
    
    for(int i = 0; i < 10; i++){
        AddRectangle(renderer,window, &objs, i,i, HEIGHT/n);
    }
    
    Update(renderer, window, objs);

    DeleteRectangle(&objs, 0);


    Update(renderer, window, objs);
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
    SDL_Rect rectangle;
    rectangle.h = s;
    rectangle.w = s;
    rectangle.x = 0 + s * x;
    rectangle.y = 0 + y * s;
    objs->rects[objs->number++] = rectangle;
}


void DeleteRectangle(Objects * objs, int pos){
    free(&objs->rects[pos]);

    for(int i = pos ; i < objs->number - 1; i++){
        objs->rects[i] = objs->rects[i+1];
    }
}

void Update(SDL_Renderer * renderer, SDL_Window * window, Objects objs){
    printf("%i",objs.number);
    for(int i = 0; i < objs.number; i++){
        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        SDL_RenderFillRect(renderer, &objs.rects[i]);
    }
}

SDL_Rect MakeRectangle(int x, int y, int w, int h){
    return (SDL_Rect){x,y,w,h};
}