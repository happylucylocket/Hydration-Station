#ifndef TIMER_H
#define TIMER_H

void Timer_init(void);
void Timer_cleanup(void);
void Timer_silenceAlarm(void);
long long Timer_getTimer(void);
void Timer_setTimer(long long newTime);
long long Timer_getTimeRemaining(void);

void sleepForMs(long long delayInMs);

#endif