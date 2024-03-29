/*
    Copyright (C) 2020 Nahim Rouidjali
    This program is free software: 
    you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#include <stdio.h>
#include <stdlib.h>
#include "video.h"

#define _TITLE_ "Le jeu de la vie"
#define _HEIGHT_ 1000
#define _WIDTH_ 1000
enum TYPE_OF_LIFE
{
    UNDEFINED = 0, // Non instancié (Non vivante)
    ALIVE = 1,     // Cellules en cours de vie
    APPEARING = 2, // Cellules naissantes
    DYING = 3,     // Cellules Mourantes
    ONEGENONLY = 4 // Cellules ne vivant qu’une seule génération
};

typedef struct Life
{
    enum TYPE_OF_LIFE type; // enumerations des types de vie
    Uint8 neighbourAlive;   // Voisins en vie € {0,...8}
} Life;

typedef struct Objects
{
    SDL_Rect **rects; // Affichage Rect
    Life **matrix;    // Vies € {0,1,2,3,4}
    Uint8 n;          // n = max(i,j)
} Objects;

void InitGame(Objects *objs, INIT_VIDEO initv);
void InitGrid(SDL_Renderer **renderer, INIT_VIDEO initv, SDL_Color color);
void AddRectangle(Objects *objs, INIT_VIDEO initv, Uint16 x, Uint16 y, enum TYPE_OF_LIFE type);
void Render(SDL_Renderer **renderer, Objects *objs, INIT_VIDEO initv);
void ChangeRectangle(Objects *objs, int x, int y, enum TYPE_OF_LIFE type);
void UpdateNeighbourCount(Objects *objs, Uint16 N, int x, int y);
void LimitFPS(Uint32 n);
Uint8 grid = 1;
const SDL_Color gridColor = {82, 82, 82, 255};
const SDL_Color cellAlive = {200, 200, 200, 255};
const SDL_Color cellAppearing = {76, 209, 55, 255};
const SDL_Color cellDying = {232, 65, 24, 255};
const SDL_Color oneGenCell = {251, 197, 49, 255};
int main(int argc, char *args[])
{
    Objects objs;
    // Instanciation de la SDL
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    Uint16 N = 20; // Nombre de case par lignes et par columns nbTotal = N^2
    Uint16 generation = 0;
    /* Les différents états (stateOfRunning):
        0 => Destroyed, 1 => Start(Paramètrages/Drawing), 2 => Début du jeu, 3 => Jeu en pause
    */
    Uint8 stateOfRunning = 1;
    Uint8 speed = 100;

    for (Uint8 i = 1; i < argc - 1; i += 2)
    {
        Uint16 argsParam = atoi(args[i + 1]);
        Uint8 nbCheck = (!strcmp(args[i], "-n") || !strcmp(args[i], "--number"));
        Uint8 gridCheck = (!strcmp(args[i], "-g") || !strcmp(args[i], "--grid"));
        if (nbCheck && argsParam > 1)
        {
            N = argsParam;
        }
        else if (gridCheck && (argsParam == 1 || argsParam == 0))
        {
            grid = argsParam;
        }
    }

    const INIT_VIDEO initv = {_WIDTH_, _HEIGHT_, N, _HEIGHT_ / N, _WIDTH_ / N};

    InitSDL(&window, &renderer, initv);
    SDL_SetWindowTitle(window, _TITLE_);
    InitGame(&objs, initv);
    Render(&renderer, &objs, initv);
    while (stateOfRunning != 0)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                stateOfRunning = 0;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_SPACE && stateOfRunning != 1)
                {
                    if (stateOfRunning != 3)
                    {
                        stateOfRunning = 3;
                        SDL_Log("Game Paused");
                    }
                    else if (stateOfRunning == 3)
                    {
                        stateOfRunning = 2;
                        SDL_Log("Game Resume");
                    }
                }
                else if (event.key.keysym.sym == SDLK_PLUS || event.key.keysym.sym == SDLK_p)
                {
                    Uint8 increment = 10;
                    if ((speed + increment) <= 100)
                    {
                        speed += 10;
                        SDL_Log("Speed UP +10");
                    }
                }
                else if (event.key.keysym.sym == SDLK_MINUS || event.key.keysym.sym == SDLK_m)
                {
                    Uint8 increment = 10;
                    if ((speed - increment) > 0)
                    {
                        speed -= increment;
                        SDL_Log("Speed Down -10");
                    }
                }
                else if (event.key.keysym.sym == SDLK_s)
                {
                    stateOfRunning = 2;
                    speed = 10;
                }
                else if (event.key.keysym.sym == SDLK_c && (stateOfRunning == 3 || stateOfRunning == 1))
                {
                    InitGame(&objs, initv);
                    Render(&renderer, &objs, initv);
                    stateOfRunning = 1;
                    generation = 0;
                    SDL_Log("Restarting Game !");
                }
            }
            else if ((event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEMOTION) && stateOfRunning == 1)
            {

                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int x = event.motion.x;
                    int y = event.motion.y;
                    int xpos = (x / initv.hDivN);
                    int ypos = (y / initv.wDivN);
                    ChangeRectangle(&objs, xpos, ypos, ALIVE);
                    Render(&renderer, &objs, initv);
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    int x = event.motion.x;
                    int y = event.motion.y;
                    int xpos = (x / initv.hDivN);
                    int ypos = (y / initv.wDivN);
                    ChangeRectangle(&objs, xpos, ypos, UNDEFINED);
                    Render(&renderer, &objs, initv);
                }
            }
        }
        LimitFPS(1000 / speed);
        if (stateOfRunning == 2)
        {
            SDL_Log("Generation: %i", generation);
            for (Uint16 i = 0; i < objs.n; i++)
            {
                for (Uint16 j = 0; j < objs.n; j++)
                {
                    if (objs.matrix[i][j].neighbourAlive == 3)
                    {
                        if (objs.matrix[i][j].type == UNDEFINED)
                        {
                            ChangeRectangle(&objs, i, j, APPEARING);
                        }
                    }
                    else if (objs.matrix[i][j].neighbourAlive < 2 || objs.matrix[i][j].neighbourAlive > 3)
                    {
                        if (objs.matrix[i][j].type == ALIVE)
                        {
                            ChangeRectangle(&objs, i, j, DYING);
                        }
                    }
                }
            }
            Render(&renderer, &objs, initv);
            for (Uint16 i = 0; i < objs.n; i++)
            {
                for (Uint16 j = 0; j < objs.n; j++)
                {
                    if (objs.matrix[i][j].type == APPEARING)
                    {
                        ChangeRectangle(&objs, i, j, ALIVE);
                    }
                    if (objs.matrix[i][j].type == DYING)
                    {
                        ChangeRectangle(&objs, i, j, UNDEFINED);
                    }
                }
            }
            Render(&renderer, &objs, initv);
            generation++;
        }
    }
    UnloadSDL(window, renderer);
    SDL_Quit();
    exit(EXIT_SUCCESS);
}

