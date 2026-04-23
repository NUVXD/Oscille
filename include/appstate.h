#ifndef APPSTATE_H
#define APPSTATE_H

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "WAV.h"

// APPSTATE STRUCT
// - owns all persistent runtime state.
typedef struct appState {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_TextEngine *textEngine;
    TTF_Text *text;
    TTF_Font *font;
    HEADER header;
    SDL_AudioStream *audioStream;
    SDL_AudioSpec audioSpec;
    float volumeGain;
    int width, height;
    int scopeWidth, scopeHeight;
    uint8_t *wavBuffer;
} appState;

#endif