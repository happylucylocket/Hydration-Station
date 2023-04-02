#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include "../sharedDataStruct.h"
#include "resource_table_empty.h"
// P8_12 (GPIO 44) = Trigger, set to pruout
// P8_15 (GPIO 47) = Echo, set to pruin
// Set Trigger on for 10us, set Trigger off, see how long it takes Echo to turn on

#define THIS_PRU_DRAM 0x00000
#define OFFSET 0x200
#define DELAY_1_MS 200000
#define DELAY_1_US 200

#define TRIGGER_BIT_MASK 1 << 14
#define ECHO_BIT_MASK 1 << 14

volatile register unit32_t __R30;
volatile register unit32_t __R31;
volatile sharedMemStruct_t* pSharedMemStruct = (volatile void*) (THIS_PRU_DRAM + OFFSET);

static void init(void);
static void runCommand(char* command);
static void sleepForMs(long long delayInMs);

void main(void)
{
    init();
    while(true) {
        long long timeElapsedInUs = 0;
        __R30 |= TRIGGER_BIT_MASK;
        sleepForUs(10);
        __R30 &= ~TRIGGER_BIT_MASK;

        while((__R31 & ECHO_BIT_MASK) == ECHO_BIT_MASK) {
            timeElapsedInUs++;
            sleepForUs(1);
        }
        if(timeElapsedInUs > 38000) {
            pSharedMemStruct->currentDistance = -1;
        } else {
            pSharedMemStruct->currentDistance = ((double)timeElapsedInUs * 0.034) / 2.0;
        }
        sleepForMs(1);
    }
}

// static void init(void)
// {
//     runCommand("config-pin P8_15 pruout");
//     runCommand("config-pin P8_16 pruin");
// }

// static void runCommand(char* command)
// {
//     // Execute the shell command (output into pipe)
//     FILE *pipe = popen(command, "r");
//     // Ignore output of the command; but consume it
//     // so we don't get an error when closing the pipe.
//     char buffer[1024];
//     while (!feof(pipe) && !ferror(pipe)) {
//         if (fgets(buffer, sizeof(buffer), pipe) == NULL)
//         break;
//         // printf("--> %s", buffer); // Uncomment for debugging
//     }

//     // Get the exit code from the pipe; non-zero is an error:
//     int exitCode = WEXITSTATUS(pclose(pipe));
//     if (exitCode != 0) {
//         perror("Unable to execute command:");
//         printf(" command: %s\n", command);
//         printf(" exit code: %d\n", exitCode);
//     }
// }

static void sleepForMs(int delayInMs)
{
    for(int index = 0; index < delayInMs; index++) {
        __delay_cycles(DELAY_1_MS);
    }
}

static void sleepForUs(int delayInUs) {
    for(int index = 0; index < delayInMs; index++) {
        __delay_cycles(DELAY_1_US);
    }
}