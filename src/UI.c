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

static void drawSymbol(SDL_Renderer *renderer, UI_BUTTONS btnType, SDL_FRect btnRect) {
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

            SDL_RenderLines(renderer, points, pointCount);
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

            SDL_RenderRect(renderer, &leftBar);
            SDL_RenderRect(renderer, &rightBar);
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

            SDL_RenderRect(renderer, &leftBar);
            SDL_RenderLines(renderer, points, pointCount);
            break;
        }

        case UI_BTN_VOLUME:
        {
            float volBarX = btnRect.x + 3.f; // starting point for bars
            float volBarCapX = btnHalfWidth / 2; // draw space, number of bars depends on available width
            float volBarOffY = btnHalfHeight / 2; // quarter height
            float volBarPosOffY = btnCenterY + volBarOffY; // quarter height above center Y
            float volBarNegOffY = btnCenterY - volBarOffY; // quarter height below center Y

            for (size_t i = 0; (float)i <= volBarCapX - 4.f; i++) {
                volBarX += 4.f;
                SDL_RenderLine(renderer, volBarX, volBarPosOffY, volBarX, volBarNegOffY);
            }
            break;
        }

        case UI_BTN_NONE:
        default:
            break;
    }
};

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
    drawSymbol(state->renderer, UI_BTN_PLAY, UI.btnPlay);
    // Pause
    UI.btnPause.h = BTN_H;
    UI.btnPause.w = settingsFrameW / 3;
    UI.btnPause.x = UI.btnPlay.x + UI.btnPlay.w; // to the right of Play Btn
    UI.btnPause.y = 1.f;
    SDL_RenderRect(state->renderer, &UI.btnPause);
    drawSymbol(state->renderer, UI_BTN_PAUSE, UI.btnPause);
    // Resume
    UI.btnResume.h = BTN_H;
    UI.btnResume.w = settingsFrameW / 3;
    UI.btnResume.x = UI.btnPause.x + UI.btnPause.w; // to the right of Pause Btn
    UI.btnResume.y = 1.f;
    SDL_RenderRect(state->renderer, &UI.btnResume);
    drawSymbol(state->renderer, UI_BTN_RESUME, UI.btnResume);

    // [ROW 2]
    // Volume
    UI.btnVolume.h = BTN_H;
    UI.btnVolume.w = settingsFrameW / 1;
    UI.btnVolume.x = settingsFrameX;
    UI.btnVolume.y = BTN_H + 1.f; // second row
    SDL_RenderRect(state->renderer, &UI.btnVolume);
    drawSymbol(state->renderer, UI_BTN_VOLUME, UI.btnVolume);
}

UI_BUTTONS getUIButton(float x, float y) {
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
