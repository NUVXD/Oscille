#include "SDL3/SDL.h"
#include "UI.h"
#include "appstate.h"

void updateScope(appState *state) {
    SDL_FRect scopeSurface;
    scopeSurface.w = (float)(state->width * 3 / 4);
    scopeSurface.h = (float)state->height;
    scopeSurface.x = 0.f;
    scopeSurface.y = 0.f;

    state->scopeHeight = (int)scopeSurface.h;
    state->scopeWidth = (int)scopeSurface.w;

    SDL_SetRenderDrawColor(state->renderer, 74, 246, 38, 255); // green
    SDL_RenderRect(state->renderer, &scopeSurface);
}

void updateSettings(appState *state) {
    SDL_FRect settingsSurface;
    settingsSurface.h = (float)state->height;
    settingsSurface.w = (float)(state->width - state->scopeWidth);
    settingsSurface.x = (float)state->scopeWidth;
    settingsSurface.y = 0.f;

    SDL_SetRenderDrawColor(state->renderer, 74, 246, 38, 255); // green
    SDL_RenderRect(state->renderer, &settingsSurface);
}
