#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "audioPlayer.h"
#include "distanceSensorLinux.h"
#include "pump.h"
#include "timer.h"

// #define DEFAULT_TIME 1800 // 30 minutes
#define DEFAULT_TIME 15 // for testing - DELETE LATER
#define MIN_TIME 900 // 15 minutes
#define MAX_TIME 10800 // 3 hours

static pthread_t timerThreadId;
void* timerThread(void* arg);
static pthread_mutex_t timerMutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t silencedSemaphore;

static long long totalSeconds;
static long long secondsRemaining;
static bool stopping = false;
static bool silenced = false;
static int waterAmount = CUP_ML;

static void timer(void);

void Timer_init() {
    sem_init(&silencedSemaphore, 0, 0);
    totalSeconds = DEFAULT_TIME;
    secondsRemaining = DEFAULT_TIME;

    pthread_create(&timerThreadId, NULL, timerThread, NULL);
}

void Timer_cleanup() {
    printf("Cleaning up Timer thread\n");
    stopping = true;
    sem_post(&silencedSemaphore);
    sem_destroy(&silencedSemaphore);
	pthread_join(timerThreadId, NULL);
    pthread_mutex_destroy(&timerMutex);
}

void Timer_setTimer(long long newTime) {
    // Ensure timer length is greater than 15 minutes (in seconds)
	if (newTime < MIN_TIME || newTime > MAX_TIME) {
		printf("ERROR: Alarm interval must be between 900s (15 minutes) and 10800s (3 hours).\n");
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
    sem_post(&silencedSemaphore);
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
            double distance = DistanceSensor_getDistance();
            printf("Distance sensor: %02f\n", distance);
            // If there is something to hold the water close enough to the distance sensor, dispense water and then scream
            if (!stopping) {
                if (distance > -1 && distance < 4) {
                    Pump_pumpML(waterAmount);
                    Audio_playScream();
                } else {
                    // If there isn't anything in front of distance sensor, scream first and wait until something is in front of the sensor to dispense water
                    Audio_playScream();
                    printf("Waiting to detect a cup infront\n");
                    do {
                        distance = DistanceSensor_getDistance();
                    } while (!(distance > -1 && distance < 4) && !stopping);
                    if (!stopping) {
                        Pump_pumpML(waterAmount);
                    }
                }
            }
            // Don't stop screaming until the alarm has been shut off
            sem_wait(&silencedSemaphore);
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