#include "SDL3/SDL.h"
#include "appstate.h"

void setGain(appState *state, float gain) {
    if (state->audioStream && !SDL_SetAudioStreamGain(state->audioStream, gain))
        SDL_Log("unable to set audio stream gain: %s\n", SDL_GetError());
}

void initAudio(appState *state) {
    SDL_AudioSpec audioSpec = { 0 };
    uint16_t bitsPerSample = state->header.Format.bitsPerSample;
    if (bitsPerSample == 16)
        audioSpec.format = SDL_AUDIO_S16;
    else if (bitsPerSample == 32)
        audioSpec.format = SDL_AUDIO_S32;
    else {
        SDL_Log("unsupported BitsPerSample\n");
        return;
    }
    audioSpec.channels = (int)state->header.Format.channelsNumber;
    audioSpec.freq = (int)state->header.Format.frequency;

    state->audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec, (void *)0, (void *)0);
    if (!state->audioStream) {
        SDL_Log("unable to open audio stream: %s\n", SDL_GetError());
        if (state->wavBuffer) {
            free(state->wavBuffer);
            state->wavBuffer = (void *)0;
        }
        return;
    }

    setGain(state, state->volumeGain);

    if (!SDL_PutAudioStreamData(state->audioStream, state->wavBuffer + state->header.Data.dataStart, (int)state->header.Data.size)) {
        SDL_Log("unable to queue WAV data for playback: %s\n", SDL_GetError());
        SDL_DestroyAudioStream(state->audioStream);
        state->audioStream = (void *)0;
        if (state->wavBuffer) {
            free(state->wavBuffer);
            state->wavBuffer = (void *)0;
        }
        return;
    }

    if (!SDL_ResumeAudioStreamDevice(state->audioStream)) {
        SDL_Log("unable to start audio playback: %s\n", SDL_GetError());
        SDL_DestroyAudioStream(state->audioStream);
        state->audioStream = (void *)0;
        if (state->wavBuffer) {
            free(state->wavBuffer);
            state->wavBuffer = (void *)0;
        }
        return;
    }
}

void destroyAudio(appState *state) {
    if (state->audioStream) {
        SDL_DestroyAudioStream(state->audioStream);
        state->audioStream = (void *)0;
    }
}

void pauseAudio(appState *state) {
    if (state->audioStream)
        SDL_PauseAudioStreamDevice(state->audioStream);
}
void resumeAudio(appState *state) {
    if (state->audioStream)
        SDL_ResumeAudioStreamDevice(state->audioStream);
}