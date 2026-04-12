#ifndef APPSTATE_H
#define APPSTATE_H

#include "SDL3/SDL.h"

// SCOPE CANVAS
typedef struct {
    int width;
    int height;
    SDL_FPoint LTop;
    SDL_FPoint LBottom;
    SDL_FPoint RTop;
    SDL_FPoint RBottom;
} CANVAS;

// APPSTATE STRUCT
// - owns all persistent runtime state.
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_AudioStream *audioStream;
    int width;
    int height;
    CANVAS canvas;
} appState;

#endif