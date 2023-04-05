#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include "timer.h"
#include "pump.h"
#include "audioPlayer.h"

#define JOYSTICK_UP_VALUE "/sys/class/gpio/gpio26/value"
#define JOYSTICK_DOWN_VALUE "/sys/class/gpio/gpio46/value"
#define JOYSTICK_LEFT_VALUE "/sys/class/gpio/gpio65/value"
#define JOYSTICK_RIGHT_VALUE "/sys/class/gpio/gpio47/value"
#define JOYSTICK_CENTER_VALUE "/sys/class/gpio/gpio27/value"
#define JOYSTICK_UP_DIRECTION "/sys/class/gpio/gpio26/direction"
#define JOYSTICK_DOWN_DIRECTION "/sys/class/gpio/gpio46/direction"
#define JOYSTICK_LEFT_DIRECTION "/sys/class/gpio/gpio65/direction"
#define JOYSTICK_RIGHT_DIRECTION "/sys/class/gpio/gpio47/direction"
#define JOYSTICK_CENTER_DIRECTION "/sys/class/gpio/gpio27/direction"

enum direction {  
    UP,
    DOWN,
    LEFT,
    RIGHT,
    CENTER,
    NONE
};

static int currentJoystick = NONE;
static pthread_t joystickThreadId;
void* joystickThread(void* arg);
static int currentDirection(void);
static void joystickPinToInput(void);
static int readJoystickValue(char *fileName);
static void runCommand(char* command);

static bool stopping = false;

static int readJoystickValue(char *fileName) {
    FILE *pFile = fopen(fileName, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file %s for reading\n", fileName);
        exit(-1);
    }

    const int MAX_LENGTH = 1024;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, pFile);

    fclose(pFile);
    // Convert to int since it was string
    int value = (int)atol(buff);
    return value;
}

// Change all joystick pins to be input
static void joystickPinToInput(void) {
    int charWritten;

    FILE *pJoystickUpDirection = fopen(JOYSTICK_UP_DIRECTION, "w");
    if (pJoystickUpDirection == NULL) {
        printf("ERROR OPENING %s.", JOYSTICK_UP_DIRECTION);
        exit(1);
    }
    charWritten = fprintf(pJoystickUpDirection, "in");
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
    }
    fclose(pJoystickUpDirection);

    FILE *pJoystickDownDirection = fopen(JOYSTICK_DOWN_DIRECTION, "w");
    if (pJoystickUpDirection == NULL) {
        printf("ERROR OPENING %s.", JOYSTICK_DOWN_DIRECTION);
        exit(1);
    }
    charWritten = fprintf(pJoystickDownDirection, "in");
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
    }
    fclose(pJoystickDownDirection);

    FILE *pJoystickLeftDirection = fopen(JOYSTICK_LEFT_DIRECTION, "w");
    if (pJoystickLeftDirection == NULL) {
        printf("ERROR OPENING %s.", JOYSTICK_LEFT_DIRECTION);
        exit(1);
    }
    charWritten = fprintf(pJoystickLeftDirection, "in");
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
    }
    fclose(pJoystickLeftDirection);

    FILE *pJoystickRightDirection = fopen(JOYSTICK_RIGHT_DIRECTION, "w");
    if (pJoystickRightDirection == NULL) {
        printf("ERROR OPENING %s.", JOYSTICK_RIGHT_DIRECTION);
        exit(1);
    }
    charWritten = fprintf(pJoystickRightDirection, "in");
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
    }
    fclose(pJoystickRightDirection);

    FILE *pJoystickPushedDirection = fopen(JOYSTICK_CENTER_DIRECTION, "w");
    if (pJoystickPushedDirection == NULL) {
        printf("ERROR OPENING %s.", JOYSTICK_CENTER_DIRECTION);
        exit(1);
    }
    charWritten = fprintf(pJoystickPushedDirection, "in");
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
    }
    fclose(pJoystickPushedDirection);
}

