// #include "display_test.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define HIGH 0
#define LOW 1
#define CMD_AUTO_ADDR 0x40
#define START_ADDR 0xc0
#define NUM_DIGITS 4
#define COLON_FLAG 0x80
#define ASCII_0 48
#define ASCII_9 57
#define DISPLAY_ON 0x88
#define CLK "/sys/class/gpio/gpio2"
#define DIO "/sys/class/gpio/gpio3"
#define OUT "out"
#define IN "in"


void wait1() 
{
    struct timespec reqDelay = {0, 400};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

void setClk(int value) 
{
    FILE *file = fopen("/sys/class/gpio/gpio2/value", "w");
    if (file == NULL) {
        printf("ERROR OPENING %s.", "/sys/class/gpio/gpio2/value");
    }

    int charWritten = fprintf(file, (char*)value);
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
    }

    fclose(file);
}

void setDio(int value) 
{
    FILE *file = fopen("/sys/class/gpio/gpio3/value", "w");
    if (file == NULL) {
        printf("ERROR OPENING %s.", "/sys/class/gpio/gpio3/value");
    }

    int charWritten = fprintf(file, (char*)value);
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
    }

    fclose(file);
}

void setDirection(char* path, char* direction)
{
    FILE *file = fopen(strcat(path, "/direction") , "w");
    if (file == NULL) {
        printf("ERROR OPENING %s.", strcat(path, "/direction"));
    }

    int charWritten = fprintf(file, direction);
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
    }

    fclose(file);
}

int getValue(char* path) {
    FILE *pFile = fopen(strcat(path, "/value"), "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file %s for reading\n", strcat(path, "/value"));
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
static void tm_start(void)
{
    /*
    * When CLK is high, and DIO goes from high to low, input begins
    */
    setClk(HIGH);
    setDio(HIGH);
    wait1();
    setDio(LOW);
    wait1();
    setClk(LOW);
    wait1();
}

static void tm_stop(void)
{
    /*
    * When CLK is high, and DIO goes from low to high, input ends
    */
    setClk(LOW);
    setDio(LOW);
    wait1();
    setClk(HIGH);
    wait1();
    setDio(HIGH);
    wait1();
}

static void tm_write(char data) {
    /*
    *Send each bit of data
    */
    for(int i = 0; i < 8; i++) {
    //transfer data when clock is low, from low bit to high bit
    setClk(LOW);
    setDio(data & 0x01);
    data >>= 1;
    wait1();
    setClk(HIGH);
    wait1();
 }
    /*
    * End of 8th clock cycle is the start of ACK from TM1637
    */
    setClk(LOW);
    setDirection(DIO, IN);
    wait1();
    //Check that we are getting the ACK from the device
    assert(getValue(DIO) == 0);
    setClk(HIGH);
    wait1();
    setClk(LOW);
    setDirection(DIO, OUT);
}

const static char displayDigits[10] = {
    0x3f,
    0x06,
    0x5b,
    0x4f,
    0x66,
    0x6d,
    0x7d,
    0x07,
    0x7f,
    0x67,
};
static char convertChar(char ch, _Bool colon) {
    char val = 0;
    if ((ASCII_0 <= ch) && (ch <= ASCII_9)) {
    val = displayDigits[ch - ASCII_0];
    }
    if (colon) {
    return val | COLON_FLAG;
    }
    return val;
}

void fourDigit_display(char* digits, _Bool colonOn) {
    assert(strlen(digits) == NUM_DIGITS);
    tm_start();
    tm_write(CMD_AUTO_ADDR);
    tm_stop();
    tm_start();
    tm_write(START_ADDR);
    for (int i = 0; i < NUM_DIGITS; i++) {
    tm_write(convertChar(digits[i], colonOn));
    }
    tm_stop();
    tm_start();
    //This sets it to the brightest
    tm_write(DISPLAY_ON | 0x07);
    tm_stop();
}

int main() {
    // fourDigit_display("0x3f", false);
    printf("DLFJSDL\n");
    wait1();
    printf("LDSFJLK]\n");
}
