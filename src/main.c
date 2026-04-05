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
        SDL_Log("failed to allocate appState\n");
        return 2;
    }
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        SDL_Log("unable to initialize SDL: %s\n", SDL_GetError());
        free(state);
        return 2;
    }
    if (appInit(state) != 0) // 0 here is correctly initialized
    {
        SDL_Log("renderer initialization failed\n");
        appClose(state);
        return 2;
    }

    HEADER header = {0};
    uint8_t *wavBuffer = (void *)0;
    if (!parseWAV(&header, &wavBuffer))
    {
        SDL_Log("error with parsing WAV file\n");
        appClose(state);
        return 2;
    }
    printf("%zu BYTES\n", header.Riff.fileSize);
    printf("MAIN CHUNK | RiffID: %s | fileFormatID: %s\n", header.Riff.ID, header.Riff.fileFormatID);
    printf("SUBCHUNK 1 | FormatID: %s | BitsPerSample: %u | Frequency: %u | BytesPerSec: %u\n", header.Format.ID, header.Format.bitsPerSample, header.Format.frequency, header.Format.bytesPerSec);

    SDL_AudioSpec audioSpec = {0};

    switch (header.Format.bitsPerSample)
    {
    case 16:
        audioSpec.format = SDL_AUDIO_S16;
        break;
    case 32:
        audioSpec.format = SDL_AUDIO_S32;
        break;
    default:
        SDL_Log("unsupported BitsPerSample\n");
        appClose(state);
        return 2;
    }
    audioSpec.channels = (int)header.Format.channelsNumber;
    audioSpec.freq = (int)header.Format.frequency;
    state->audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec, NULL, NULL);

    if (!state->audioStream)
    {
        SDL_Log("Unable to open audio stream: %s\n", SDL_GetError());
        appClose(state);
        free(wavBuffer);
        return 2;
    }
    if (!SDL_PutAudioStreamData(state->audioStream, wavBuffer + header.Data.dataStart, (int)header.Data.size))
    {
        SDL_Log("unable to queue WAV data for playback: %s\n", SDL_GetError());
        appClose(state);
        free(wavBuffer);
        return 2;
    }
    if (!SDL_ResumeAudioStreamDevice(state->audioStream))
    {
        SDL_Log("unable to start audio playback: %s\n", SDL_GetError());
        appClose(state);
        free(wavBuffer);
        return 2;
    }

    /*************************
    /          LOOP          /
    *************************/
    _Bool running = 1;
    while (running)
    {
        // [Events Call]

        SDL_Event event;
        while (SDL_PollEvent(&event))
            if (appEvents(state, &event))
                running = 0;

        // [Every Frame - Graphics]

        if (state->height <= 0)
            state->height = 1;

        if (!doWave(state, header, wavBuffer))
        {
            appClose(state);
            return 2;
        }
    }

    appClose(state);
    return 0;
}

static int appInit(appState *state)
{
    // create window & renderer
    SDL_CreateWindowAndRenderer(
        windowTitle,
        windowInitialW,
        windowInitialH,
        SDL_WINDOW_RESIZABLE,
        &state->window,
        &state->renderer);

    // initialize window
    if (!state->window)
    {
        SDL_Log("couldn't create Window: %s\n", SDL_GetError());
        return 2;
    }

    // initialize renderer
    if (!state->renderer)
    {
        SDL_Log("couldn't create Renderer: %s\n", SDL_GetError());
        return 2;
    }

    if (!SDL_SetRenderVSync(state->renderer, 1))
        SDL_Log("couldn't enable VSync: %s\n", SDL_GetError());

    SDL_GetWindowSize(state->window, &state->width, &state->height);

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
        if (state->audioStream)
            SDL_DestroyAudioStream(state->audioStream);
        if (state->window)
            SDL_DestroyWindow(state->window);
        if (state->renderer)
            SDL_DestroyRenderer(state->renderer);
        free(state);
    }
    SDL_Quit();
    return 0;
}