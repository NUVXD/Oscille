#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "UI.h"
#include "appstate.h"
#include <string.h>

#define COLOR_GREEN 74,246,38,255
#define COLOR_WHITE 255,255,255,255
#define COLOR_BLACK 0,0,0,255

typedef struct {
    /* ----------- */
    /*   BUTTONS   */
    /* ----------- */
    struct {
        SDL_FRect wavPlay;
        SDL_FRect wavPause;
        SDL_FRect wavResume;
        SDL_FRect wavVolume;

        SDL_FRect scopeScaleNeg;
        SDL_FRect scopeScalePos;
    } buttons;

    /* ---------- */
    /*   TITLES   */
    /* ---------- */
    struct {
        SDL_FRect wavSettings;
        SDL_FRect wavFilePath;
        SDL_FRect wavControls;
        SDL_FRect wavVolume;

        SDL_FRect scopeSettings;
        SDL_FRect scopeScale;
    } titles;

    /* ---------- */
    /*   FIELDS   */
    /* ---------- */
    struct {
        SDL_FRect wavFilePath;
        SDL_FRect scopeScale;
    } fields;
    //
} UI_ELEMENTS;
static UI_ELEMENTS UI;

typedef struct {
    char *text;
    int w;
    int h;
    float x;
    float y;
} UI_TEXT;

static void renderTitle(appState *state, UI_TEXT *UI_Text, char *textString, SDL_FRect rect) {
    UI_Text->text = textString;
    TTF_SetFontStyle(state->TEXT.font, TTF_STYLE_BOLD);
    TTF_SetTextString(state->TEXT.text, UI_Text->text, 0);
    TTF_GetTextSize(state->TEXT.text, &UI_Text->w, &UI_Text->h);
    UI_Text->x = (rect.x + (rect.w / 2)) - (UI_Text->w / 2);
    UI_Text->y = (rect.y + (rect.h / 2)) - (UI_Text->h / 2);
    // keeps them from overlapping on the left
    if (UI_Text->x < rect.x + 5.f)
        UI_Text->x = rect.x + 5.f;
    TTF_DrawRendererText(state->TEXT.text, UI_Text->x, UI_Text->y);
    TTF_SetFontStyle(state->TEXT.font, TTF_STYLE_NORMAL);
}

static void renderPathFieldText(appState *state, SDL_FRect *rect) {
    const char *source = state->WAV.filePath;

    char visibleText[APP_WAV_PATH_MAX + 4];
    int textW = 0;
    int textH = 0;
    float textX;
    float textY;

    TTF_SetTextString(state->TEXT.text, source, 0);
    TTF_GetTextSize(state->TEXT.text, &textW, &textH);

    if ((float)textW > (rect->w - 10.f)) {
        size_t srcLen = strlen(source);
        size_t start = 0;

        visibleText[0] = '\0';
        for (start = 0; start < srcLen; start++) {
            SDL_snprintf(visibleText, sizeof(visibleText), "...%s", source + start);
            TTF_SetTextString(state->TEXT.text, visibleText, 0);
            TTF_GetTextSize(state->TEXT.text, &textW, &textH);
            if ((float)textW <= (rect->w - 10.f)) {
                break;
            }
        }
    }

    textX = rect->x + 5.f;
    textY = rect->y + ((rect->h - (float)textH) / 2.f);
    TTF_DrawRendererText(state->TEXT.text, textX, textY);
}

