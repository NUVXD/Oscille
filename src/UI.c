#include "SDL3/SDL.h"
#include "UI.h"
#include "appstate.h"

#define GREEN 74,246,38,255

void updateScope(appState *state) {
    //
    SDL_SetRenderDrawColor(state->renderer, GREEN);

    // Frame
    SDL_FRect scopeSurface;
    scopeSurface.w = (float)(state->width * 3 / 4);
    scopeSurface.h = (float)state->height;
    scopeSurface.x = 0.f;
    scopeSurface.y = 0.f;
    state->scopeHeight = (int)scopeSurface.h;
    state->scopeWidth = (int)scopeSurface.w;
    SDL_RenderRect(state->renderer, &scopeSurface);
}

void updateSettings(appState *state) {
    //
    SDL_SetRenderDrawColor(state->renderer, GREEN);

    float BTN_H = 30.f;

    // Frame
    SDL_FRect settingsSurface;
    settingsSurface.h = (float)state->height;
    settingsSurface.w = (float)(state->width - state->scopeWidth);
    settingsSurface.x = (float)state->scopeWidth;
    settingsSurface.y = 0.f;
    SDL_RenderRect(state->renderer, &settingsSurface);

    // Pause
    SDL_FRect btnPause;
    btnPause.h = BTN_H;
    btnPause.w = (settingsSurface.w - 2) / 2;
    btnPause.x = settingsSurface.x + 1;
    btnPause.y = 1.f;
    SDL_RenderRect(state->renderer, &btnPause);

    // Resume
    SDL_FRect btnResume;
    btnResume.h = BTN_H;
    btnResume.w = (settingsSurface.w - 2) / 2;
    btnResume.x = (settingsSurface.x + 1) + btnPause.w;
    btnResume.y = 1.f;
    SDL_RenderRect(state->renderer, &btnResume);

    // Volume
    SDL_FRect btnVolume;
    btnVolume.h = BTN_H * 2;
    btnVolume.w = settingsSurface.w - 2;
    btnVolume.x = settingsSurface.x + 1;
    btnVolume.y = 1.f;
    SDL_RenderRect(state->renderer, &btnVolume);
}
