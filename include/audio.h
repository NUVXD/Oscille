#ifndef AUDIO_H
#define AUDIO_H

struct appState;

extern void setGain(struct appState *state, float gain);
extern void initAudio(struct appState *state);
extern void destroyAudio(struct appState *state);
extern void pauseAudio(struct appState *state);
extern void resumeAudio(struct appState *state);

#endif