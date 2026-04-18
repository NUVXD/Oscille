#ifndef UI_H
#define UI_H

#include "appstate.h"

typedef enum {
    UI_BTN_NONE,
    UI_BTN_PLAY,
    UI_BTN_PAUSE,
    UI_BTN_RESUME,
	UI_BTN_VOLUME
} UI_BTN_CLICKED;

extern void updateScope(appState *state);
extern void updateSettings(appState* state);
extern UI_BTN_CLICKED getUIButton(float x, float y);

#endif