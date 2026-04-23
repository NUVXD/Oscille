#ifndef SCOPE_H
#define SCOPE_H

#include <stddef.h>
#include "WAV.h"

struct appState;

typedef struct {
    SDL_FPoint *points;
    size_t pointCount;
} Wave;

extern int doWave(struct appState *state, HEADER header, uint8_t *wavBuffer);

#endif