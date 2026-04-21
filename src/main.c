#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "SDL3/SDL.h"
#include "main.h"

#define WINDOW_TITLE "Oscille"
#define WINDOW_INIT_W 900
#define WINDOW_INIT_H 600

static int appInit(appState *state) {
    // create window & renderer
    SDL_CreateWindowAndRenderer(
        WINDOW_TITLE,
        WINDOW_INIT_W,
        WINDOW_INIT_H,
        SDL_WINDOW_RESIZABLE,
        &state->window,
        &state->renderer);
    // check window
    if (!state->window) {
        SDL_Log("couldn't create Window: %s\n", SDL_GetError());
        return 2;
    }
    // check renderer
    if (!state->renderer) {
        SDL_Log("couldn't create Renderer: %s\n", SDL_GetError());
        return 2;
    }
    // VSYNC
    if (!SDL_SetRenderVSync(state->renderer, 1))
        SDL_Log("couldn't enable VSync: %s\n", SDL_GetError());

    SDL_GetWindowSize(state->window, &state->width, &state->height);

    return 0;
}

static int appClose(appState *state) {
    if (state) {
        if (state->audioStream)
            SDL_DestroyAudioStream(state->audioStream);
        if (state->wavBuffer)
            free(state->wavBuffer);
        if (state->window)
            SDL_DestroyWindow(state->window);
        if (state->renderer)
            SDL_DestroyRenderer(state->renderer);
        free(state);
    }
    SDL_Quit();
    return 0;
}

int main(void) {
    /*************************
    /      INIT & GUARDS     /
    *************************/
    appState *state = (appState *)calloc(1, sizeof(appState));
    if (!state) {
        SDL_Log("failed to allocate appState\n");
        return 2;
    }
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("unable to initialize SDL - VIDEO or AUDIO: %s\n", SDL_GetError());
        appClose(state);
    }
    }
    if (appInit(state) != 0) // (== 0): app correctly initialized, (!= 0): app not initialized
    {
        SDL_Log("app initialization failed\n");
        appClose(state);
    }

    /**
     * Trying out this new way of checking errors,
     * seems more readable (and better-looking),
     * also seems to adhere to what looks like consensus on error handling (?)
     * i might also try messing around with SDL's error handling in the future,
     * but for now i'll experiment with plain C to figure out what i like best
     *
     * Usually, i returned 0 on errors and checked immediately on function call
     * like if function(x) returned 0 on errors and i wanted to call it i'd do:
     * `if( !function(x) ){ printf("error! \\n"); }`
     *
     * I can picture ts new way would help discern error codes in a way,
     * like if i wanted to have a single errors-handler with different error codes
     * (since anything != 0 is TRUE)
     * ^ tho i won't do this now
     *
     * not sure how scalable either
     */
    _Bool isError;

    /*************************
    /          LOOP          /
    *************************/
    _Bool running = 1;
    while (running) {
        // [Events Call]
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (appEvents(state, &event))
                running = 0;
        }
        // [Every Frame - Graphics]
        // ensures height > 0
        if (state->height <= 0) {
            state->height = 1;
        }
        // DRAW
        // clears prev frame buffer
        SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
        SDL_RenderClear(state->renderer);
        // adds all the things to new frame
        updateScope(state); // updates scope surface
        updateSettings(state); // updates settings surface
        if (state->wavBuffer) {
            isError = doWave(state, state->header, state->wavBuffer);
            if (isError) {
                SDL_Log("error while drawing waveform\n");
                appClose(state);
                return 2;
            }
        }
        // presents new frame
        SDL_RenderPresent(state->renderer);
    }

    appClose(state);
    return 0;
}