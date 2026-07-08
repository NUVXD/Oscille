#include <math.h>
#include <limits.h>
#include "SDL3/SDL.h"
#include "UI.h"
#include "scope.h"
#include "appstate.h"

// TODO
// - lower resolution of wave function as opposed to clamp/cap total points drawn <- big issue, ignoring for now

static int16_t read16Bit(const uint8_t *buffer) {
    int16_t value = (buffer[0] | ((int16_t)buffer[1] << 8));
    return value;
}

static int32_t read32Bit(const uint8_t *buffer) {
    int32_t value = (buffer[0] | ((int32_t)buffer[1] << 8) | ((int32_t)buffer[2] << 16) | ((int32_t)buffer[3] << 24));
    return value;
}

static _Bool whatFrame(appState state, HEADER header, size_t *startFrame, size_t *totalFrames) {
    size_t playedFrame = 0;

    if (state.AUDIO.audioStream) {
        int queuedBytes = SDL_GetAudioStreamQueued(state.AUDIO.audioStream);
        if (queuedBytes < 0)
            queuedBytes = 0;
        size_t queuedBytesSize = (size_t)queuedBytes;
        if (queuedBytesSize > header.Data.size)
            queuedBytesSize = header.Data.size;
        size_t playedBytes = header.Data.size - queuedBytesSize;
        playedFrame = playedBytes / (size_t)header.Format.bytesPerBlock;
    }

    *totalFrames = (size_t)(header.Data.size / header.Format.bytesPerBlock);
    if (*totalFrames == 0) {
        SDL_Log("total frames were calculated as 0\n");
        return 1;
    }

    *startFrame = playedFrame % *totalFrames;

    return 0;
}

static _Bool calcWAVPoints(appState *state, Wave *wave) {
    _Bool isError;

    // origins to center of scope canvas
    int originX = state->SCOPE.width / 2;  // canvas-width (x) center
    int originY = (state->SCOPE.height / 2) + 25; // canvas-height (y) center

    // scale/transform inits & checks
    if (state->SCOPE.scale <= 0)
        state->SCOPE.scale = 1;
    if (state->SCOPE.scale >= 100)
        state->SCOPE.scale = 100;
    int TRANSFORM = ((state->SCOPE.height / 2) * state->SCOPE.scale / 100);

    int maxScaleX = originX - 1;
    int maxScaleY = originY - 1;
    int maxScale = maxScaleY;

    if (maxScaleX < maxScaleY)
        maxScale = maxScaleX;
    if (maxScale < 1)
        maxScale = 1;
    if (TRANSFORM > maxScale)
        TRANSFORM = maxScale;

    size_t startFrame, totalFrames;
    isError = whatFrame(*state, state->WAV.header, &startFrame, &totalFrames);
    if (isError) {
        SDL_Log("error with whatFrame function\n");
        return 1;
    }

    uint16_t bitsPerSample = state->WAV.header.Format.bitsPerSample;
    switch (bitsPerSample) {
        case 16: case 24: case 32: break; // all ok here
        default: // anything else could cause issues, considering as error
            SDL_Log("bitsPerSample must be 16, 24, or 32 - current bitsPerSample: %u", bitsPerSample);
            return 1;
    }
    uint16_t sampleBytes = state->WAV.header.Format.bitsPerSample / 8;

    /* ---------------- */
    /*   POINT BY WAV   */
    /* ---------------- */
    for (size_t i = 0; i < wave->pointCount; i++) {
        size_t sampleFrame = (startFrame + i) % totalFrames;
        size_t sampleOffset = state->WAV.header.Data.dataStart + (sampleFrame * state->WAV.header.Format.bytesPerBlock);

        if (sampleOffset + sampleBytes >= state->WAV.header.Data.dataStart + state->WAV.header.Data.size) {
            SDL_Log("sampleOffset + sampleBytes exceeds allocated wav buffer\n");
            return 1;
        }

        int32_t leftSample;
        int32_t rightSample;
        float leftAmp;
        float rightAmp;

        switch (state->WAV.header.Format.bitsPerSample) {
            case 16:
                leftSample = read16Bit(&state->WAV.wavBuffer[sampleOffset]);
                rightSample = read16Bit(&state->WAV.wavBuffer[sampleOffset + sampleBytes]);
                // normalizes 0-1 for SCALE
                leftAmp = (float)leftSample / powf(2, 15);
                rightAmp = (float)rightSample / powf(2, 15);
                break;
            case 32:
                leftSample = read32Bit(&state->WAV.wavBuffer[sampleOffset]);
                rightSample = read32Bit(&state->WAV.wavBuffer[sampleOffset + sampleBytes]);
                // normalizes 0-1 for SCALE
                leftAmp = (float)leftSample / powf(2, 31);
                rightAmp = (float)rightSample / powf(2, 31);
                break;
            default:
                return 1;
        }

        float x;
        // if x-inversion
        if (*getUIBoolean(SETTINGS_IS_SCOPE_INVERTED_X))
            x = (float)originX - (leftAmp * (float)TRANSFORM);
        else
            x = (float)originX + (leftAmp * (float)TRANSFORM);

        if (x < 0.0f)
            x = 0.0f;
        else if (x > (float)(state->SCOPE.width - 1))
            x = (float)(state->SCOPE.width - 1);

        float y;
        // if y-inversion
        if (*getUIBoolean(SETTINGS_IS_SCOPE_INVERTED_Y))
            y = (float)originY - (rightAmp * (float)TRANSFORM);
        else
            y = (float)originY + (rightAmp * (float)TRANSFORM);

        if (y < 0.0f)
            y = 0.0f;
        else if (y > (float)(state->SCOPE.height - 1))
            y = (float)(state->SCOPE.height - 1);

        // populates points
        wave->points[i].x = x;
        wave->points[i].y = y;
    }

    return 0;
}

static int initWave(appState *state, Wave *wave) {
    wave->pointCount = state->WAV.header.Format.frequency;
    if (wave->pointCount > state->SCOPE.maxPoints)
        wave->pointCount = state->SCOPE.maxPoints;
    if (wave->pointCount == 0)
        return 1;
    wave->points = SDL_malloc(sizeof(SDL_FPoint) * wave->pointCount);
    if (!wave->points)
        return 1;
    return 0;
}

int doWave(appState *state) {
    _Bool isError;
    Wave wave = { 0 };

    SDL_Color color = getRGBColor(UI_COLOR_GREEN);
    SDL_SetRenderDrawColor(state->renderer, color.r, color.g, color.b, 255);

    isError = initWave(state, &wave);
    if (isError) {
        SDL_free(wave.points);
        return 1;
    }

    isError = calcWAVPoints(state, &wave);
    if (isError) {
        SDL_free(wave.points);
        return 1;
    }

    if (wave.pointCount > INT_MAX) {
        SDL_free(wave.points);
        return 1;
    }

    // draw points/lines
    if (state->SCOPE.mode == 0)
        SDL_RenderPoints(state->renderer, wave.points, (int)wave.pointCount);
    else if (state->SCOPE.mode == 1)
        SDL_RenderLines(state->renderer, wave.points, (int)wave.pointCount);

    SDL_free(wave.points);

    return 0;
}
