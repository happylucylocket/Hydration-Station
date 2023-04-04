#include <stdio.h>
#include <stdbool.h>
#include <alsa/asoundlib.h>
#include "audioPlayer.h"
#include "joystick.h"
#include "timer.h"
#include "udp.h"
#include "shutdown.h"
#include "pump.h"
#include "distanceSensorLinux.h"

int main() {
    Audio_init();
    printf("1");
    Joystick_init();
    printf("2");
    DistanceSensor_init();
    printf("3");
    Timer_init();
    printf("4");
    Pump_init();
    printf("5");
    UDP_init();
    printf("6\n");

    Shutdown_waitForShutdown();
    
    UDP_cleanup();
    Timer_cleanup();
    DistanceSensor_cleanup();
    Joystick_cleanup();
    Audio_cleanup();

    return 0;
}