static _Bool isMouseInButton(float x, float y, SDL_FRect button) {
    _Bool isX = (x >= button.x) && (x <= (button.x + button.w));
    _Bool isY = (y >= button.y) && (y <= (button.y + button.h));
    _Bool isTrue = isX && isY;
    return isTrue;
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

        case UI_BTN_SCOPE_SCALE_NEG: {
            int pointCount = 2;
            SDL_FPoint points[2];
            if (btnHalfWidth >= 10.f) {
                points[0].x = btnCenterX - 4.f;
                points[1].x = btnCenterX + 4.f;
            }
            else {
                points[0].x = btnCenterX - (btnHalfWidth * 0.3f);
                points[1].x = btnCenterX + (btnHalfWidth * 0.3f);
            }
            points[0].y = btnCenterY;
            points[1].y = btnCenterY;
            SDL_RenderLines(state->renderer, points, pointCount);
            break;
        }

        case UI_BTN_SCOPE_SCALE_POS: {
            int pointCount = 2;
            SDL_FPoint pointsH[2];
            SDL_FPoint pointsV[2];
            if (btnHalfWidth >= 10.f) {
                // horizontal bar
                pointsH[0].x = btnCenterX - 4.f;
                pointsH[0].y = btnCenterY;
                pointsH[1].x = btnCenterX + 4.f;
                pointsH[1].y = btnCenterY;
                // vertical bar
                pointsV[0].x = btnCenterX;
                pointsV[0].y = btnCenterY - 4.f;
                pointsV[1].x = btnCenterX;
                pointsV[1].y = btnCenterY + 4.f;
            }
            else {
                pointsH[0].x = btnCenterX - (btnHalfWidth * 0.3f);
                pointsH[0].y = btnCenterY;
                pointsH[1].x = btnCenterX + (btnHalfWidth * 0.3f);
                pointsH[1].y = btnCenterY;
                // vertical bar
                pointsV[0].x = btnCenterX;
                pointsV[0].y = btnCenterY - (btnHalfWidth * 0.3f);
                pointsV[1].x = btnCenterX;
                pointsV[1].y = btnCenterY + (btnHalfWidth * 0.3f);
            }
            SDL_RenderLines(state->renderer, pointsH, pointCount);
            SDL_RenderLines(state->renderer, pointsV, pointCount);
            break;
        }

        case UI_BTN_NONE:
        case UI_BTN_VOLUME:
        case UI_FIELD_PATH:
        default:
            break;
    }
}

void updateScope(appState *state) {
    /* ------------------------------ */
    /*   Initial Rendering Settings   */
    /* ------------------------------ */
    SDL_SetRenderDrawColor(state->renderer, COLOR_GREEN);

    /* -------------------- */
    /*   Main Scope Frame   */
    /* -------------------- */
    SDL_FRect scopeFrame = {
        .w = (float)(state->width * 3 / 4),
        .h = (float)state->height,
        .x = 0.f,
        .y = 0.f };
    state->scopeHeight = (int)scopeFrame.h - 1; // - 1 for safety due to type conversion
    state->scopeWidth = (int)scopeFrame.w - 1; // - 1 for safety due to type conversion
    SDL_RenderRect(state->renderer, &scopeFrame);
}

