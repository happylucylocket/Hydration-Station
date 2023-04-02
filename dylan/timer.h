#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>

#define US_TO_MS(US) US / 1000
#define MS_TO_US(MS) MS * 1000
long long getTimeInUs(void);
void sleepForMs(long long delayInMs);

#endif