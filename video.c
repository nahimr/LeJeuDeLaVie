/*
    Copyright (C) 2020 Nahim Rouidjali
    This program is free software: 
    you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/

#include <SDL2/SDL.h>
typedef struct INIT_VIDEO
{
    int WIDTH;
    int HEIGHT;
    Uint16 N;
    Uint16 hDivN;
    Uint16 wDivN;
} INIT_VIDEO;

void UnloadSDL(SDL_Window *window, SDL_Renderer *renderer)
{
    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
    }
    else if (window != NULL)
    {
        SDL_DestroyWindow(window);
    }
}

void InitSDL(SDL_Window **window, SDL_Renderer **renderer, INIT_VIDEO initv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Init failed!!");
        exit(EXIT_FAILURE);
    }
    if (SDL_CreateWindowAndRenderer(initv.WIDTH, initv.HEIGHT, SDL_WINDOW_SHOWN, window, renderer) != 0)
    {
        SDL_Log("Error Window & Renderer on initialisation");
        UnloadSDL(NULL, NULL);
        exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
}