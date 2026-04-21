#ifndef SCOPE_H
#define SCOPE_H

#include "WAV.h"

struct appState;

typedef struct {
    uint32_t pointCount;
    SDL_FPoint *points;
} Wave;

extern int doWave(struct appState *state, HEADER header, uint8_t *wavBuffer);

#endif