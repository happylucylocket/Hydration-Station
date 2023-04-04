#ifndef TIMER_H
#define TIMER_H

#define US_TO_MS(US) US / 1000
#define MS_TO_US(MS) MS * 1000

void Timer_init(void);
void Timer_cleanup(void);
void Timer_silenceAlarm(void);
long long Timer_getTimer(void);
void Timer_setTimer(long long newTime);
long long Timer_getTimeRemaining(void);
int Timer_getWaterAmount(void);
void Timer_setWaterAmount(int amount);

long long getTimeInUs(void);
void sleepForMs(long long delayInMs);

#endif