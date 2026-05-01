#ifndef APPSTATE_H
#define APPSTATE_H

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "WAV.h"

#define COLOR_GREEN 74,246,38,255
#define COLOR_WHITE 255,255,255,255
#define COLOR_BLACK 0,0,0,255
#define _2PI (2 * SDL_PI_F)
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

typedef struct appStateScope {
    int width;
    int height;
    int scale;
    int mode;
    unsigned int maxPoints;
} appStateScope;

// APPSTATE STRUCT
// - owns all persistent runtime state.
typedef struct appState {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width, height;

    appStateWAV WAV;
    appStateText TEXT;
    appStateAudio AUDIO;
    appStateScope SCOPE;
    
    uint8_t _padding[4];
} appState;

#endif