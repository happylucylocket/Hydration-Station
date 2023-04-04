#include "distanceSensor.h"
#include <stdio.h>
#include "timer.h"
#include <stdlib.h>

// Trigger = P8_27 (GPIO 86), direction = out
// Echo = P8_29 (GPIO 87), direction = in
#define TRIGGER "/sys/class/gpio/gpio86/value"
#define ECHO "/sys/class/gpio/gpio87/value"

#define ON "1"
#define OFF "0"

static int readFromFile(char* fileLocation);
static void writeToFile(char* fileLocation, char* message);

// notes
// http://wiki.ros.org/Drivers/Tutorials/DistanceMeasurementWithUltrasonicSensorHC-SR04ArduinoI2CPython
// https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/links/files/2022-student-howtos/RCWL-1601UltrasonicDistanceSensor.pdf

// TODO init function, clean up code + comments, // sometimes the code stops working, debug
// init function: make p9_41 and p9_42 gpio, make gpio20 direction out, make gpio7 direction in
// USE PRU INSTEAD OF GPIO FOR MORE ACCURATE RESULTS

double getDistance(void)
{
    writeToFile(TRIGGER, ON);
    sleepForMs(US_TO_MS(10));
    writeToFile(TRIGGER, OFF);

    long long startTime = getTimeInUs();
    while(readFromFile(ECHO) == 0) {
        startTime = getTimeInUs();
    }

    long long stopTime = getTimeInUs();
    while(readFromFile(ECHO) == 1) {
        stopTime = getTimeInUs();
        // printf("Test\n\n");
    }

    // printf("startTime: %lld, stopTime: %lld\n", startTime, stopTime);

    double timeElapsed = (double)(stopTime - startTime);
    // printf("timeElapsed: %f\n", timeElapsed);
    if(timeElapsed >= MS_TO_US(38)) {
        return -1;
    } else if(timeElapsed < 0) {
        return 0;
    } else {
        return (timeElapsed * 0.034) / 2.0;
    }
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