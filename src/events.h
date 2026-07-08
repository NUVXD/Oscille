#ifndef EVENTS_H
#define EVENTS_H

#include "SDL3/SDL.h"

struct appState;

extern int appEvents(struct appState *state, SDL_Event *event);

#endif