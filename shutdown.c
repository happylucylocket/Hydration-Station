#include "shutdown.h"

void Shutdown_waitForShutdown(void);
void Shutdown_shutdown(void);
void Shutdown_init(void);
void Shutdown_freeMemory(void);

static sem_t semaphore;

void Shutdown_waitForShutdown(void)
{
    printf("SHUTDOWN: waiting for shutdown!\n");
    sem_wait(&semaphore);
    printf("SHUTDOWN: finished waiting for semaphore\n");
}

void Shutdown_shutdown(void)
{
    sem_post(&semaphore);
}

void Shutdown_init(void)
{
    sem_init(&semaphore, 0, 0);
}

void Shutdown_freeMemory(void)
{
    sem_destroy(&semaphore);
}