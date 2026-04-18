#include "SDL3/SDL.h"
#include "events.h"
#include "appstate.h"
#include "UI.h"

int appEvents(appState *state, SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return 1;
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_ESCAPE)
                return 1;
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            state->width = event->window.data1;
            state->height = event->window.data2;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event->button.button == SDL_BUTTON_LEFT)
            {
                float x, y;
                SDL_GetMouseState(&x, &y);
                UI_BTN_CLICKED button = getUIButton(x, y);
                switch (button) {
                    case UI_BTN_PLAY:
                        SDL_Log("play button clicked\n");
                        break;
                    case UI_BTN_PAUSE:
                        SDL_Log("pause button clicked\n");
                        break;
                    case UI_BTN_RESUME:
                        SDL_Log("resume button clicked\n");
                        break;
                    case UI_BTN_VOLUME:
                        SDL_Log("volume button clicked\n");
                        break;
                    case UI_BTN_NONE:
                        SDL_Log("left mouse click outside buttons at XY coordinates %2.f %2.f\n", x, y);
                        break;
                }
            }
            break;
    }

    return 0;
}