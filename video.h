#include "video.c"
#ifndef VIDEO_H
void InitSDL(SDL_Window **window, SDL_Renderer **renderer, INIT_VIDEO initv);
void UnloadSDL(SDL_Window *window, SDL_Renderer *renderer);
#endif