static void runCommand(char* command) {
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");

    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL) {
            break;
            // printf("--> %s", buffer); // debugging
        }
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf("    command:    %s\n", command);
        printf("    exit code: %d\n", exitCode);
    }
}

// Checks the joystick for presses and returns it in the given time
static int currentDirection() {
    int direction = NONE;
    // Check all the values of the gpios for the one that was pressed (0)
 
    if (readJoystickValue(JOYSTICK_UP_VALUE) == 0) {
        direction = UP;
    }
    else if (readJoystickValue(JOYSTICK_DOWN_VALUE) == 0) {
        direction = DOWN;
    }
    else if (readJoystickValue(JOYSTICK_LEFT_VALUE) == 0) {
        direction = LEFT;
    }
    else if (readJoystickValue(JOYSTICK_RIGHT_VALUE) == 0) {
        direction = RIGHT;
    }
    else if (readJoystickValue(JOYSTICK_CENTER_VALUE) == 0) {
        direction = CENTER;
    }
    return direction;
}

void Joystick_init(void) {
    // Joystick setup
    runCommand("config-pin p8.14 gpio"); // Joystick up
    runCommand("config-pin p8.15 gpio"); // Joystick right
    runCommand("config-pin p8.16 gpio"); // Joystick down
    runCommand("config-pin p8.18 gpio"); // Joystick left
    runCommand("config-pin p8.17 gpio"); // Joystick pushed

    sleepForMs(350);
    joystickPinToInput();

    // Launch joystick thread:
	pthread_create(&joystickThreadId, NULL, joystickThread, NULL);
}

void Joystick_cleanup(void) {
    printf("Cleaning up Joystick thread\n");
    stopping = true;
	pthread_join(joystickThreadId, NULL);
}

void* joystickThread(void* arg) {
    while(!stopping) {
        currentJoystick = currentDirection();
        if (currentJoystick == CENTER) {
            if (Timer_getTimeRemaining() > 0) {
                printf("Alarm hasn't gone off!\n");
            }
            else {
                printf("ALARM SILENCED\n");
                Audio_stopScream();
                Timer_silenceAlarm();
            }
            sleepForMs(300);
        }
        else if (currentJoystick == UP) {
            // Increase water amount by 1 preset amount
            printf("WATER INCREASED\n");
            int current_amount = Timer_getWaterAmount();
            if (current_amount == CUP_ML) {
                Timer_setWaterAmount(MUG_ML);
            }
            else if (current_amount == MUG_ML) {
                Timer_setWaterAmount(BOTTLE_ML);
            }
            else if (current_amount == BOTTLE_ML) {
                Timer_setWaterAmount(BIG_BOTTLE_ML);
            }
            else if (current_amount == BIG_BOTTLE_ML) {
                Timer_setWaterAmount(MUG_ML);
            }
            sleepForMs(300);
        }
        else if (currentJoystick == DOWN) {
            // Decrease water amount by 1 preset amount
            printf("WATER DECREASED\n");
            int current_amount = Timer_getWaterAmount();
            if (current_amount == BIG_BOTTLE_ML) {
                Timer_setWaterAmount(BOTTLE_ML);
            }
            else if (current_amount == BOTTLE_ML) {
                Timer_setWaterAmount(MUG_ML);
            }
            else if (current_amount == MUG_ML) {
                Timer_setWaterAmount(CUP_ML);
            }
            else if (current_amount == CUP_ML) {
                Timer_setWaterAmount(BIG_BOTTLE_ML);
            }
            sleepForMs(300);
        }
        else if (currentJoystick == LEFT) {
            // Minus 15 minutes
            printf("TIME INCREASED\n");
            long long currentTimer = Timer_getTimer();
            int newTime = currentTimer - 900;
            Timer_setTimer(newTime);
            sleepForMs(300);
        }
        else if (currentJoystick == RIGHT) {
            // Add 15 minutes
            printf("TIME DECREASED");
            long long currentTimer = Timer_getTimer();
            int newTime = currentTimer + 900;
            Timer_setTimer(newTime);
            sleepForMs(300);
        }
        sleepForMs(10);
    }
    return NULL;
}