// Playback sounds in real time, allowing multiple simultaneous wave files
// to be mixed together and played without jitter.
#include <alsa/asoundlib.h>
#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

// Store data of a single wave file read into memory.
// Space is dynamically allocated; must be freed correctly!
typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;

#define AUDIOMIXER_MAX_VOLUME 100

// init() must be called before any other functions,
// cleanup() must be called last to stop playback threads and free memory.
void Audio_init(void);
void Audio_cleanup(void);
void Audio_playScream(void);
void Audio_stopScream(void);

#endif