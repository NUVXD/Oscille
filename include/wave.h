#ifndef WAVE_H
#define WAVE_H

#include "appstate.h"

typedef struct
{
    int hz;
    float amplitude;
    float phase;
    float time;
    float step;
    int pointCount;
    SDL_FPoint *points;
} Wave;

int doWave(appState *state);

#endif