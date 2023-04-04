/*
 *  Small program to read a 16-bit, signed, 44.1kHz wave file and play it.
 *  Written by Brian Fraser, heavily based on code found at:
 *  http://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm_min_8c-example.html
 */

#include <alsa/asoundlib.h>
#include <alloca.h> // needed for mixer
#include "audioPlayer.h"
#include <pthread.h>
#include <stdbool.h>
#include "timer.h"

// File used for play-back:
// If cross-compiling, must have this file available, via this relative path,
// on the target when the application is run. This example's Makefile copies the wave-files/
// folder along with the executable to ensure both are present.
#define SCREAM_FILE "wave-files/screams.wav"
#define QUENCH_FILE "wave-files/quench.wav"

#define DEFAULT_VOLUME 100
#define SAMPLE_RATE   88200 // Originally 44100
#define NUM_CHANNELS  1
#define SAMPLE_SIZE   (sizeof(short)) 	// bytes per sample


snd_pcm_t *handle;
wavedata_t screamFile;
wavedata_t quenchFile;

static pthread_t audioThreadId;
static void* audioThread(void* arg);
static bool stopping = false;

// Prototypes:
static snd_pcm_t *Audio_openDevice();
static void Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct);
static void Audio_playFile(snd_pcm_t *handle, wavedata_t *pWaveData);
static int volume = 0;
static void Audio_setVolume(int newVolume);


void Audio_init(void)
{
    Audio_setVolume(DEFAULT_VOLUME);

	// Configure Output Device
	handle = Audio_openDevice();

	// Load wave file we want to play:
	
	Audio_readWaveFileIntoMemory(SCREAM_FILE, &screamFile);
	Audio_readWaveFileIntoMemory(QUENCH_FILE, &quenchFile);
}

void Audio_cleanup(void)
{
	printf("Cleaning up audioPlayer.c\n");
	Audio_stopScream();
	sleepForMs(2000);
	// Cleanup, letting the music in buffer play out (drain), then close and free.
	snd_pcm_drain(handle);
	snd_pcm_hw_free(handle);
	snd_pcm_close(handle);
	free(screamFile.pData);
	free(quenchFile.pData);
}

// Open the PCM audio output device and configure it.
// Returns a handle to the PCM device; needed for other actions.
static snd_pcm_t *Audio_openDevice()
{
	snd_pcm_t *handle;

	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Play-back open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Configure parameters of PCM output
	err = snd_pcm_set_params(handle,
			SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			NUM_CHANNELS,
			SAMPLE_RATE,
			1,			// Allow software resampling
			50000);		// 0.05 seconds per buffer
	if (err < 0) {
		printf("Play-back configuration error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	return handle;
}

// Function copied from:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
// Written by user "trenki".
static void Audio_setVolume(int newVolume)
{
	// Ensure volume is reasonable; If so, cache it for later getVolume() calls.
	if (newVolume < 0 || newVolume > AUDIOMIXER_MAX_VOLUME) {
		printf("ERROR: Volume must be between 0 and 100.\n");
		return;
	}
	volume = newVolume;

    long min, max;
    snd_mixer_t *volHandle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "PCM";

    snd_mixer_open(&volHandle, 0);
    snd_mixer_attach(volHandle, card);
    snd_mixer_selem_register(volHandle, NULL, NULL);
    snd_mixer_load(volHandle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(volHandle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(volHandle);
}

// Read in the file to dynamically allocated memory.
// !! Client code must free memory in wavedata_t !!
static void Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct)
{
	assert(pWaveStruct);

	// Wave file has 44 bytes of header data. This code assumes file
	// is correct format.
	const int DATA_OFFSET_INTO_WAVE = 44;

	// Open file
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName);
		exit(EXIT_FAILURE);
	}

	// Get file size
	fseek(file, 0, SEEK_END);
	int sizeInBytes = ftell(file) - DATA_OFFSET_INTO_WAVE;
	fseek(file, DATA_OFFSET_INTO_WAVE, SEEK_SET);
	pWaveStruct->numSamples = sizeInBytes / SAMPLE_SIZE;

	// Allocate Space
	pWaveStruct->pData = malloc(sizeInBytes);
	if (pWaveStruct->pData == NULL) {
		fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n",
				sizeInBytes, fileName);
		exit(EXIT_FAILURE);
	}

	// Read data:
	int samplesRead = fread(pWaveStruct->pData, SAMPLE_SIZE, pWaveStruct->numSamples, file);
	if (samplesRead != pWaveStruct->numSamples) {
		fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n",
				pWaveStruct->numSamples, fileName, samplesRead);
		exit(EXIT_FAILURE);
	}

	fclose(file);
}

// Play the audio file (blocking)
static void Audio_playFile(snd_pcm_t *handle, wavedata_t *pWaveData)
{
	// If anything is waiting to be written to screen, can be delayed unless flushed.
	fflush(stdout);

	// If we dropped the sound, prepare again
	snd_pcm_prepare(handle);
	
	// Write data and play sound (blocking)
	snd_pcm_sframes_t frames = snd_pcm_writei(handle, pWaveData->pData, pWaveData->numSamples);

	// Check for errors
	if (frames < 0)
		frames = snd_pcm_recover(handle, frames, 0);
	if (frames < 0) {
		fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n", frames);
		exit(EXIT_FAILURE);
	}
}

void Audio_playScream(void) {
	// Play Audio thread
	stopping = false;
	pthread_create(&audioThreadId, NULL, audioThread, NULL);
}

void Audio_stopScream(void) {
	snd_pcm_drop(handle);
	stopping = true;
	pthread_join(audioThreadId, NULL);
}


static void* audioThread(void* arg) {
    while(!stopping) {
		printf("Playing audio\n");
		Audio_playFile(handle, &screamFile);
    }
	Audio_playFile(handle, &quenchFile);
    return NULL;
}