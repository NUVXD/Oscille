#include <string.h>
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "UI.h"
#include "appstate.h"

/* --------------- */
/*   UI ELEMENTS   */
/* --------------- */
static struct {
    /*   STATIC ELEMENTS   */
    struct {
        // TITLES
        struct {
            SDL_FRect wavSettings;
            SDL_FRect wavFilePath;
            SDL_FRect wavControls;
            SDL_FRect wavVolume;
            SDL_FRect scopeSettings;
            SDL_FRect scopeScale;
            SDL_FRect scopeMaxPoints;
            SDL_FRect scopeMode;
        } Title;
        // DISPLAYS
        struct {
            UI_ELEMENT scopeScale;
            UI_ELEMENT scopeMaxPoints;
        } Display;
    } STATIC;
    /*   INTERACTIVE ELEMENTS   */
    struct {
        // BUTTONS
        struct {
            UI_ELEMENT wavPlay;
            UI_ELEMENT wavPause;
            UI_ELEMENT wavResume;
            UI_ELEMENT wavVolume;
            UI_ELEMENT scopeScaleNeg;
            UI_ELEMENT scopeScalePos;
            UI_ELEMENT scopeMaxPointsNeg;
            UI_ELEMENT scopeMaxPointsPos;
            UI_ELEMENT scopeModePoints;
            UI_ELEMENT scopeModeLines;
            UI_ELEMENT menuOpenFile;
        } Button;
        // FIELDS
        struct {
            UI_ELEMENT wavFilePath;
        } Field;
    } INTERACTIVE;
    //
} UI;

static UI_ELEMENT *const UI_INTERACTIVES[] = {
    &UI.INTERACTIVE.Button.wavPlay,
    &UI.INTERACTIVE.Button.wavPause,
    &UI.INTERACTIVE.Button.wavResume,
    &UI.INTERACTIVE.Button.wavVolume,
    &UI.INTERACTIVE.Button.scopeScaleNeg,
    &UI.INTERACTIVE.Button.scopeScalePos,
    &UI.INTERACTIVE.Button.scopeMaxPointsNeg,
    &UI.INTERACTIVE.Button.scopeMaxPointsPos,
    &UI.INTERACTIVE.Button.scopeModePoints,
    &UI.INTERACTIVE.Button.scopeModeLines,
    &UI.INTERACTIVE.Field.wavFilePath,
    &UI.INTERACTIVE.Button.menuOpenFile
};

static void renderTitle(appState *state, UI_TEXT *UI_Text, TTF_FontStyleFlags style, char *textString, SDL_FRect rect) {
    UI_Text->text = textString;
    TTF_SetTextFont(state->TEXT.text, state->TEXT.font[FONT_MONOSPACE_NORMAL]);

    if (style)
        TTF_SetFontStyle(state->TEXT.font[FONT_MONOSPACE_NORMAL], style);
    else
        TTF_SetFontStyle(state->TEXT.font[FONT_MONOSPACE_NORMAL], TTF_STYLE_NORMAL);

    TTF_SetTextString(state->TEXT.text, UI_Text->text, 0);
    TTF_GetTextSize(state->TEXT.text, &UI_Text->w, &UI_Text->h);
    UI_Text->x = (rect.x + (rect.w / 2)) - (UI_Text->w / 2);
    UI_Text->y = (rect.y + (rect.h / 2)) - (UI_Text->h / 2);
    // keeps them from overlapping on the left
    if (UI_Text->x < rect.x + 5.f)
        UI_Text->x = rect.x + 5.f;
    TTF_DrawRendererText(state->TEXT.text, UI_Text->x, UI_Text->y);
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
    _Bool isInButtonX = (x >= button.x) && (x <= (button.x + button.w));
    _Bool isInButtonY = (y >= button.y) && (y <= (button.y + button.h));
    _Bool isInButton = (isInButtonX && isInButtonY);
    return isInButton;
}

static void setRowFrom(SDL_FRect rect, float *rowStartY) {
    *rowStartY = (rect.y + rect.h);
}

static void drawSymbol(appState *state, UI_ELEMENT element) {
    float btnHalfWidth = element.rect.w / 2;
    float btnHalfHeight = element.rect.h / 2;
    float btnCenterX = element.rect.x + btnHalfWidth;
    float btnCenterY = element.rect.y + btnHalfHeight;

    switch (element.ID) {
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
            float symbolLeft = element.rect.x + innerMargin;
            float symbolRight = element.rect.x + element.rect.w - innerMargin;
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
        case UI_BTN_SCOPE_MAX_POINTS_NEG:
        case UI_BTN_SCOPE_MAX_POINTS_POS:
        case UI_BTN_SCOPE_MODE_POINTS:
        case UI_BTN_SCOPE_MODE_LINES:
        case UI_FIELD_PATH:
        case UI_BTN_MENU_OPENFILE:
        default: break;
    }
}

