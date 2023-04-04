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
    Joystick_init();
    DistanceSensor_init();
    Timer_init();
    Pump_init();
    UDP_init();

    Shutdown_waitForShutdown();
    
    
    UDP_cleanup();
    Timer_cleanup();
    DistanceSensor_cleanup();
    Joystick_cleanup();
    Audio_cleanup();

    return 0;
}