#ifndef SHUTDOWN_H_
#define SHUTDOWN_H_

#include <semaphore.h>
#include <stdio.h>

void Shutdown_waitForShutdown(void);
void Shutdown_shutdown(void);
void Shutdown_init(void);
void Shutdown_freeMemory(void);

#endif