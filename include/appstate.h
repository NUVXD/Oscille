#ifndef APPSTATE_H
#define APPSTATE_H

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "WAV.h"

#define APP_WAV_PATH_MAX 401U

typedef struct appStateText {
    TTF_TextEngine *textEngine;
    TTF_Text *text;
    TTF_Font *font;
} appStateText;

typedef struct appStateAudio {
    SDL_AudioStream *audioStream;
    SDL_AudioSpec audioSpec;
    float volumeGain;
} appStateAudio;

typedef struct appStateWAV {
    uint8_t *wavBuffer;
    HEADER header;
    char filePath[APP_WAV_PATH_MAX];
    uint8_t _padding[7];
} appStateWAV;

// APPSTATE STRUCT
// - owns all persistent runtime state.
typedef struct appState {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width, height;
    int scopeWidth, scopeHeight;

    appStateWAV WAV;
    appStateText TEXT;
    appStateAudio AUDIO;
} appState;

#endif