void updateSettings(appState *state) {
    /* ------------------------------ */
    /*   Initial Rendering Settings   */
    /* ------------------------------ */
    SDL_SetRenderDrawColor(state->renderer, COLOR_GREEN);
    TTF_SetTextColor(state->TEXT.text, COLOR_GREEN);

    /* --------------------- */
    /*   Default Variables   */
    /* --------------------- */
    float SECTION_H = 35.f;
    float ROW_TITLE_H = 20.f; // default title height in row
    float ROW_ELEMENT_H = 35.f; // default element height in row
    float ROW_H = ROW_TITLE_H + ROW_ELEMENT_H; // row height
    float settingsFrameW;
    float settingsFrameX;

    /* ----------------------- */
    /*   Main Settings Frame   */
    /* ----------------------- */
    SDL_FRect settingsFrame = {
        .h = (float)state->height,
        .w = (float)(state->width - state->scopeWidth),
        .x = (float)state->scopeWidth,
        .y = 0.f };
    settingsFrameW = settingsFrame.w - 2; // "- 2" j for aesthetic purposes
    settingsFrameX = settingsFrame.x + 1; // cuz the "- 2" above
    SDL_RenderRect(state->renderer, &settingsFrame);

    /* -------------------------- */
    /*   Settings Rows Elements   */
    /* -------------------------- */
    // [ROW 0]
    // WAV Section Title
    SDL_FRect *ttlWavSettings = &UI.titles.wavSettings;
    *ttlWavSettings = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = (ROW_H * 0) };
    UI_TEXT titleWAVSettings;
    SDL_RenderFillRect(state->renderer, ttlWavSettings);
    TTF_SetTextColor(state->TEXT.text, COLOR_BLACK);
    renderTitle(state, &titleWAVSettings, "WAV Settings", *ttlWavSettings);
    TTF_SetTextColor(state->TEXT.text, COLOR_GREEN);

    // [ROW 1]
    // WAV File Path Title
    SDL_FRect *ttlFilePath = &UI.titles.wavFilePath;
    *ttlFilePath = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = (ROW_H * 1) - SECTION_H };
    UI_TEXT titleWAVPath;
    renderTitle(state, &titleWAVPath, "WAV File Path", *ttlFilePath);
    SDL_RenderRect(state->renderer, ttlFilePath);
    // WAV File Path Field
    SDL_FRect *fldFilePath = &UI.fields.wavFilePath;
    *fldFilePath = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = (ROW_H * 1) + ROW_TITLE_H - SECTION_H };
    SDL_RenderRect(state->renderer, fldFilePath);
    renderPathFieldText(state, fldFilePath);

    // [ROW 2]
    // Controls Title
    SDL_FRect *ttlControls = &UI.titles.wavControls;
    *ttlControls = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = (ROW_H * 2) - SECTION_H };
    UI_TEXT titleWAVControls;
    renderTitle(state, &titleWAVControls, "WAV Audio Controls", *ttlControls);
    SDL_RenderRect(state->renderer, ttlControls);
    // Play Button
    SDL_FRect *btnPlay = &UI.buttons.wavPlay;
    *btnPlay = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 3,
        .x = settingsFrameX,
        .y = (ROW_H * 2) + ROW_TITLE_H - SECTION_H };
    SDL_RenderRect(state->renderer, btnPlay);
    drawSymbol(state, UI_BTN_PLAY, *btnPlay);
    // Pause Button
    SDL_FRect *btnPause = &UI.buttons.wavPause;
    *btnPause = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 3,
        .x = btnPlay->x + btnPlay->w, // to the right of Play Btn
        .y = (ROW_H * 2) + ROW_TITLE_H - SECTION_H };
    SDL_RenderRect(state->renderer, btnPause);
    drawSymbol(state, UI_BTN_PAUSE, *btnPause);
    // Resume Button
    SDL_FRect *btnResume = &UI.buttons.wavResume;
    *btnResume = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 3, // to the right of Pause Btn
        .x = btnPause->x + btnPause->w,
        .y = (ROW_H * 2) + ROW_TITLE_H - SECTION_H };
    SDL_RenderRect(state->renderer, btnResume);
    drawSymbol(state, UI_BTN_RESUME, *btnResume);

    // [ROW 3]
    // Volume Title
    SDL_FRect *ttlVolume = &UI.titles.wavVolume;
    *ttlVolume = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = (ROW_H * 3) - SECTION_H };
    UI_TEXT titleAudioVolume;
    renderTitle(state, &titleAudioVolume, "WAV Audio Volume", *ttlVolume);
    SDL_RenderRect(state->renderer, ttlVolume);
    // Volume Button Slider
    SDL_FRect volFrameBig = {
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = (ROW_H * 3) + ROW_TITLE_H - SECTION_H };
    SDL_RenderRect(state->renderer, &volFrameBig);
    SDL_FRect *btnVolume = &UI.buttons.wavVolume;
    *btnVolume = (SDL_FRect){
        .h = volFrameBig.h / 2,
        .w = volFrameBig.w - 20.f,
        .x = volFrameBig.x + 10.f,
        .y = volFrameBig.y + (volFrameBig.h / 4) };
    if (btnVolume->w == 0) btnVolume->w = 0.00001f;
    SDL_RenderRect(state->renderer, btnVolume);
    // ts is only UI representation of gain
    float UIgain = state->AUDIO.volumeGain;
    // clamps if for some reason < 0 || > 1
    if (UIgain < 0.f) UIgain = 0.f;
    if (UIgain > 1.f) UIgain = 1.f;
    SDL_FRect volBar = {
        .h = btnVolume->h,
        .w = btnVolume->w * UIgain,
        .x = btnVolume->x,
        .y = btnVolume->y };
    if (volBar.w == 0) volBar.w = 0.00001f;
    SDL_RenderFillRect(state->renderer, &volBar);

    // [ROW 4]
    // Scope Section Title
    SDL_FRect *ttlScopeSettings = &UI.titles.scopeSettings;
    *ttlScopeSettings = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = (ROW_H * 4) - SECTION_H };
    UI_TEXT titleScopeSettings;
    SDL_RenderFillRect(state->renderer, ttlScopeSettings);
    TTF_SetTextColor(state->TEXT.text, COLOR_BLACK);
    renderTitle(state, &titleScopeSettings, "Scope Settings", *ttlScopeSettings);
    TTF_SetTextColor(state->TEXT.text, COLOR_GREEN);

    // [ROW 5]
    // Scope Size Title
    SDL_FRect *ttlscopeScale = &UI.titles.scopeScale;
    *ttlscopeScale = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = (ROW_H * 5) - SECTION_H * 2 };
    UI_TEXT titlescopeScale;
    renderTitle(state, &titlescopeScale, "Scope Size", *ttlscopeScale);
    SDL_RenderRect(state->renderer, ttlscopeScale);
    // Scope Size Frame
    SDL_FRect scopeScaleFrame = {
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = (ROW_H * 5) + ROW_TITLE_H - SECTION_H * 2 };
    SDL_RenderRect(state->renderer, &scopeScaleFrame);
    // Scope Size Value Display
    SDL_FRect *scopeScaleVal = &UI.fields.scopeScale;
    *scopeScaleVal = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = (settingsFrameW * 0.60f),
        .x = settingsFrameX + ((settingsFrameW / 2) - ((settingsFrameW * 0.60f) / 2)),
        .y = (ROW_H * 5) + ROW_TITLE_H - SECTION_H * 2 };
    UI_TEXT textScopeScaleVal;
    char scaleValue[4];
    SDL_snprintf(scaleValue, 4, "%i", state->scopeScale);
    renderTitle(state, &textScopeScaleVal, scaleValue, *scopeScaleVal);
    SDL_RenderRect(state->renderer, scopeScaleVal);
    // Scope Size Negative Button
    SDL_FRect *scopeScaleNeg = &UI.buttons.scopeScaleNeg;
    *scopeScaleNeg = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = (settingsFrameW * 0.20f),
        .x = settingsFrameX,
        .y = (ROW_H * 5) + ROW_TITLE_H - SECTION_H * 2 };
    SDL_RenderRect(state->renderer, scopeScaleNeg);
    drawSymbol(state, UI_BTN_SCOPE_SCALE_NEG, *scopeScaleNeg);
    // Scope Size Positive Button
    SDL_FRect *scopeScalePos = &UI.buttons.scopeScalePos;
    *scopeScalePos = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = (settingsFrameW * 0.20f),
        .x = scopeScaleVal->x + scopeScaleVal->w,
        .y = (ROW_H * 5) + ROW_TITLE_H - SECTION_H * 2 };
    SDL_RenderRect(state->renderer, scopeScalePos);
    drawSymbol(state, UI_BTN_SCOPE_SCALE_POS, *scopeScalePos);
}

