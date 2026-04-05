#include <math.h>
#include <stdio.h>
#include "SDL3/SDL.h"
#include "wave.h"
#include "appstate.h"

// TODO
// - lower resolution of wave function as opposed to clamp/cap total points drawn

#define SCALE 300U // hard-coded for now, TODO as option
#define _2PI (2 * SDL_PI_F)
#define MAX_SCOPE_POINTS 2048U

static uint16_t read16Bit(const uint8_t *buffer) { return (uint16_t)(buffer[0] | ((uint16_t)buffer[1] << 8)); }
static uint32_t read32Bit(const uint8_t *buffer) { return (uint32_t)(buffer[0] | ((uint32_t)buffer[1] << 8) | ((uint32_t)buffer[2] << 16) | ((uint32_t)buffer[3] << 24)); }

static void calcPoints(Wave *wave, appState state, HEADER header, uint8_t *wavBuffer)
{
    // origins to center of screen
    int originX = state.width / 2;  // screen-width (x) center
    int originY = state.height / 2; // screen-height (y) center

    size_t playedFrame = 0;
    if (state.audioStream)
    {
        int queuedBytes = SDL_GetAudioStreamQueued(state.audioStream);
        if (queuedBytes < 0)
            queuedBytes = 0;
        size_t queuedBytesSize = (size_t)queuedBytes;
        if (queuedBytesSize > header.Data.size)
            queuedBytesSize = header.Data.size;
        size_t playedBytes = header.Data.size - queuedBytesSize;
        playedFrame = playedBytes / (size_t)header.Format.bytesPerBlock;
    }

    size_t totalFrames = (size_t)(header.Data.size / header.Format.bytesPerBlock);
    if (totalFrames == 0)
        return;

    size_t startFrame = playedFrame % totalFrames;

    /*************************
    /     POINT BY WAV       /
    *************************/
    for (size_t i = 0; i < wave->pointCount; i++)
    {
        size_t sampleFrame = (startFrame + i) % totalFrames;
        size_t sampleOffset = header.Data.dataStart + (sampleFrame * header.Format.bytesPerBlock);

        int32_t leftSample;
        int32_t rightSample;
        float leftAmp;
        float rightAmp;

        switch (header.Format.bitsPerSample)
        {
        case 16:
            leftSample = (int16_t)read16Bit(&wavBuffer[sampleOffset]);
            rightSample = (int16_t)read16Bit(&wavBuffer[sampleOffset + 2]);
            leftAmp = (float)leftSample / 32768.0f;
            rightAmp = (float)rightSample / 32768.0f;
            break;
        case 32:
            leftSample = (int32_t)read32Bit(&wavBuffer[sampleOffset]);
            rightSample = (int32_t)read32Bit(&wavBuffer[sampleOffset + 2]);
            leftAmp = (float)leftSample / 2147483648.0f;
            rightAmp = (float)rightSample / 2147483648.0f;
            break;
        default:
            return;
        }

        wave->points[i].x = originX + (leftAmp * SCALE);
        wave->points[i].y = originY + (rightAmp * SCALE);
    }
}

static int initWave(Wave *wave, HEADER header)
{
    wave->pointCount = header.Format.frequency;
    if (wave->pointCount > MAX_SCOPE_POINTS)
        wave->pointCount = MAX_SCOPE_POINTS;
    if (wave->pointCount == 0)
        return 0;
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
    SDL_RenderPoints(state->renderer, wave.points, wave.pointCount);
    SDL_RenderPresent(state->renderer);
}

int doWave(appState *state, HEADER header, uint8_t *wavBuffer)
{
    Wave wave;

    if (!initWave(&wave, header))
        return 0;
    calcPoints(&wave, *state, header, wavBuffer);

    drawWave(wave, state);
    SDL_free(wave.points);

    return 1;
}
