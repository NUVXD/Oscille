#include <string.h>
#include "SDL3/SDL.h"
#include "WAV.h"
#include "events.h"
#include "appstate.h"
#include "audio.h"
#include "UI.h"

static void printDebugHeaderInfo(appState *state) {
    SDL_Log("\n");
    SDL_Log("FILE SIZE ACCORDING TO FILE: %u BYTES\n", state->WAV.header.Riff.fileSize);
    SDL_Log("MAIN CHUNK: \n");
    SDL_Log("| RiffID: %.4s\n", state->WAV.header.Riff.ID);
    SDL_Log("| fileFormatID: %.4s\n", state->WAV.header.Riff.fileFormatID);
    SDL_Log("SUBCHUNK 1: \n");
    SDL_Log("| FormatID: %.4s\n", state->WAV.header.Format.ID);
    SDL_Log("| BitsPerSample: %u\n", state->WAV.header.Format.bitsPerSample);
    SDL_Log("| Frequency: %u\n", state->WAV.header.Format.frequency);
    SDL_Log("| BytesPerSec: %u\n", state->WAV.header.Format.bytesPerSec);
    SDL_Log("\n");
};

static _Bool isFieldPathActive = 0;

static void appendPathText(appState *state, const char *text) {
    if (!state || !text || text[0] == '\0')
        return;
    size_t currentLen = strlen(state->WAV.filePath);
    if (currentLen >= (APP_WAV_PATH_MAX - 1))
        return;
    size_t appendLen = strlen(text);
    size_t freeSpace = (APP_WAV_PATH_MAX - 1) - currentLen;
    if (appendLen > freeSpace)
        appendLen = freeSpace;
    memcpy(state->WAV.filePath + currentLen, text, appendLen);
    state->WAV.filePath[currentLen + appendLen] = '\0';
}

static void pastePathText(appState *state, const char *text) {
    if (!state || !text || text[0] == '\0')
        return;
    size_t pasteLen = strlen(text);
    if (pasteLen >= (APP_WAV_PATH_MAX - 1)) {
        pasteLen = APP_WAV_PATH_MAX;
        SDL_Log("clipboard text length exceeds the maximum file path length of %u, it has been cropped\n", APP_WAV_PATH_MAX);
    }
    memcpy(state->WAV.filePath, text, pasteLen);
    state->WAV.filePath[pasteLen] = '\0';
}

static void removePathChar(char *text) {
    if (!text)
        return;
    size_t len;
    len = strlen(text);
    if (len == 0)
        return;
    len--;
    while (len > 0 && (((unsigned char)text[len] & 0xC0) == 0x80))
        len--;
    text[len] = '\0';
}

int appEvents(appState *state, SDL_Event *event) {

    switch (event->type) {
        case SDL_EVENT_QUIT:
            return 1;
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_ESCAPE)
                return 1;
            if (isFieldPathActive) {
                if (event->key.key == SDLK_BACKSPACE) {
                    removePathChar(state->WAV.filePath);
                }
                else if (event->key.key == SDLK_RETURN || event->key.key == SDLK_KP_ENTER) {
                    SDL_Log("WAV path confirmed: %s\n", state->WAV.filePath);
                    isFieldPathActive = 0;
                    SDL_StopTextInput(state->window);
                }
            }
            break;
        case SDL_EVENT_TEXT_INPUT:
            if (isFieldPathActive && event->text.text) {
                appendPathText(state, event->text.text);
            }
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
                    case UI_BTN_PLAY: {
                        SDL_Log("play button clicked\n");
                        destroyAudio(state); // should make it return something in the future - as a check
                        freeWAV(&state->WAV.wavBuffer);
                        _Bool isError = parseWAV(state->WAV.filePath, &state->WAV.header, &state->WAV.wavBuffer);
                        if (isError) {
                            SDL_Log("error with loading or parsing WAV file\n");
                            if (state->WAV.wavBuffer) {
                                free(state->WAV.wavBuffer);
                                state->WAV.wavBuffer = (void *)0;
                            }
                            break;
                        }
                        initAudio(state);
                        printDebugHeaderInfo(state);
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
                            // QOL thresholds over which gain is intuitively set
                            if (gain <= 0.05f) gain = 0.f;
                            if (gain <= 0.525 && gain >= 0.475) gain = 0.5f;
                            if (gain >= 0.95f) gain = 1.f;
                            // UI will read from here
                            state->AUDIO.volumeGain = gain;
                            setGain(state, gain);
                            SDL_Log("volume gain set to: %f\n", state->AUDIO.volumeGain);
                        }
                        break;
                    }
                    case UI_FIELD_PATH:
                        isFieldPathActive = 1;
                        break;
                    case UI_BTN_SCOPE_SCALE_NEG:
                        SDL_Log("scope size neg button clicked\n");
                        if (state->scopeScale > 0)
                        state->scopeScale -= 1;
                        break;
                    case UI_BTN_SCOPE_SCALE_POS:
                        SDL_Log("scope size pos button clicked\n");
                        if (state->scopeScale < 100)
                        state->scopeScale += 1;
                        break;
                    case UI_BTN_NONE:
                    default:
                        isFieldPathActive = 0;
                        SDL_StopTextInput(state->window);
                        break;
                }
            }
            else if (event->button.button == SDL_BUTTON_RIGHT) {
                // paste clipboard text into fieldPath
                if (isFieldPathActive) {
                    const char *clipboardText = SDL_GetClipboardText();
                    if (!clipboardText) {
                        SDL_Log("unable to get text from clipboard: %s\n", SDL_GetError());
                    }
                    pastePathText(state, clipboardText);
                }
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event->button.button == SDL_BUTTON_LEFT) {
                float x, y;
                SDL_GetMouseState(&x, &y);
                UI_BUTTONS button = getUIButtonEnum(x, y);
                switch (button) {
                    case UI_FIELD_PATH: {
                        SDL_Log("WAV filePath field clicked up\n");
                        isFieldPathActive = 1;
                        SDL_StartTextInput(state->window);
                        break;
                    }
                    case UI_BTN_PLAY:
                    case UI_BTN_PAUSE:
                    case UI_BTN_RESUME:
                    case UI_BTN_VOLUME:
                    case UI_BTN_NONE:
                    case UI_BTN_SCOPE_SCALE_NEG:
                    case UI_BTN_SCOPE_SCALE_POS:
                    default:
                        isFieldPathActive = 0;
                        SDL_StopTextInput(state->window);
                        break;
                }
            }
            break;
    }
    return 0;
}