#include <math.h>
#include <stdio.h>
#include "SDL3/SDL.h"
#include "wave.h"
#include "appstate.h"

// single channel for now

static SDL_FPoint calcPoint(Wave wave, appState state, float time)
{
    SDL_FPoint point;

    int originX = state.width / 2;
    int originY = state.height / 2;
    int scale = 100;

    point.x = originX + (wave.amplitude * cosf(time) * scale);
    point.y = originY + (wave.amplitude * sinf(time) * scale);

    return point;
}

static int initWave(Wave *wave)
{
    wave->amplitude = 1.00f;
    wave->time = 0;
    wave->step = 0.01f;
    wave->pointCount = (int)((2 * SDL_PI_F) / wave->step) + 1;
    wave->points = SDL_malloc(sizeof(SDL_FPoint) * wave->pointCount);
    if (!wave->points)
        return 0;
    return 1;
}

static void drawWave(Wave wave, appState *state)
{
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);
    SDL_SetRenderDrawColor(state->renderer, 74, 246, 38, 255); // green
    SDL_RenderLines(state->renderer, wave.points, wave.pointCount);
    SDL_RenderPresent(state->renderer);
}

int doWave(appState *state)
{
    Wave wave;
    initWave(&wave);

    for (size_t i = 0; wave.time <= 2 * SDL_PI_F; wave.time += wave.step, i++)
    {
        wave.points[i] = calcPoint(wave, *state, wave.time);
        //printf("(%f, %f)\n", wave.points[i].x, wave.points[i].y);
    }

    drawWave(wave, state);
    SDL_free(wave.points);

    return 0;
}
