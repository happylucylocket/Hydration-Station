#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>


static void sleepForMs(long long delayInMs) {
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;

    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;

    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

//30 minute timer
void timer30min() {
    long long totalSeconds = 1800;
    while (totalSeconds >= 0) {
        sleepForMs(1000);
        int mins = totalSeconds/60;
        int seconds = totalSeconds%60;
        printf("Time remaining: %d:%d\n", mins, seconds);
        totalSeconds -= 1;
    }
}
int main() {
    while(true) {
        timer30min();
    }

    return 0;
}