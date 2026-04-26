#ifndef UI_H
#define UI_H

struct appState;

typedef enum {
    UI_BTN_NONE,
    UI_FIELD_PATH,
    UI_BTN_PLAY,
    UI_BTN_PAUSE,
    UI_BTN_RESUME,
	UI_BTN_VOLUME,
    UI_BTN_SCOPE_SCALE_NEG,
    UI_BTN_SCOPE_SCALE_POS,
} UI_BUTTONS;

extern void updateScope(struct appState *state);
extern void updateSettings(struct appState* state);
extern UI_BUTTONS getUIButtonEnum(float x, float y);
extern SDL_FRect getUIButtonRect(UI_BUTTONS button);

#endif