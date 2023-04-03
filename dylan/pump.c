#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "timer.h"
#include "pump.h"


#define P9_19_GPIO_CONFIG "config-pin P9_19 gpio"
#define P9_20_GPIO_CONFIG "config-pin P9_20 gpio"

#define GPIO_FILE(GPIO_NUMBER) "/sys/class/gpio/gpio" GPIO_NUMBER
#define GPIO_12 "12"
#define GPIO_13 "13"
#define GPIO_EXPORT_FILE "/sys/class/gpio"

// ONLY P9_20 (GPIO 13) direction and value matters
// TODO: figure out why GPIO_13 instead of GPIO_12
#define PUMP_DIRECTION_FILE \
            GPIO_FILE(GPIO_13)"/direction"
#define PUMP_VALUE_FILE \
            GPIO_FILE(GPIO_13)"/value"

#define ON "1"
#define OFF "0"

static void writeToFile(char* fileLocation, char* message);
static void runCommand(char* command);
static bool fileExists(char* file);

void Pump_init(void)
{
    // printf("%s", GPIO_FILE(GPIO_12));

    // config the i2c2 pins to gpio
    runCommand(P9_19_GPIO_CONFIG);
    runCommand(P9_20_GPIO_CONFIG);

    // export gpio pins if not already exported
    if(!fileExists(GPIO_FILE(GPIO_12))) {
        writeToFile(GPIO_EXPORT_FILE, GPIO_12);
    }

    if(!fileExists(GPIO_FILE(GPIO_13))) {
        writeToFile(GPIO_EXPORT_FILE, GPIO_13);
    }
    // have to wait for ~350ms after exporting gpio pins before using them
    sleepForMs(350);

    writeToFile(PUMP_DIRECTION_FILE, "out");
    writeToFile(PUMP_VALUE_FILE, OFF);
}

void Pump_turnOff(void)
{
    writeToFile(PUMP_VALUE_FILE, OFF);
}

void Pump_turnOn(void)
{
    writeToFile(PUMP_VALUE_FILE, ON);
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

// copied from https://www.learnc.net/c-tutorial/c-file-exists/
static bool fileExists(char* file)  
{
    return access(file, F_OK) == 0;
}