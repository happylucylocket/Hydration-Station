#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include "shutdown.h"
#include "timer.h"

// using pin P8_46 (GPIO 71) to shutdown via button
#define PIN_VALUE "/sys/class/gpio/gpio71/value"
#define PIN_DIRECTION "/sys/class/gpio/gpio71/direction"
#define GPIO_CONFIG "config-pin P8_46 gpio"

static bool stopping = false;
static pthread_t shutdownThreadId;
void* shutdownThread(void* arg);

static void runCommand(char* command);
static int readFromFile(char* fileLoaction);
static void writeToFile(char* fileLocation, char* message);

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
    runCommand(GPIO_CONFIG);
    writeToFile(PIN_DIRECTION, "in");
    sem_init(&semaphore, 0, 0);
    sleepForMs(350);
    pthread_create(&shutdownThreadId, NULL, shutdownThread, NULL);
}

void Shutdown_cleanup(void)
{
    stopping = true;
    pthread_join(shutdownThreadId, NULL);
    sem_destroy(&semaphore);
}

void* shutdownThread(void* arg) {
    while(!stopping) {
        if(readFromFile(PIN_VALUE) == 1) {
            Shutdown_shutdown();
        }
    }
    return NULL;
}

static int readFromFile(char* fileLocation)
{
    FILE* file = fopen(fileLocation, "r");
    if(!file) {
        printf("ERROR opening file %s", fileLocation);
        fclose(file);
        exit(1);
    }
    const int MAX_LENGTH = 2;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, file);
    fclose(file);
    return atoi(buff);
}

static void writeToFile(char* fileLocation, char* message)
{
    FILE* file = fopen(fileLocation, "w");
    if(!file) {
        printf("ERROR OPENING FILE %s\n", fileLocation);
        fclose(file);
        exit(1);
    }

    if(fprintf(file, "%s", message) <= 0) {
        printf("ERROR PRINTING TO FILE %s\n", fileLocation);
        exit(1);
    }

    fclose(file);
}

static void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
        break;
        // printf("--> %s", buffer); // Uncomment for debugging
    }

    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}