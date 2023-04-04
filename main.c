#include <stdio.h>
#include <stdbool.h>
#include <alsa/asoundlib.h>
#include "audioPlayer.h"
#include "joystick.h"
#include "timer.h"
#include "udp.h"
#include "shutdown.h"
#include "pump.h"


int main() {
    Audio_init();
    printf("1");
    Joystick_init();
    printf("2");
    Timer_init();
    printf("3");
    Pump_init();
    printf("4");
    UDP_init();
    printf("5");

    Shutdown_waitForShutdown();
    
    UDP_cleanup();
    Joystick_cleanup();
    Timer_cleanup();
    Audio_cleanup();

    return 0;
}