void InitGame(Objects *objs, INIT_VIDEO initv)
{
    objs->n = initv.N + 1;
    objs->rects = (SDL_Rect **)calloc(objs->n, sizeof(SDL_Rect *));
    objs->matrix = (Life **)calloc(objs->n, sizeof(Life *));
    for (Uint16 i = 0; i < objs->n; i++)
    {
        objs->rects[i] = (SDL_Rect *)calloc(objs->n, sizeof(SDL_Rect));
        objs->matrix[i] = (Life *)calloc(objs->n, sizeof(Life));
        for (Uint16 j = 0; j < objs->n; j++)
        {
            AddRectangle(objs, initv, i, j, UNDEFINED);
        }
    }
}
void InitGrid(SDL_Renderer **renderer, INIT_VIDEO initv, SDL_Color color)
{
    // On créer des variables dans le local scope pour éviter de call plusieurs fois sur initv
    Uint16 WIDTH = initv.WIDTH;
    Uint16 HEIGHT = initv.HEIGHT;
    Uint16 casesVertical = initv.hDivN;
    Uint16 casesHorizontal = initv.wDivN;
    Uint16 sumVertial = 0;
    Uint16 sumHorizontal = 0;
    SDL_SetRenderDrawColor(*renderer, color.r, color.g, color.b, color.a);

    do
    {
        if (SDL_RenderDrawLine(*renderer, sumVertial, 0, sumVertial, HEIGHT) != 0)
        {
            SDL_Log("RenderDrawLine");
            exit(EXIT_FAILURE);
        }
        sumVertial += casesVertical;
    } while (sumVertial < WIDTH);

    do
    {
        if (SDL_RenderDrawLine(*renderer, 0, sumHorizontal, WIDTH, sumHorizontal) != 0)
        {
            SDL_Log("RenderDrawLine");
            exit(EXIT_FAILURE);
        }
        sumHorizontal += casesHorizontal;
    } while (sumHorizontal < HEIGHT);
}

