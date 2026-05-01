#ifndef UI_H
#define UI_H

struct appState;

typedef enum UI_BUTTONS {
    UI_BTN_NONE,
    UI_FIELD_PATH,
    UI_BTN_PLAY,
    UI_BTN_PAUSE,
    UI_BTN_RESUME,
	UI_BTN_VOLUME,
    UI_BTN_SCOPE_SCALE_NEG,
    UI_BTN_SCOPE_SCALE_POS,
    UI_BTN_SCOPE_MAX_POINTS_NEG,
    UI_BTN_SCOPE_MAX_POINTS_POS,
    UI_BTN_SCOPE_MODE_POINTS,
    UI_BTN_SCOPE_MODE_LINES
} UI_BUTTONS;

typedef struct UI_ELEMENT {
    UI_BUTTONS ID;
    SDL_FRect rect;
} UI_ELEMENT;

typedef struct UI_TEXT {
    char *text;
    int w;
    int h;
    float x;
    float y;
} UI_TEXT;

extern void updateScope(struct appState *state);
extern void updateSettings(struct appState* state);
extern UI_ELEMENT getUIElement(float x, float y);

#endif