#include <stdio.h>
#include <stdbool.h>
#include <alsa/asoundlib.h>
#include "audioPlayer.h"
#include "joystick.h"
#include "timer.h"

int main() {
    Audio_init();
    Joystick_init();
    Timer_init();

    sleepForMs(100000);
    // Shutdown_waitForShutdown();
    
    Joystick_cleanup();
    Timer_cleanup();
    Audio_cleanup();

    return 0;
}