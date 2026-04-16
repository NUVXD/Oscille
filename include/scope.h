#ifndef SCOPE_H
#define SCOPE_H

#include "appstate.h"
#include "WAV.h"

typedef struct {
    uint32_t pointCount;
    SDL_FPoint *points;
} Wave;

extern int doWave(appState *state, HEADER header, uint8_t *wavBuffer);

#endif