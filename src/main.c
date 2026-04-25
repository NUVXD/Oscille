#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "main.h"

/**
 *     ██████╗ ███████╗ ██████╗██╗██╗     ██╗     ███████╗
 *    ██╔═══██╗██╔════╝██╔════╝██║██║     ██║     ██╔════╝
 *    ██║   ██║███████╗██║     ██║██║     ██║     █████╗
 *    ██║   ██║╚════██║██║     ██║██║     ██║     ██╔══╝
 *    ╚██████╔╝███████║╚██████╗██║███████╗███████╗███████╗
 *     ╚═════╝ ╚══════╝ ╚═════╝╚═╝╚══════╝╚══════╝╚══════╝
 *
 * ASCII art made with:
 * https://patorjk.com/software/taag/#p=display&f=ANSI%20Shadow
 */

#define WINDOW_TITLE "Oscille"
#define WINDOW_INIT_W 900
#define WINDOW_INIT_H 600

static int appInit(appState *state) {
    // init and check SDL Video & SDL Audio
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("unable to initialize SDL - VIDEO or AUDIO: %s\n", SDL_GetError());
        return 2;
    }
    // init SDL TTF
    if (!TTF_Init()) {
        SDL_Log("couldn't init TTF: %s\n", SDL_GetError());
        return 2;
    }
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
    // create & check text engine
    state->TEXT.textEngine = TTF_CreateRendererTextEngine(state->renderer);
    if (!state->TEXT.textEngine) {
        SDL_Log("unable to create text engine: %s\n", SDL_GetError());
        return 2;
    }
    // init & check font
    state->TEXT.font = TTF_OpenFont("../assets/monospace.ttf", 15);
    if (!state->TEXT.font) {
        SDL_Log("unable to load font: %s\n", SDL_GetError());
        return 2;
    }
    // init & check text object
    state->TEXT.text = TTF_CreateText(state->TEXT.textEngine, state->TEXT.font, "\0", 0);
    if (!state->TEXT.text) {
        SDL_Log("unablel to create text object: %s\n", SDL_GetError());
        return 2;
    }
    // enables V-SYNC
    if (!SDL_SetRenderVSync(state->renderer, 1))
        SDL_Log("couldn't enable VSync: %s\n", SDL_GetError());

    // inits various appState variables
    SDL_GetWindowSize(state->window, &state->width, &state->height);
    state->AUDIO.volumeGain = 0.5f;

    return 0;
}

static int appClose(appState *state) {
    if (state) {
        if (state->AUDIO.audioStream)
            SDL_DestroyAudioStream(state->AUDIO.audioStream);
        if (state->WAV.wavBuffer)
            free(state->WAV.wavBuffer);
        if (state->window)
            SDL_DestroyWindow(state->window);
        if (state->renderer)
            SDL_DestroyRenderer(state->renderer);
        if (state->TEXT.textEngine)
            TTF_DestroyRendererTextEngine(state->TEXT.textEngine);
        if (state->TEXT.font)
            TTF_CloseFont(state->TEXT.font);
        if (state->TEXT.text)
            TTF_DestroyText(state->TEXT.text);
        free(state);
    }
    SDL_Quit();
    TTF_Quit();
    return 0;
}

int main(void) {
    /*************************
    /      INIT & GUARDS     /
    *************************/
    _Bool isError;
    appState *state = (appState *)calloc(1, sizeof(appState));
    if (!state) {
        SDL_Log("failed to allocate appState\n");
        return 2;
    }
    if (appInit(state) != 0) // (== 0): app correctly initialized, (!= 0): app not initialized
    {
        SDL_Log("app initialization failed\n");
        appClose(state);
    }

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
        if (state->WAV.wavBuffer) {
            isError = doWave(state, state->WAV.header, state->WAV.wavBuffer);
            if (isError) {
                SDL_Log("critical error while drawing waveform or parsing WAV\n");
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