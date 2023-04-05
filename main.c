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
    Joystick_init();
    DistanceSensor_init();
    Audio_init();
    Pump_init();
    Timer_init();
    UDP_init();
    Shutdown_init();

    Shutdown_waitForShutdown();

    Shutdown_freeMemory();
    UDP_cleanup();
    Timer_cleanup();
    Audio_cleanup();
    DistanceSensor_cleanup();
    Joystick_cleanup();

    return 0;
}