#include "SDL3/SDL.h"
#include "UI.h"
#include "appstate.h"

#define COLOR_GREEN 74,246,38,255

typedef struct {
    SDL_FRect scopeSurface;
    SDL_FRect settingsSurface;
    SDL_FRect btnPlay;
    SDL_FRect btnPause;
    SDL_FRect btnResume;
    SDL_FRect btnVolume;
} UI_ELEMENTS;
static UI_ELEMENTS UI;

static _Bool isMouseInButton(float x, float y, SDL_FRect button) {
    return ((x >= button.x) &&
        (x <= (button.x + button.w)) &&
        (y >= button.y) &&
        (y <= (button.y + button.h)));
}

void updateScope(appState *state) {
    //
    SDL_SetRenderDrawColor(state->renderer, COLOR_GREEN);

    // Frame
    UI.scopeSurface.w = (float)(state->width * 3 / 4);
    UI.scopeSurface.h = (float)state->height;
    UI.scopeSurface.x = 0.f;
    UI.scopeSurface.y = 0.f;
    state->scopeHeight = (int)UI.scopeSurface.h;
    state->scopeWidth = (int)UI.scopeSurface.w;
    SDL_RenderRect(state->renderer, &UI.scopeSurface);
}

void updateSettings(appState *state) {
    //
    SDL_SetRenderDrawColor(state->renderer, COLOR_GREEN);

    // Frame
    UI.settingsSurface.h = (float)state->height;
    UI.settingsSurface.w = (float)(state->width - state->scopeWidth);
    UI.settingsSurface.x = (float)state->scopeWidth;
    UI.settingsSurface.y = 0.f;
    SDL_RenderRect(state->renderer, &UI.settingsSurface);

    float BTN_H = 35.f; // default settings-button height, helps with row calc
    float settingsFrameW = (UI.settingsSurface.w - 2); // "- 2" j for aesthetic purposes
    float settingsFrameX = (UI.settingsSurface.x + 1); // cuz the "- 2" above

    // [ROW 1]
    // Play
    UI.btnPlay.h = BTN_H;
    UI.btnPlay.w = settingsFrameW / 3;
    UI.btnPlay.x = settingsFrameX;
    UI.btnPlay.y = 1.f;
    SDL_RenderRect(state->renderer, &UI.btnPlay);
    // Pause
    UI.btnPause.h = BTN_H;
    UI.btnPause.w = settingsFrameW / 3;
    UI.btnPause.x = UI.btnPlay.x + UI.btnPlay.w; // to the right of Play Btn
    UI.btnPause.y = 1.f;
    SDL_RenderRect(state->renderer, &UI.btnPause);
    // Resume
    UI.btnResume.h = BTN_H;
    UI.btnResume.w = settingsFrameW / 3;
    UI.btnResume.x = UI.btnPause.x + UI.btnPause.w; // to the right of Pause Btn
    UI.btnResume.y = 1.f;
    SDL_RenderRect(state->renderer, &UI.btnResume);

    // [ROW 2]
    // Volume
    UI.btnVolume.h = BTN_H;
    UI.btnVolume.w = settingsFrameW;
    UI.btnVolume.x = settingsFrameX;
    UI.btnVolume.y = BTN_H + 1.f; // second row
    SDL_RenderRect(state->renderer, &UI.btnVolume);
}

UI_BTN_CLICKED getUIButton(float x, float y) {
    if (isMouseInButton(x, y, UI.btnPlay))
        return UI_BTN_PLAY;
    if (isMouseInButton(x, y, UI.btnPause))
        return UI_BTN_PAUSE;
    if (isMouseInButton(x, y, UI.btnResume))
        return UI_BTN_RESUME;
    if (isMouseInButton(x, y, UI.btnVolume))
        return UI_BTN_VOLUME;
    return UI_BTN_NONE;
}
