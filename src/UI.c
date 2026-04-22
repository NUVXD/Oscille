#include "SDL3/SDL.h"
#include "UI.h"
#include "appstate.h"

#define COLOR_GREEN 74,246,38,255

typedef struct {
    SDL_FRect scopeSurface;
    SDL_FRect settingsSurface;
    SDL_FRect fieldPath;
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

static void drawSymbol(appState *state, UI_BUTTONS btnType, SDL_FRect btnRect) {
    float btnHalfWidth = btnRect.w / 2;
    float btnHalfHeight = btnRect.h / 2;
    float btnCenterX = btnRect.x + btnHalfWidth;
    float btnCenterY = btnRect.y + btnHalfHeight;

    switch (btnType) {
        case UI_BTN_PLAY: {
            int pointCount = 4;
            SDL_FPoint points[4];

            if (btnHalfWidth >= 10.f) {
                points[0].x = btnCenterX - 7.5f;
                points[1].x = btnCenterX - 7.5f;
                points[2].x = btnCenterX + 7.5f;
                points[3].x = btnCenterX - 7.5f;
            }
            else {
                points[0].x = btnCenterX - (btnHalfWidth * 0.75f);
                points[1].x = btnCenterX - (btnHalfWidth * 0.75f);
                points[2].x = btnCenterX + (btnHalfWidth * 0.75f);
                points[3].x = btnCenterX - (btnHalfWidth * 0.75f);
            }
            // non-responsive cuz rect height is fixed
            points[0].y = btnCenterY + 7.5f;
            points[1].y = btnCenterY - 7.5f;
            points[2].y = btnCenterY + 0.f;
            points[3].y = btnCenterY + 7.5f;

            SDL_RenderLines(state->renderer, points, pointCount);
            break;
        }

        case UI_BTN_PAUSE: {
            SDL_FRect leftBar;
            SDL_FRect rightBar;

            if (btnHalfWidth >= 10.f) {
                leftBar.w = 5.f;
                leftBar.x = btnCenterX - 3.f - leftBar.w;
                rightBar.w = 5.f;
                rightBar.x = btnCenterX + 3.f;
            }
            else {
                leftBar.w = btnHalfWidth * 0.5f;
                leftBar.x = btnCenterX - (btnHalfWidth * 0.3f) - leftBar.w;
                rightBar.w = btnHalfWidth * 0.5f;
                rightBar.x = btnCenterX + (btnHalfWidth * 0.3f);
            }
            leftBar.h = 16.f; // 16.f instead of 15.f because i do 1.f when setting rect y, not sure why it matters but fixes it
            leftBar.y = btnCenterY - 7.5f;
            rightBar.h = 16.f;
            rightBar.y = btnCenterY - 7.5f;

            SDL_RenderRect(state->renderer, &leftBar);
            SDL_RenderRect(state->renderer, &rightBar);
            break;
        }

        case UI_BTN_RESUME: {
            int pointCount = 4;
            SDL_FPoint points[4];
            SDL_FRect leftBar;

            float leftBarW;
            float triangleW;
            float gap;
            float halfHeight = 7.5f;
            float innerMargin = 1.f;
            float symbolLeft = btnRect.x + innerMargin;
            float symbolRight = btnRect.x + btnRect.w - innerMargin;

            float symbolW = symbolRight - symbolLeft;
            if (symbolW <= 2.f)
                break;

            leftBarW = 5.f;
            triangleW = 15.f;
            gap = 2.5f;

            float totalW = leftBarW + gap + triangleW;
            if (totalW > symbolW) {
                float scale = symbolW / totalW;
                leftBarW *= scale;
                gap *= scale;
                triangleW *= scale;
                totalW = symbolW;
            }

            leftBar.w = leftBarW;
            leftBar.x = symbolLeft + ((symbolW - totalW) / 2.f);
            points[0].x = leftBar.x + leftBar.w + gap;
            points[1].x = leftBar.x + leftBar.w + gap;
            points[2].x = leftBar.x + leftBar.w + gap + triangleW;
            points[3].x = leftBar.x + leftBar.w + gap;

            leftBar.h = (halfHeight * 2.f) + 1.f;
            leftBar.y = btnCenterY - halfHeight;
            points[0].y = btnCenterY + halfHeight;
            points[1].y = btnCenterY - halfHeight;
            points[2].y = btnCenterY + 0.f;
            points[3].y = btnCenterY + halfHeight;

            SDL_RenderRect(state->renderer, &leftBar);
            SDL_RenderLines(state->renderer, points, pointCount);
            break;
        }

        case UI_BTN_NONE:
        case UI_BTN_VOLUME:
        case UI_FIELD_PATH:
        default:
            break;
    }
};

void updateScope(appState *state) {
    //
    SDL_SetRenderDrawColor(state->renderer, COLOR_GREEN);

    // Frame
    UI.scopeSurface = (SDL_FRect){
        .w = (float)(state->width * 3 / 4),
        .h = (float)state->height,
        .x = 0.f,
        .y = 0.f
    };
    state->scopeHeight = (int)UI.scopeSurface.h;
    state->scopeWidth = (int)UI.scopeSurface.w;
    SDL_RenderRect(state->renderer, &UI.scopeSurface);
}

void updateSettings(appState *state) {
    //
    SDL_SetRenderDrawColor(state->renderer, COLOR_GREEN);

    // Frame
    UI.settingsSurface = (SDL_FRect){
        .h = (float)state->height,
        .w = (float)(state->width - state->scopeWidth),
        .x = (float)state->scopeWidth,
        .y = 0.f
    };
    SDL_RenderRect(state->renderer, &UI.settingsSurface);

    float BTN_H = 35.f; // default settings-button height, helps with row calc
    float settingsFrameW = (UI.settingsSurface.w - 2); // "- 2" j for aesthetic purposes
    float settingsFrameX = (UI.settingsSurface.x + 1); // cuz the "- 2" above

    // [ROW 0]
    // WAV filePath field
    UI.fieldPath = (SDL_FRect){
        .h = BTN_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = (BTN_H * 0) + 1.f
    };
    SDL_RenderRect(state->renderer, &UI.fieldPath);

    // [ROW 1]
    // Play
    UI.btnPlay = (SDL_FRect){
        .h = BTN_H,
        .w = settingsFrameW / 3,
        .x = settingsFrameX,
        .y = (BTN_H * 1) + 1.f
    };
    SDL_RenderRect(state->renderer, &UI.btnPlay);
    drawSymbol(state, UI_BTN_PLAY, UI.btnPlay);
    // Pause
    UI.btnPause = (SDL_FRect){
        .h = BTN_H,
        .w = settingsFrameW / 3,
        .x = UI.btnPlay.x + UI.btnPlay.w, // to the right of Play Btn
        .y = (BTN_H * 1) + 1.f
    };
    SDL_RenderRect(state->renderer, &UI.btnPause);
    drawSymbol(state, UI_BTN_PAUSE, UI.btnPause);
    // Resume
    UI.btnResume = (SDL_FRect){
        .h = BTN_H,
        .w = settingsFrameW / 3, // to the right of Pause Btn
        .x = UI.btnPause.x + UI.btnPause.w,
        .y = (BTN_H * 1) + 1.f
    };
    SDL_RenderRect(state->renderer, &UI.btnResume);
    drawSymbol(state, UI_BTN_RESUME, UI.btnResume);

    // [ROW 2]
    // Volume
    SDL_FRect volFrameBig = {
        .h = BTN_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = (BTN_H * 2) + 1.f
    };
    SDL_RenderRect(state->renderer, &volFrameBig);
    UI.btnVolume = (SDL_FRect){
        .h = volFrameBig.h / 2,
        .w = volFrameBig.w - 20.f,
        .x = volFrameBig.x + 10.f,
        .y = (volFrameBig.y + (volFrameBig.h / 2)) - (UI.btnVolume.h / 2)
    };
    if (UI.btnVolume.w == 0) UI.btnVolume.w = 0.00001f;
    SDL_RenderRect(state->renderer, &UI.btnVolume);
    // ts is only UI representation of gain
    float UIgain = state->volumeGain;
    // clamps if for some reason < 0 || > 1
    if (UIgain < 0.f) UIgain = 0.f;
    if (UIgain > 1.f) UIgain = 1.f;
    SDL_FRect volBar = {
        .h = UI.btnVolume.h,
        .w = UI.btnVolume.w * UIgain,
        .x = UI.btnVolume.x,
        .y = UI.btnVolume.y
    };
    if (volBar.w == 0) volBar.w = 0.00001f;
    SDL_RenderFillRect(state->renderer, &volBar);
}

UI_BUTTONS getUIButtonEnum(float x, float y) {
    if (isMouseInButton(x, y, UI.fieldPath))
        return UI_FIELD_PATH;
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

SDL_FRect getUIButtonRect(UI_BUTTONS button) {
    switch (button) {
        case UI_BTN_VOLUME:
            return UI.btnVolume;
        case UI_FIELD_PATH:
            return UI.fieldPath;
        case UI_BTN_PLAY:
            return UI.btnPlay;
        case UI_BTN_PAUSE:
            return UI.btnPause;
        case UI_BTN_RESUME:
            return UI.btnResume;
        case UI_BTN_NONE:
            break;
    }
    return (SDL_FRect) { 0 };
}