void UpdateNeighbourCount(Objects *objs, Uint16 N, int x, int y)
{
    // Uniquement 3 calls max sur la matrice ! (Optimization process)
    Uint8 nbTemp = 0;
    if ((x - 1) > 0 && (y - 1) > 0)
    {
        if (objs->matrix[x - 1][y - 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((y - 1 > 0))
    {
        if (objs->matrix[x][y - 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((x + 1) < N && (y - 1) > 0)
    {
        if (objs->matrix[x + 1][y - 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((x + 1) < N)
    {
        if (objs->matrix[x + 1][y].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((x + 1) < N && (y + 1) < N)
    {
        if (objs->matrix[x + 1][y + 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((y + 1) < N)
    {
        if (objs->matrix[x][y + 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((x - 1) > 0 && (y + 1) < N)
    {
        if (objs->matrix[x - 1][y + 1].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    if ((x - 1) > 0)
    {
        if (objs->matrix[x - 1][y].type != UNDEFINED)
        {
            nbTemp++;
        }
    }

    objs->matrix[x][y].neighbourAlive = nbTemp;
}

void AddRectangle(Objects *objs, INIT_VIDEO initv, Uint16 x, Uint16 y, enum TYPE_OF_LIFE type)
{
    // Pas de verif sur x < 0 et y < 0 car unsigned int16 (limited range)
    if ((x >= initv.N || y >= initv.N))
    {
        return;
    }

    SDL_Rect rect;
    rect.x = initv.hDivN * x;
    rect.y = y * initv.wDivN;
    rect.w = initv.wDivN;
    rect.h = initv.hDivN;

    objs->rects[x][y] = rect;
    objs->matrix[x][y].type = type;
}

void ChangeRectangle(Objects *objs, int x, int y, enum TYPE_OF_LIFE type)
{
    objs->matrix[x][y].type = type;
}

void Render(SDL_Renderer **renderer, Objects *objs, INIT_VIDEO initv)
{
    SDL_SetRenderDrawColor(*renderer, 26, 26, 26, 255);
    SDL_RenderClear(*renderer);

    if (grid)
    {
        InitGrid(renderer, initv, gridColor);
    }

    for (Uint16 i = 0; i < objs->n; i++)
    {
        for (Uint16 j = 0; j < objs->n; j++)
        {
            switch (objs->matrix[i][j].type)
            {
            case ALIVE:
                SDL_SetRenderDrawColor(*renderer, cellAlive.r, cellAlive.g, cellAlive.b, cellAlive.a);
                break;
            case APPEARING:
                SDL_SetRenderDrawColor(*renderer, cellAppearing.r, cellAppearing.g, cellAppearing.b, cellAppearing.a);
                break;
            case DYING:
                SDL_SetRenderDrawColor(*renderer, cellDying.r, cellDying.g, cellDying.b, cellDying.a);
                break;
            case ONEGENONLY:
                SDL_SetRenderDrawColor(*renderer, oneGenCell.r, oneGenCell.g, oneGenCell.b, oneGenCell.a);
                break;
            default:
                SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 0);
                break;
            }

            // On veut pas draw les cellules non vivantes

            if (objs->matrix[i][j].type == UNDEFINED)
            {
                SDL_RenderDrawRect(*renderer, &objs->rects[i][j]);
            }
            else
            {
                SDL_RenderFillRect(*renderer, &objs->rects[i][j]);
            }

            UpdateNeighbourCount(objs, initv.N, i, j);
        }
    }

    SDL_RenderPresent(*renderer);
}

void LimitFPS(Uint32 n)
{
    Uint32 time = SDL_GetTicks();
    if ((SDL_GetTicks() - time) < n)
    {
        SDL_Delay(n);
    }
}
