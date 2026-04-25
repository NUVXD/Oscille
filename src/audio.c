#include "SDL3/SDL.h"
#include "appstate.h"

void setGain(appState *state, float gain) {
    if (state->AUDIO.audioStream && !SDL_SetAudioStreamGain(state->AUDIO.audioStream, gain))
        SDL_Log("unable to set audio stream gain: %s\n", SDL_GetError());
}

void initAudio(appState *state) {
    uint16_t bitsPerSample = state->WAV.header.Format.bitsPerSample;
    if (bitsPerSample == 16)
        state->AUDIO.audioSpec.format = SDL_AUDIO_S16;
    else if (bitsPerSample == 32)
        state->AUDIO.audioSpec.format = SDL_AUDIO_S32;
    else {
        SDL_Log("unsupported BitsPerSample\n");
        return;
    }
    state->AUDIO.audioSpec.channels = (int)state->WAV.header.Format.channelsNumber;
    state->AUDIO.audioSpec.freq = (int)state->WAV.header.Format.frequency;

    state->AUDIO.audioStream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, 
        &state->AUDIO.audioSpec,
        (void *)0, 
        (void *)0
    );

    if (!state->AUDIO.audioStream) {
        SDL_Log("unable to open audio stream: %s\n", SDL_GetError());
        if (state->WAV.wavBuffer) {
            free(state->WAV.wavBuffer);
            state->WAV.wavBuffer = (void *)0;
        }
        return;
    }

    setGain(state, state->AUDIO.volumeGain);

    if (!SDL_PutAudioStreamData(state->AUDIO.audioStream, state->WAV.wavBuffer + state->WAV.header.Data.dataStart, (int)state->WAV.header.Data.size)) {
        SDL_Log("unable to queue WAV data for playback: %s\n", SDL_GetError());
        SDL_DestroyAudioStream(state->AUDIO.audioStream);
        state->AUDIO.audioStream = (void *)0;
        if (state->WAV.wavBuffer) {
            free(state->WAV.wavBuffer);
            state->WAV.wavBuffer = (void *)0;
        }
        return;
    }

    if (!SDL_ResumeAudioStreamDevice(state->AUDIO.audioStream)) {
        SDL_Log("unable to start audio playback: %s\n", SDL_GetError());
        SDL_DestroyAudioStream(state->AUDIO.audioStream);
        state->AUDIO.audioStream = (void *)0;
        if (state->WAV.wavBuffer) {
            free(state->WAV.wavBuffer);
            state->WAV.wavBuffer = (void *)0;
        }
        return;
    }
}

void destroyAudio(appState *state) {
    if (state->AUDIO.audioStream) {
        SDL_DestroyAudioStream(state->AUDIO.audioStream);
        state->AUDIO.audioStream = (void *)0;
    }
}

void pauseAudio(appState *state) {
    if (state->AUDIO.audioStream)
        SDL_PauseAudioStreamDevice(state->AUDIO.audioStream);
}
void resumeAudio(appState *state) {
    if (state->AUDIO.audioStream)
        SDL_ResumeAudioStreamDevice(state->AUDIO.audioStream);
}