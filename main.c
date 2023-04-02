#include <stdio.h>
#include <stdbool.h>
#include <alsa/asoundlib.h>
#include "audioPlayer.h"
#include "joystick.h"
#include "timer.h"
#include "udp.h"
#include "shutdown.h"

int main() {
    Audio_init();
    Joystick_init();
    Timer_init();
    UDP_init();

    Shutdown_waitForShutdown();
    
    UDP_cleanup();
    Joystick_cleanup();
    Timer_cleanup();
    Audio_cleanup();

    return 0;
}