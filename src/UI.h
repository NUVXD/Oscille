#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include "SDL3/SDL.h"

typedef struct appState appState;
typedef struct SDL_FRect SDL_FRect;

typedef enum UI_COLOR_ENUMS {
    UI_COLOR_NULL,
    UI_COLOR_WHITE,
    UI_COLOR_RED,
    UI_COLOR_ORANGE,
    UI_COLOR_YELLOW,
    UI_COLOR_GREEN,
    UI_COLOR_BLUE,
    UI_COLOR_PINK,
    UI_COLOR_PURPLE,

    UI_COLOR_COUNT
} UI_COLOR_ENUMS;

typedef enum UI_BUTTON_ENUMS {
    UI_NONE,
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
    UI_BTN_SCOPE_MODE_LINES,
    UI_BTN_COMPACT_SECTION_WAV,
    UI_BTN_COMPACT_SECTION_SCOPE,
    UI_BTN_MENU_OPENFILE,
    UI_BTN_SCOPE_INVERT_X,
    UI_BTN_SCOPE_INVERT_Y,

    UI_BUTTON_COUNT
} UI_BUTTON_ENUMS;

typedef enum UI_BOOL_ENUMS {
    MENU_IS_HOVERING_COMPACT_FILE,
    MENU_IS_HOVERING_EXPANDED_FILE,
    SETTINGS_IS_WAV_SECTION_COMPACT,
    SETTINGS_IS_SCOPE_SECTION_COMPACT,
    SETTINGS_IS_SCOPE_INVERTED_X,
    SETTINGS_IS_SCOPE_INVERTED_Y,

    UI_BOOL_COUNT
} UI_BOOL_ENUMS;

typedef struct UI_ELEMENT {
    UI_BUTTON_ENUMS ID;
    SDL_FRect rect;
} UI_ELEMENT, UI_BUTTON, UI_DISPLAY, UI_TITLE, UI_FIELD;

typedef struct UI_COLOR {
    UI_COLOR_ENUMS ID;
    SDL_FRect rect;
} UI_COLOR;

typedef struct UI_TEXT {
    char *text;
    int w;
    int h;
    float x;
    float y;
} UI_TEXT;

extern SDL_Color InterfaceColor;
extern void updateScope(struct appState *state);
extern void updateSettings(struct appState* state);
extern void updateTopMenu(struct appState *state);
extern UI_ELEMENT getUIElement(float x, float y);
extern SDL_Color getRGBColor(UI_COLOR_ENUMS RGB_colorEnum);
extern UI_COLOR getUIColorBtn(float x, float y);
extern _Bool *getUIBoolean(UI_BOOL_ENUMS boolEnum);

#endif