static enum {
    MENU_IS_HOVERING_COMPACT_FILE,
    MENU_IS_HOVERING_EXPANDED_FILE,
    MENU_BOOL_COUNT
} MENU_BOOL_ENUMS;
static _Bool menuBooleans[MENU_BOOL_COUNT + 1];

void updateTopMenu(appState *state) {
    /* ------------------------------ */
    /*   Initial Rendering Settings   */
    /* ------------------------------ */
    SDL_SetRenderDrawColor(state->renderer, COLOR_GREEN);
    TTF_SetTextColor(state->TEXT.text, COLOR_BLACK);

    /* --------------------- */
    /*   Default Variables   */
    /* --------------------- */
    float colStartX = 1.f;
    float menuFrameH = 25.f;
    float ELEMENT_FRAME_W = 50.f;
    float ELEMENT_FRAME_H = menuFrameH - 2.f;
    float ELEMENT_FRAME_Y = (menuFrameH - ELEMENT_FRAME_H) / 2.f;
    float DROPDOWN_TOP_GAP = 5.f;

    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    /* ------------------- */
    /*   Main Menu Frame   */
    /* ------------------- */
    SDL_FRect menuFrame = {
        .h = menuFrameH,
        .w = (float)state->width,
        .x = 0.f,
        .y = 0.f };
    SDL_RenderFillRect(state->renderer, &menuFrame);

    SDL_FRect menuFrameOutline = {
    .h = ELEMENT_FRAME_H,
    .w = (float)state->width - 2.f,
    .x = 1.f,
    .y = 1.f };
    SDL_SetRenderDrawColor(state->renderer, COLOR_BLACK);
    SDL_RenderRect(state->renderer, &menuFrameOutline);

    /* ------------------------- */
    /*   Menu Columns Elements   */
    /* ------------------------- */
    SDL_SetRenderDrawColor(state->renderer, COLOR_BLACK);

    /*   File Dropdown Compact   */
    UI_ELEMENT btnMenuCmpctFile;
    btnMenuCmpctFile.ID = UI_BTN_NONE;
    btnMenuCmpctFile.rect = (SDL_FRect){
        .h = ELEMENT_FRAME_H,
        .w = ELEMENT_FRAME_W,
        .x = colStartX,
        .y = ELEMENT_FRAME_Y };
    UI_TEXT txtMenuCmpctFile;
    renderTitle(state, &txtMenuCmpctFile, TTF_STYLE_BOLD, "File", btnMenuCmpctFile.rect);
    SDL_RenderRect(state->renderer, &btnMenuCmpctFile.rect);

    /*   File Dropdown Expanded   */
    float ddElementCount = 1.f;
    _Bool *isHoverCmpctFile = &menuBooleans[MENU_IS_HOVERING_COMPACT_FILE];
    _Bool *isHoverExpndFile = &menuBooleans[MENU_IS_HOVERING_EXPANDED_FILE];

    UI_ELEMENT btnMenuExpndFile;
    btnMenuExpndFile.ID = UI_BTN_NONE;
    btnMenuExpndFile.rect = (SDL_FRect){
        .h = (ELEMENT_FRAME_H * ddElementCount) + DROPDOWN_TOP_GAP * 1.5f,
        .w = btnMenuCmpctFile.rect.w * 2.f,
        .x = btnMenuCmpctFile.rect.x,
        .y = btnMenuCmpctFile.rect.y + btnMenuCmpctFile.rect.h };

    if (isMouseInButton(mouseX, mouseY, btnMenuCmpctFile.rect))
        *isHoverCmpctFile = 1;

    if (isMouseInButton(mouseX, mouseY, btnMenuExpndFile.rect) && *isHoverCmpctFile)
        *isHoverExpndFile = 1;
    else
        *isHoverExpndFile = 0;

    if (*isHoverExpndFile || *isHoverCmpctFile) {
        SDL_SetRenderDrawColor(state->renderer, COLOR_GREEN);
        SDL_RenderFillRect(state->renderer, &btnMenuExpndFile.rect);
    }

    if (!isMouseInButton(mouseX, mouseY, btnMenuCmpctFile.rect) && !*isHoverExpndFile) {
        *isHoverExpndFile = 0;
        *isHoverCmpctFile = 0;
    }

    /*   File Dropdown Element: Open File   */
    UI_ELEMENT *btnMenuOpenFile = &UI.INTERACTIVE.Button.menuOpenFile;
    btnMenuOpenFile->ID = UI_BTN_NONE;
    btnMenuOpenFile->rect = (SDL_FRect){
        .h = ELEMENT_FRAME_H,
        .w = btnMenuExpndFile.rect.w - 1.f,
        .x = btnMenuExpndFile.rect.x,
        .y = btnMenuExpndFile.rect.y + DROPDOWN_TOP_GAP };
    UI_TEXT txtOpenFile;
    if (*isHoverExpndFile || *isHoverCmpctFile) {
        SDL_SetRenderDrawColor(state->renderer, COLOR_BLACK);
        renderTitle(state, &txtOpenFile, TTF_STYLE_NORMAL, "Open File", btnMenuOpenFile->rect);
        SDL_RenderRect(state->renderer, &btnMenuOpenFile->rect);
        btnMenuOpenFile->ID = UI_BTN_MENU_OPENFILE;
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
    // the 25s are because of menuFrameH, hardcoded j for now
    SDL_FRect scopeFrame = {
        .w = (float)(state->width * 3 / 4),
        .h = (float)(state->height - 25.f),
        .x = 0.f,
        .y = 25.f };
    state->SCOPE.height = (int)scopeFrame.h - 1; // - 1 for safety due to type conversion
    state->SCOPE.width = (int)scopeFrame.w - 1; // - 1 for safety due to type conversion
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
    float ROW_TITLE_H = 20.f; // default title height in row
    float ROW_ELEMENT_H = 30.f; // default element height in row
    float rowStartY = 0.f;
    float settingsFrameW;
    float settingsFrameX;

    /* ----------------------- */
    /*   Main Settings Frame   */
    /* ----------------------- */
    // the 25s are because of menuFrameH, hardcoded j for now
    SDL_FRect settingsFrame = {
        .h = (float)(state->height - 25.f),
        .w = (float)(state->width - state->SCOPE.width),
        .x = (float)state->SCOPE.width,
        .y = 25.f };
    settingsFrameW = settingsFrame.w - 2; // "- 2" j for aesthetic purposes
    settingsFrameX = settingsFrame.x + 1; // cuz the "- 2" above
    SDL_RenderRect(state->renderer, &settingsFrame);

    /* -------------------------- */
    /*   Settings Rows Elements   */
    /* -------------------------- */

    /* --------- */
    /*   ROW 0   */
    /* --------- */
    rowStartY = settingsFrame.y;

    /*   WAV Settings Section Title   */
    SDL_FRect *ttlWavSettings = &UI.STATIC.Title.wavSettings;
    *ttlWavSettings = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = rowStartY };
    setRowFrom(*ttlWavSettings, &rowStartY);
    UI_TEXT txtWAVSettings;
    SDL_RenderFillRect(state->renderer, ttlWavSettings);
    TTF_SetTextColor(state->TEXT.text, COLOR_BLACK);
    renderTitle(state, &txtWAVSettings, TTF_STYLE_BOLD, "WAV Settings", *ttlWavSettings);
    TTF_SetTextColor(state->TEXT.text, COLOR_GREEN);

    /* --------- */
    /*   ROW 1   */
    /* --------- */

    /*   WAV File Path Title   */
    SDL_FRect *ttlFilePath = &UI.STATIC.Title.wavFilePath;
    *ttlFilePath = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = rowStartY };
    setRowFrom(*ttlFilePath, &rowStartY);
    UI_TEXT txtWAVPath;
    renderTitle(state, &txtWAVPath, TTF_STYLE_NORMAL, "WAV File Path", *ttlFilePath);
    SDL_RenderRect(state->renderer, ttlFilePath);

    /*   WAV File Path Field   */
    UI_ELEMENT *fldWavFilePath = &UI.INTERACTIVE.Field.wavFilePath;
    fldWavFilePath->ID = UI_FIELD_PATH;
    fldWavFilePath->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = rowStartY };
    setRowFrom(fldWavFilePath->rect, &rowStartY);
    SDL_RenderRect(state->renderer, &fldWavFilePath->rect);
    renderPathFieldText(state, &fldWavFilePath->rect);

    /* --------- */
    /*   ROW 2   */
    /* --------- */

    /*   Controls Title   */
    SDL_FRect *ttlWavControls = &UI.STATIC.Title.wavControls;
    *ttlWavControls = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = rowStartY };
    setRowFrom(*ttlWavControls, &rowStartY);
    UI_TEXT txtWAVControls;
    renderTitle(state, &txtWAVControls, TTF_STYLE_NORMAL, "WAV Audio Controls", *ttlWavControls);
    SDL_RenderRect(state->renderer, ttlWavControls);

    /*   Play Button   */
    UI_ELEMENT *btnWavPlay = &UI.INTERACTIVE.Button.wavPlay;
    btnWavPlay->ID = UI_BTN_PLAY;
    btnWavPlay->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 3,
        .x = settingsFrameX,
        .y = rowStartY };
    SDL_RenderRect(state->renderer, &btnWavPlay->rect);
    drawSymbol(state, *btnWavPlay);

    /*   Pause Button   */
    UI_ELEMENT *btnWavPause = &UI.INTERACTIVE.Button.wavPause;
    btnWavPause->ID = UI_BTN_PAUSE;
    btnWavPause->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 3,
        .x = btnWavPlay->rect.x + btnWavPlay->rect.w, // to the right of Play Btn
        .y = rowStartY };
    SDL_RenderRect(state->renderer, &btnWavPause->rect);
    drawSymbol(state, *btnWavPause);

    /*   Resume Button   */
    UI_ELEMENT *btnWavResume = &UI.INTERACTIVE.Button.wavResume;
    btnWavResume->ID = UI_BTN_RESUME;
    btnWavResume->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 3, // to the right of Pause Btn
        .x = btnWavPause->rect.x + btnWavPause->rect.w,
        .y = rowStartY };
    setRowFrom(btnWavResume->rect, &rowStartY);
    SDL_RenderRect(state->renderer, &btnWavResume->rect);
    drawSymbol(state, *btnWavResume);

    /* --------- */
    /*   ROW 3   */
    /* --------- */

    /*   Volume Title   */
    SDL_FRect *ttlVolume = &UI.STATIC.Title.wavVolume;
    *ttlVolume = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = rowStartY };
    setRowFrom(*ttlVolume, &rowStartY);
    UI_TEXT txtAudioVolume;
    renderTitle(state, &txtAudioVolume, TTF_STYLE_NORMAL, "WAV Audio Volume", *ttlVolume);
    SDL_RenderRect(state->renderer, ttlVolume);

    /*   Volume Button Frame   */
    SDL_FRect volFrameBig = {
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = rowStartY };
    setRowFrom(volFrameBig, &rowStartY);
    SDL_RenderRect(state->renderer, &volFrameBig);

    /*   Volume Button Slider   */
    UI_ELEMENT *btnWavVolume = &UI.INTERACTIVE.Button.wavVolume;
    btnWavVolume->ID = UI_BTN_VOLUME;
    btnWavVolume->rect = (SDL_FRect){
        .h = volFrameBig.h / 2,
        .w = volFrameBig.w - 20.f,
        .x = volFrameBig.x + 10.f,
        .y = volFrameBig.y + (volFrameBig.h / 4) };
    if (btnWavVolume->rect.w == 0) btnWavVolume->rect.w = 0.00001f;
    SDL_RenderRect(state->renderer, &btnWavVolume->rect);
    //
    float UIgain = state->AUDIO.volumeGain; // this is only UI representation of gain
    if (UIgain < 0.f) UIgain = 0.f; // clamps if for some reason < 0.f
    if (UIgain > 1.f) UIgain = 1.f; // clamps if for some reason > 1.f
    //
    SDL_FRect volBar = {
        .h = btnWavVolume->rect.h,
        .w = btnWavVolume->rect.w * UIgain,
        .x = btnWavVolume->rect.x,
        .y = btnWavVolume->rect.y };
    if (volBar.w == 0) volBar.w = 0.00001f;
    SDL_RenderFillRect(state->renderer, &volBar);

    /* --------- */
    /*   ROW 4   */
    /* --------- */

    /*   Scope Settings Section Title   */
    SDL_FRect *ttlScopeSettings = &UI.STATIC.Title.scopeSettings;
    *ttlScopeSettings = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = rowStartY };
    setRowFrom(*ttlScopeSettings, &rowStartY);
    UI_TEXT txtScopeSettings;
    SDL_RenderFillRect(state->renderer, ttlScopeSettings);
    TTF_SetTextColor(state->TEXT.text, COLOR_BLACK);
    renderTitle(state, &txtScopeSettings, TTF_STYLE_BOLD, "Scope Settings", *ttlScopeSettings);
    TTF_SetTextColor(state->TEXT.text, COLOR_GREEN);

    /* --------- */
    /*   ROW 5   */
    /* --------- */

    /*   Scope Scale Title   */
    SDL_FRect *ttlScopeScale = &UI.STATIC.Title.scopeScale;
    *ttlScopeScale = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = rowStartY };
    setRowFrom(*ttlScopeScale, &rowStartY);
    UI_TEXT txtScopeScale;
    renderTitle(state, &txtScopeScale, TTF_STYLE_NORMAL, "Scope Scale", *ttlScopeScale);
    SDL_RenderRect(state->renderer, ttlScopeScale);

    /*   Scope Scale Value Display   */
    UI_ELEMENT *dspScopeScale = &UI.STATIC.Display.scopeScale;
    dspScopeScale->ID = UI_BTN_NONE;
    dspScopeScale->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = (settingsFrameW * 0.60f), // 2/4
        .x = settingsFrameX + ((settingsFrameW / 2) - ((settingsFrameW * 0.60f) / 2)),
        .y = rowStartY };
    UI_TEXT txtScaleVal;
    char scaleValue[4];
    SDL_snprintf(scaleValue, 5, "%i%%", state->SCOPE.scale);
    renderTitle(state, &txtScaleVal, TTF_STYLE_NORMAL, scaleValue, dspScopeScale->rect);
    SDL_RenderRect(state->renderer, &dspScopeScale->rect);

    /*   Scope Scale Negative Button   */
    UI_ELEMENT *btnScopeScaleNeg = &UI.INTERACTIVE.Button.scopeScaleNeg;
    btnScopeScaleNeg->ID = UI_BTN_SCOPE_SCALE_NEG;
    btnScopeScaleNeg->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = (settingsFrameW * 0.20f), // 1/4
        .x = settingsFrameX,
        .y = rowStartY };
    SDL_RenderRect(state->renderer, &btnScopeScaleNeg->rect);
    drawSymbol(state, *btnScopeScaleNeg);

    /*   Scope Scale Positive Button   */
    UI_ELEMENT *btnScopeScalePos = &UI.INTERACTIVE.Button.scopeScalePos;
    btnScopeScalePos->ID = UI_BTN_SCOPE_SCALE_POS;
    btnScopeScalePos->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = (settingsFrameW * 0.20f), // 1/4
        .x = dspScopeScale->rect.x + dspScopeScale->rect.w,
        .y = rowStartY };
    setRowFrom(btnScopeScalePos->rect, &rowStartY);
    SDL_RenderRect(state->renderer, &btnScopeScalePos->rect);
    drawSymbol(state, *btnScopeScalePos);

    /* --------- */
    /*   ROW 6   */
    /* --------- */

    /*   Scope Max Points Title   */
    SDL_FRect *ttlScopeMaxPoints = &UI.STATIC.Title.scopeMaxPoints;
    *ttlScopeMaxPoints = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = rowStartY };
    setRowFrom(*ttlScopeMaxPoints, &rowStartY);
    UI_TEXT titleScopeMaxPoints;
    renderTitle(state, &titleScopeMaxPoints, TTF_STYLE_NORMAL, "Max Points", *ttlScopeMaxPoints);
    SDL_RenderRect(state->renderer, ttlScopeMaxPoints);

    /*   Scope Max Points Value Display   */
    UI_ELEMENT *dspScopeMaxPoints = &UI.STATIC.Display.scopeMaxPoints;
    dspScopeMaxPoints->ID = UI_BTN_NONE;
    dspScopeMaxPoints->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = (settingsFrameW * 0.60f), // 2/4
        .x = settingsFrameX + ((settingsFrameW / 2) - ((settingsFrameW * 0.60f) / 2)),
        .y = rowStartY };
    UI_TEXT textScopeMaxPointsVal;
    char maxPointsValue[20];
    SDL_snprintf(maxPointsValue, 21, "%u", state->SCOPE.maxPoints);
    renderTitle(state, &textScopeMaxPointsVal, TTF_STYLE_NORMAL, maxPointsValue, dspScopeMaxPoints->rect);
    SDL_RenderRect(state->renderer, &dspScopeMaxPoints->rect);

    /*   Scope Max Points Negative Button   */
    UI_ELEMENT *btnScopeMaxPointsNeg = &UI.INTERACTIVE.Button.scopeMaxPointsNeg;
    btnScopeMaxPointsNeg->ID = UI_BTN_SCOPE_MAX_POINTS_NEG;
    btnScopeMaxPointsNeg->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = (settingsFrameW * 0.20f), // 1/4
        .x = settingsFrameX,
        .y = rowStartY };
    SDL_RenderRect(state->renderer, &btnScopeMaxPointsNeg->rect);
    drawSymbol(state, (UI_ELEMENT) { .ID = btnScopeScaleNeg->ID, .rect = btnScopeMaxPointsNeg->rect }); // easier to clone here than in drawSymbol

    /*   Scope Max Points Positive Button   */
    UI_ELEMENT *btnScopeMaxPointsPos = &UI.INTERACTIVE.Button.scopeMaxPointsPos;
    btnScopeMaxPointsPos->ID = UI_BTN_SCOPE_MAX_POINTS_POS;
    btnScopeMaxPointsPos->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = (settingsFrameW * 0.20f), // 1/4
        .x = dspScopeMaxPoints->rect.x + dspScopeMaxPoints->rect.w,
        .y = rowStartY };
    setRowFrom(btnScopeMaxPointsPos->rect, &rowStartY);
    SDL_RenderRect(state->renderer, &btnScopeMaxPointsPos->rect);
    drawSymbol(state, (UI_ELEMENT) { .ID = btnScopeScalePos->ID, .rect = btnScopeMaxPointsPos->rect }); // easier to clone here than in drawSymbol

    /* --------- */
    /*   ROW 7   */
    /* --------- */

    /*   Scope Mode Title   */
    SDL_FRect *ttlScopeMode = &UI.STATIC.Title.scopeMode;
    *ttlScopeMode = (SDL_FRect){
        .h = ROW_TITLE_H,
        .w = settingsFrameW / 1,
        .x = settingsFrameX,
        .y = rowStartY };
    setRowFrom(*ttlScopeMode, &rowStartY);
    UI_TEXT titleScopeMode;
    renderTitle(state, &titleScopeMode, TTF_STYLE_NORMAL, "Draw Mode", *ttlScopeMode);
    SDL_RenderRect(state->renderer, ttlScopeMode);

    /*   Scope Mode Points   */
    UI_ELEMENT *btnScopeModePoints = &UI.INTERACTIVE.Button.scopeModePoints;
    btnScopeModePoints->ID = UI_BTN_SCOPE_MODE_POINTS;
    btnScopeModePoints->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 2,
        .x = settingsFrameX,
        .y = rowStartY };
    UI_TEXT textModePoints;
    if (state->SCOPE.mode == 0)
        renderTitle(state, &textModePoints, TTF_STYLE_NORMAL, "> Points <", btnScopeModePoints->rect);
    else
        renderTitle(state, &textModePoints, TTF_STYLE_NORMAL, "Points", btnScopeModePoints->rect);
    SDL_RenderRect(state->renderer, &btnScopeModePoints->rect);

    /*   Scope Mode Lines   */
    UI_ELEMENT *btnScopeModeLines = &UI.INTERACTIVE.Button.scopeModeLines;
    btnScopeModeLines->ID = UI_BTN_SCOPE_MODE_LINES;
    btnScopeModeLines->rect = (SDL_FRect){
        .h = ROW_ELEMENT_H,
        .w = settingsFrameW / 2,
        .x = settingsFrameX + btnScopeModePoints->rect.w,
        .y = rowStartY };
    setRowFrom(btnScopeModeLines->rect, &rowStartY);
    UI_TEXT textModeLines;
    if (state->SCOPE.mode == 1)
        renderTitle(state, &textModeLines, TTF_STYLE_NORMAL, "> Lines <", btnScopeModeLines->rect);
    else
        renderTitle(state, &textModeLines, TTF_STYLE_NORMAL, "Lines", btnScopeModeLines->rect);
    SDL_RenderRect(state->renderer, &btnScopeModeLines->rect);
}

UI_ELEMENT getUIElement(float x, float y) {
    size_t elementsLen = sizeof(UI_INTERACTIVES) / sizeof(UI_INTERACTIVES[0]);
    for (size_t i = 0; i < elementsLen; i++) {
        UI_ELEMENT *element = UI_INTERACTIVES[i];
        if (isMouseInButton(x, y, element->rect))
            return *element;
    }
    return (UI_ELEMENT) { .ID = UI_BTN_NONE, .rect = { 0 } };
}