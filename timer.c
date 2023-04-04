#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "audioPlayer.h"
#include "distanceSensorLinux.h"
#include "pump.h"

// #define DEFAULT_TIME 1800 // 30 minutes
#define DEFAULT_TIME 5 // for testing - DELETE LATER
#define MIN_TIME 900 // 15 minutes
#define MAX_TIME 10800 // 3 hours

static void timer(void);

static long long totalSeconds;
static long long secondsRemaining;
static pthread_t timerThreadId;
void* timerThread(void* arg);
static pthread_mutex_t timerMutex = PTHREAD_MUTEX_INITIALIZER;
static bool stopping = false;
static bool silenced = false;
static int waterAmount = CUP_ML;


void Timer_init() {
    totalSeconds = DEFAULT_TIME;
    secondsRemaining = DEFAULT_TIME;

    pthread_create(&timerThreadId, NULL, timerThread, NULL);
}

void Timer_cleanup() {
    printf("Cleaning up Timer thread\n");
    stopping = true;
	pthread_join(timerThreadId, NULL);
    pthread_mutex_destroy(&timerMutex);
}

void Timer_setTimer(long long newTime) {
    // Ensure timer length is greater than 15 minutes (in seconds)
	if (newTime < MIN_TIME || newTime > MAX_TIME) {
		printf("ERROR: Alarm interval must be between 900s (15 minutes) and 7200s (2 hours).\n");
		return;
	}
    else if (secondsRemaining == 0) {
        printf("ERROR: Cannot set new time while alarm is on\n");
        return;
    }
    pthread_mutex_lock(&timerMutex);
    {
        totalSeconds = newTime;
        secondsRemaining = newTime;
    }
    pthread_mutex_unlock(&timerMutex);
}

long long Timer_getTimer() 
{
    return totalSeconds;
}

long long Timer_getTimeRemaining()
{
    return secondsRemaining;
}

int Timer_getWaterAmount() 
{
    return waterAmount;
}

void Timer_setWaterAmount(int newAmount) 
{
    pthread_mutex_lock(&timerMutex);
    {
        waterAmount = newAmount;
    }
    pthread_mutex_unlock(&timerMutex);
}

void Timer_silenceAlarm()
{
    silenced = true;
}

//30 minute timer
static void timer(void) {
    silenced = false;
    secondsRemaining = totalSeconds; // Set the timer to the wanted time interval

    while (secondsRemaining >= 0 && !stopping) {
        sleepForMs(1000);
        int mins = secondsRemaining/60;
        int seconds = secondsRemaining%60;
        printf("Time remaining: %d:%d\n", mins, seconds);
        secondsRemaining -= 1;

        if (secondsRemaining <= 0) {
            printf("SECONDS = 0\n");
            double distance = DistanceSensor_getDistance();
            printf("distance sensor: %02f\n", distance);
            if(distance > -1 && distance < 3) {
                Pump_pumpML(waterAmount);
                // Audio_playScream();
            } else {
                // Audio_playScream();
                do {
                    distance = DistanceSensor_getDistance();
                    printf("distance sensor inside: %02f\n", distance);
                } while(!(distance > -1 && distance < 4) && !stopping);
                Pump_pumpML(waterAmount);
            }
            while(!silenced && !stopping) {}; 
            return;
        }
    }
}

void* timerThread(void* arg) {
    while(!stopping) {
        timer();
    }
    return NULL;
}

long long getTimeInUs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long microSeconds = seconds * 1000000
    + nanoSeconds / 1000;
    return microSeconds;
}

void sleepForMs(long long delayInMs) {
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;

    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;

    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}
