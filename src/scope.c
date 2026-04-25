#include <math.h>
#include <stdio.h>
#include <limits.h>
#include "SDL3/SDL.h"
#include "scope.h"
#include "appstate.h"

// TODO
// - lower resolution of wave function as opposed to clamp/cap total points drawn <- big issue, ignoring for now

#define _2PI (2 * SDL_PI_F)
#define SCALE 90U // hard-coded for now, TODO as option (error case if < 0)
#define MAX_SCOPE_POINTS 2048U // maximum possible number of total points per frame (pure cap, not sampling reduction)
#define COLOR_GREEN 74,246,38,255

static uint16_t read16Bit(const uint8_t *buffer) { return (uint16_t)(buffer[0] | ((uint16_t)buffer[1] << 8)); }
static uint32_t read32Bit(const uint8_t *buffer) { return (uint32_t)(buffer[0] | ((uint32_t)buffer[1] << 8) | ((uint32_t)buffer[2] << 16) | ((uint32_t)buffer[3] << 24)); }

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

static _Bool calcPoints(Wave *wave, appState state, HEADER header, uint8_t *wavBuffer) {
    _Bool isError;

    // origins to center of scope canvas
    int originX = state.scopeWidth / 2;  // canvas-width (x) center
    int originY = state.scopeHeight / 2; // canvas-height (y) center

    // scale/transform inits & checks
    int TRANSFORM = ((state.scopeHeight / 2) * SCALE / 100);
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
    isError = whatFrame(state, header, &startFrame, &totalFrames);
    if (isError) {
        SDL_Log("error with whatFrame function\n");
        return 1;
    }

    uint16_t bitsPerSample = header.Format.bitsPerSample;
    switch (bitsPerSample) {
        case 8: case 16: case 24: case 32: break; // all ok here
        default: // anything else could cause issues, considering as error
            SDL_Log("bitsPerSample must be 8, 16, 24, or 32 - current bitsPerSample: %u", bitsPerSample);
            return 1;
    }
    uint16_t sampleBytes = header.Format.bitsPerSample / 8;

    /*************************
    /     POINT BY WAV       /
    *************************/
    for (size_t i = 0; i < wave->pointCount; i++) {
        size_t sampleFrame = (startFrame + i) % totalFrames;
        size_t sampleOffset = header.Data.dataStart + (sampleFrame * header.Format.bytesPerBlock);

        if (sampleOffset + sampleBytes >= header.Data.size) {
            SDL_Log("sampleOffset + sampleBytes exceeds allocated wav buffer\n");
            return 1;
        }

        int32_t leftSample;
        int32_t rightSample;
        float leftAmp;
        float rightAmp;

        switch (header.Format.bitsPerSample) {
            case 16:
                leftSample = (int16_t)read16Bit(&wavBuffer[sampleOffset]);
                rightSample = (int16_t)read16Bit(&wavBuffer[sampleOffset + sampleBytes]);
                // normalizes 0-1 for SCALE
                leftAmp = (float)leftSample / powf(2, 15);
                rightAmp = (float)rightSample / powf(2, 15);
                break;
            case 32:
                leftSample = (int32_t)read32Bit(&wavBuffer[sampleOffset]);
                rightSample = (int32_t)read32Bit(&wavBuffer[sampleOffset + sampleBytes]);
                // normalizes 0-1 for SCALE
                leftAmp = (float)leftSample / powf(2, 31);
                rightAmp = (float)rightSample / powf(2, 31);
                break;
            default:
                return 1;
        }

        float x = (float)originX + (leftAmp * (float)TRANSFORM);
        float y = (float)originY + (rightAmp * (float)TRANSFORM);

        // keeps points inside scope and makes sure > 0
        if (x < 0.0f)
            x = 0.0f;
        else if (x > (float)(state.scopeWidth - 1))
            x = (float)(state.scopeWidth - 1);
        if (y < 0.0f)
            y = 0.0f;
        else if (y > (float)(state.scopeHeight - 1))
            y = (float)(state.scopeHeight - 1);

        // populates points
        wave->points[i].x = x;
        wave->points[i].y = y;
    }

    return 0;
}

static int initWave(Wave *wave, HEADER header) {
    wave->pointCount = header.Format.frequency;
    if (wave->pointCount > MAX_SCOPE_POINTS)
        wave->pointCount = MAX_SCOPE_POINTS;
    if (wave->pointCount == 0)
        return 1;
    wave->points = SDL_malloc(sizeof(SDL_FPoint) * wave->pointCount);
    if (!wave->points)
        return 1;
    return 0;
}

int doWave(appState *state, HEADER header, uint8_t *wavBuffer) {
    Wave wave;
    _Bool isError;

    isError = initWave(&wave, header);
    if (isError)
        return 1;

    isError = calcPoints(&wave, *state, header, wavBuffer);
    if (isError)
        return 1;

    // draw points
    SDL_SetRenderDrawColor(state->renderer, COLOR_GREEN);
    if (wave.pointCount > INT_MAX)
        return 1;
    SDL_RenderPoints(state->renderer, wave.points, (int)wave.pointCount);

    SDL_free(wave.points);

    return 0;
}
