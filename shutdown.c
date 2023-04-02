#include <stdio.h>
#include <stdbool.h>

static bool shutdown = false;

void Shutdown_shutdown(void)
{
    shutdown = true;
}

void Shutdown_waitForShutdown(void) 
{
    while (!shutdown) {}

    return;
}