UI_BUTTONS getUIButtonEnum(float x, float y) {
    if (isMouseInButton(x, y, UI.fields.wavFilePath))
        return UI_FIELD_PATH;

    if (isMouseInButton(x, y, UI.buttons.wavPlay))
        return UI_BTN_PLAY;

    if (isMouseInButton(x, y, UI.buttons.wavPause))
        return UI_BTN_PAUSE;

    if (isMouseInButton(x, y, UI.buttons.wavResume))
        return UI_BTN_RESUME;

    if (isMouseInButton(x, y, UI.buttons.wavVolume))
        return UI_BTN_VOLUME;

    if (isMouseInButton(x, y, UI.buttons.scopeScaleNeg))
        return UI_BTN_SCOPE_SCALE_NEG;

    if (isMouseInButton(x, y, UI.buttons.scopeScalePos))
        return UI_BTN_SCOPE_SCALE_POS;

    return UI_BTN_NONE;
}

SDL_FRect getUIButtonRect(UI_BUTTONS button) {
    switch (button) {
        case UI_FIELD_PATH:
            return UI.fields.wavFilePath;

        case UI_BTN_PLAY:
            return UI.buttons.wavPlay;

        case UI_BTN_PAUSE:
            return UI.buttons.wavPause;

        case UI_BTN_RESUME:
            return UI.buttons.wavResume;

        case UI_BTN_VOLUME:
            return UI.buttons.wavVolume;

        case UI_BTN_SCOPE_SCALE_NEG:
            return UI.buttons.scopeScaleNeg;

        case UI_BTN_SCOPE_SCALE_POS:
            return UI.buttons.scopeScalePos;

        case UI_BTN_NONE:
            break;
    }
    return (SDL_FRect) { 0 };
}