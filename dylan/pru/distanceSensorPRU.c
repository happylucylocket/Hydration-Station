#include <stdint.h>
#include <stdbool.h>
#include <pru_cfg.h>
#include "sharedDataStruct.h"
#include "resource_table_empty.h"
// P8_12 = Trigger, set to pruout
// P8_15 = Echo, set to pruin
// Set Trigger on for 10us, set Trigger off, see how long it takes Echo to turn on

#define THIS_PRU_DRAM 0x00000
#define OFFSET 0x200

#define DELAY_1_MS 200000
#define DELAY_1_US 200

#define TRIGGER_BIT_MASK (1 << 14)
#define ECHO_BIT_MASK (1 << 15)

volatile register uint32_t __R30;   // output GPIO register
volatile register uint32_t __R31;   // input GPIO register
volatile sharedMemStruct_t* pSharedMemStruct = (volatile void*) (THIS_PRU_DRAM + OFFSET);

// static void init(void);
// static void runCommand(char* command);
static void sleepForMs(int delayInMs);
static void sleepForUs(int delayInUs);

// I FEEL LIKE THE PIN IS NOT TURNING OFF WHEN THE SIGNAL IS RECEIVED --> EITHER THAT OR THE SIGNAL IS NOT EVEN BEING SENT
void main(void)
{
    // init();
    pSharedMemStruct->currentDistance = 0;
    pSharedMemStruct->smileCount = 0x5566;
    pSharedMemStruct->numMsSinceBigBang = 0x0000111122223333;

    __R30 &= ~TRIGGER_BIT_MASK;
    while(true) {
        bool test = false;
        double timeElapsedInUs = 2;
        __R30 |= TRIGGER_BIT_MASK;
        sleepForUs(10);
        __R30 &= ~TRIGGER_BIT_MASK;

        while((__R31 & ECHO_BIT_MASK) != 0 && timeElapsedInUs < 38000) {
            test = true;
            timeElapsedInUs += 1;
            sleepForUs(1);
        }

        pSharedMemStruct->currentDistance = timeElapsedInUs;

        // if(test) {
        //     pSharedMemStruct->currentDistance = 69;
        // } else {
        //     pSharedMemStruct->currentDistance = 90;
        // }

        // if(timeElapsedInUs > 38000) {
        //     pSharedMemStruct->currentDistance = -1;
        // } else {
        //     pSharedMemStruct->currentDistance = ((double)timeElapsedInUs * 0.034) / 2.0;   
        // }
        // sleepForMs(1);
    }
}

static void sleepForMs(int delayInMs)
{
    for(int index = 0; index < delayInMs; index++) {
        __delay_cycles(DELAY_1_MS);
    }
}

static void sleepForUs(int delayInUs) {
    for(int index = 0; index < delayInUs; index++) {
        __delay_cycles(DELAY_1_US);
    }
}
