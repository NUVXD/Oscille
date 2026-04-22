#include "SDL3/SDL.h"
#include "WAV.h"
#include "events.h"
#include "appstate.h"
#include "audio.h"
#include "UI.h"

static void printDebugHeaderInfo(appState *state) {
    SDL_Log("\n");
    SDL_Log("FILE SIZE ACCORDING TO FILE: %zu BYTES\n", state->header.Riff.fileSize);
    SDL_Log("MAIN CHUNK: \n");
    SDL_Log("| RiffID: %s\n", state->header.Riff.ID);
    SDL_Log("| fileFormatID: % s\n", state->header.Riff.fileFormatID);
    SDL_Log("SUBCHUNK 1: \n");
    SDL_Log("| FormatID: %s\n", state->header.Format.ID);
    SDL_Log("| BitsPerSample: %u\n", state->header.Format.bitsPerSample);
    SDL_Log("| Frequency: %u\n", state->header.Format.frequency);
    SDL_Log("| BytesPerSec: %u\n", state->header.Format.bytesPerSec);
    SDL_Log("\n");
};

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
            if (event->button.button == SDL_BUTTON_LEFT) {
                float x, y;
                SDL_GetMouseState(&x, &y);

                UI_BUTTONS button = getUIButtonEnum(x, y);
                SDL_FRect buttonRect = getUIButtonRect(button);
                switch (button) {
                    case UI_FIELD_PATH: {
                        SDL_Log("WAV filePath field clicked\n");
                        break;
                    }

                    case UI_BTN_PLAY: {
                        SDL_Log("play button clicked\n");

                        destroyAudio(state); // should make it return something in the future - as a check
                        freeWAV(&state->wavBuffer);

                        _Bool isError = parseWAV(&state->header, &state->wavBuffer);
                        if (isError) {
                            SDL_Log("error with loading or parsing WAV file\n");
                            if (state->wavBuffer) {
                                free(state->wavBuffer);
                                state->wavBuffer = (void *)0;
                            }
                            break;
                        }
                        printDebugHeaderInfo(state);

                        initAudio(state);

                        break;
                    }

                    case UI_BTN_PAUSE:
                        SDL_Log("pause button clicked\n");

                        pauseAudio(state);
                        break;

                    case UI_BTN_RESUME:
                        SDL_Log("resume button clicked\n");

                        resumeAudio(state);
                        break;

                    case UI_BTN_VOLUME: {
                        if (buttonRect.w > 0.f) {
                            /**
                             * [mouse x coord] - [rect x coord] = mouse's offset in x-axis from rect's x
                             * ([mouse x coord] - [rect x coord]) / [rect width] = percentage (0-1) of mouse x coord in rect's max x (width)
                             *
                             * doesnt care about current UI rappresentation of gain;
                             * gain is calculated on-the-fly here based on the mouse's position within the volume rect
                             * only then is UI rappresentation of gain calculated based on state->volumeGain
                            */
                            float gain = (x - buttonRect.x) / buttonRect.w;

                            // clamps if for some reason < 0 || > 1
                            if (gain < 0.f) gain = 0.f;
                            if (gain > 1.f) gain = 1.f;

                            // QOL thresholds over which gain is intuitively muted or maximised
                            if (gain <= 0.05f) gain = 0.f;
                            if (gain >= 0.95f) gain = 1.f;

                            // UI will read from here
                            state->volumeGain = gain;

                            setGain(state, gain);

                            SDL_Log("volume gain set to: %f\n", state->volumeGain);
                        }
                        break;
                    }

                    case UI_BTN_NONE:
                        //SDL_Log("left mouse click outside buttons at XY coordinates %2.f %2.f\n", x, y);
                        break;
                }
            }
            //
            break;
    }

    return 0;
}