#ifndef APPSTATE_H
#define APPSTATE_H

#include "SDL3/SDL.h"

// APPSTATE STRUCT
// - owns all persistent runtime state.
typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width;
    int height;
} appState;

#endif