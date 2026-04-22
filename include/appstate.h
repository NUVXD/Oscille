#ifndef APPSTATE_H
#define APPSTATE_H

#include "SDL3/SDL.h"
#include "WAV.h"

// APPSTATE STRUCT
// - owns all persistent runtime state.
typedef struct appState {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width, height;

    uint8_t *wavBuffer;
    HEADER header;
    SDL_AudioStream *audioStream;
    SDL_AudioSpec audioSpec;

    int scopeWidth, scopeHeight;
    float volumeGain;
} appState;

#endif