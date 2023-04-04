#include <stdio.h>
#include <stdlib.h>
#include "pump.h"
#include "timer.h"
#include "distanceSensorLinux.h"

// int main(int argc, char* args[]) 
// {
//     Pump_init();
//     for(int i = 0; i < 10; i++) {
//         if(i%2) {
//             Pump_turnOff();
//         } else {
//             Pump_turnOn();
//         }
//         printf("sleeping\n");
//         sleepForMs(1000);
//     }    
//     return 0;
// }

int main(int argc, char* args[]) 
{
    DistanceSensor_init();
    for(int i = 0; i < 10; i++) {
    // while(1) {
        double distance = DistanceSensor_getDistance();
        printf("Current distance: %0.2fcm\n", distance);
        // test();
        sleepForMs(500);
    }    
    DistanceSensor_cleanup();
    return 0;
}