#ifndef UI_H
#define UI_H

#include "appstate.h"

typedef enum {
    UI_BTN_NONE,
    UI_FIELD_PATH,
    UI_BTN_PLAY,
    UI_BTN_PAUSE,
    UI_BTN_RESUME,
	UI_BTN_VOLUME
} UI_BUTTONS;

extern void updateScope(appState *state);
extern void updateSettings(appState* state);
extern UI_BUTTONS getUIButton(float x, float y);

#endif