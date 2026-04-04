#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "SDL3/SDL.h"
#include "appstate.h"
#include "wave.h"
#include "wav.h"

#define windowTitle "Hello World! :3"
#define windowInitialW 800
#define windowInitialH 600

static int appInit(appState *state);
static int appEvents(appState *state, SDL_Event *event);
static int appClose(appState *state);

int main(void)
{
    /*************************
    /      INIT & GUARDS     /
    *************************/
    appState *state = (appState *)calloc(1, sizeof(appState));
    if (!state)
    {
        SDL_Log("Failed to allocate appState");
        return 2;
    }
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        free(state);
        return 2;
    }
    if (appInit(state) != 0) // 0 here is correctly initialized
    {
        SDL_Log("Renderer initialization failed");
        appClose(state);
        return 2;
    }

    /*************************
    /          LOOP          /
    *************************/
    _Bool running = 1;
    while (running)
    {
        // Events Call
        //
        SDL_Event event;
        while (SDL_PollEvent(&event))
            if (appEvents(state, &event))
                running = 0;

        // Every Frame - Graphics
        //
        if (state->height <= 0)
            state->height = 1;
        //
        doWave(state);
        
    }

    appClose(state);
    return 0;
}

static int appInit(appState *state)
{
    SDL_CreateWindowAndRenderer(
        windowTitle,
        windowInitialW,
        windowInitialH,
        SDL_WINDOW_RESIZABLE,
        &state->window,
        &state->renderer);

    if (!state->window)
    {
        SDL_Log("Couldn't create Window: %s", SDL_GetError());
        return 2;
    }
    if (!state->renderer)
    {
        SDL_Log("Couldn't create Renderer: %s", SDL_GetError());
        return 2;
    }
    SDL_GetWindowSize(state->window, &state->width, &state->height);

    parseWAV();

    return 0;
}

static int appEvents(appState *state, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
        return 1;
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE)
        return 1;

    if (event->type == SDL_EVENT_WINDOW_RESIZED)
    {
        state->width = event->window.data1;
        state->height = event->window.data2;
    }

    return 0;
}

static int appClose(appState *state)
{
    if (state)
    {
        if (state->window)
            SDL_DestroyWindow(state->window);
        if (state->renderer)
            SDL_DestroyRenderer(state->renderer);
        free(state);
    }
    SDL_Quit();
    return 0;
}