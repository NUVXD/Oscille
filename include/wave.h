#ifndef WAVE_H
#define WAVE_H

#include "appstate.h"
#include "wav.h"

typedef struct {
    uint32_t pointCount;
    SDL_FPoint *points;
} Wave;

extern int doWave(appState *state, HEADER header, uint8_t *wavBuffer);
extern int doCanvas(appState *state);

#endif