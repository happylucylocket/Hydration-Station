/*
 * UDP Listening program (modified by Sarah Li)
 * By Brian Fraser, Modified from Linux Programming Unleashed (book)
*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include <stdbool.h>
#include "shutdown.h"
#include "timer.h"
#include "audioPlayer.h"
#include "pump.h"
#include "distanceSensorLinux.h"

#define MSG_MAX_LEN 1024
#define PORT        8088

void* udpThread(void* arg);
static pthread_t udpThreadId;
static bool stopping = false;
static int socketDescriptor;

struct sockaddr_in sinRemote;
unsigned int sin_len;

static void silence_response();
static void timer_up_response();
static void timer_down_response();
static void quit_response();
static void water_volume_response();
static void timer_response();
static void time_remaining_response();
static void cup_response();
static void mug_response();
static void bottle_response();
static void big_bottle_response();
static void sensor_response();

void UDP_init(void) 
{
    // Address
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network short
	
	// Create the socket for UDP
	socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port (PORT) that we specify
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
    // Check for errors (-1)

    pthread_create(&udpThreadId, NULL, udpThread, NULL);
}

void UDP_cleanup(void)
{
    stopping = true;
    printf("Cleaning up UDP thread\n");
    close(socketDescriptor);
    pthread_join(udpThreadId, NULL);
}

void* udpThread(void* arg)
{
    while (!stopping) {
		// Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		
		sin_len = sizeof(sinRemote);
		char messageRx[MSG_MAX_LEN];

		// Pass buffer size - 1 for max # bytes so room for the null (string data)
		int bytesRx = recvfrom(socketDescriptor,
			messageRx, MSG_MAX_LEN - 1, 0,
			(struct sockaddr *) &sinRemote, &sin_len);

		// Check for errors (-1)
        if (bytesRx == -1) {
            perror("Socket: Unable to receive info from socket.");
            exit(1);
        }

		// Make it null terminated (so string functions work)
		// - recvfrom given max size - 1, so there is always room for the null
		messageRx[bytesRx] = 0;
		// printf("Message received (%d bytes): \n\n'%s'\n", bytesRx, messageRx);

        if (strncmp(messageRx, "silence", strlen("silence")) == 0) {
			// printf("User silenced timer!\n");
			silence_response();
		}

        else if (strncmp(messageRx, "cup", strlen("cup")) == 0) {
			// printf("User selected to change volume to 235mL!\n");
			cup_response();
		}

        else if (strncmp(messageRx, "mug", strlen("mug")) == 0) {
			// printf("User selected to change volume to 350mL!\n");
			mug_response();
		}

        else if (strncmp(messageRx, "bottle", strlen("bottle")) == 0) {
			// printf("User selected to change volume to 500mL!\n");
			bottle_response();
		}

        else if (strncmp(messageRx, "big bottle", strlen("big bottle")) == 0) {
			// printf("User selected to change volume to 930mL!\n");
			big_bottle_response();
		}

        else if (strncmp(messageRx, "timer up", strlen("timer up")) == 0) {
			// printf("User selected to increase timer!\n");
			timer_up_response();
		}
		
        else if (strncmp(messageRx, "timer down", strlen("timer down")) == 0) {
			// printf("User selected to decrease timer!\n");
			timer_down_response();
		}

        else if (strncmp(messageRx, "water volume", strlen("water volume")) == 0) {
            // printf("User selected to check water volume!\n");
			water_volume_response();
		}

        else if (strncmp(messageRx, "time remaining", strlen("time remaining")) == 0) {
            // printf("User selected to check time remaining!\n");
			time_remaining_response();
		}

        else if (strncmp(messageRx, "timer", strlen("timer")) == 0) {
            // printf("User selected to check total alarm time!\n");
			timer_response();
		}

		else if (strncmp(messageRx, "quit", strlen("quit")) == 0) {
			// printf("User selected to quit\n");
			quit_response();
		}
        
        else if (strncmp(messageRx, "sensor", strlen("sensor")) == 0) {
			// printf("User selected to get sensor\n");
			sensor_response();
		}
	}
    return NULL;
}

static void silence_response()
{
    if (Timer_getTimeRemaining() > 0) {
        printf("Alarm hasn't gone off!\n");
    }
    else {
        printf("ALARM SILENCED\n");
        Audio_stopScream();
        Timer_silenceAlarm();
    }
    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    sprintf(messageTx, "Alarm silenced.\n");

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
}

static void cup_response()
{
    // CHANGE PUMP TIMING TO 235mL
    Timer_setWaterAmount(CUP_ML);

    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    sprintf(messageTx, "Volume 235mL\n");

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
}

static void mug_response()
{
    // CHANGE PUMP TIMING TO 350mL
    Timer_setWaterAmount(MUG_ML);

    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    sprintf(messageTx, "Volume 350mL\n");

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
}

static void bottle_response()
{
    // CHANGE PUMP TIMING TO 500mL
    Timer_setWaterAmount(BOTTLE_ML);

    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    sprintf(messageTx, "Volume 500mL\n");

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
}

static void big_bottle_response()
{
    // CHANGE PUMP TIMING TO 930mL
    Timer_setWaterAmount(BIG_BOTTLE_ML);

    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    sprintf(messageTx, "Volume 930mL\n");

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
}

static void timer_up_response()
{
    long long currentTimer = Timer_getTimer();
    int newTime = currentTimer + 900;
    Timer_setTimer(newTime);

    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    sprintf(messageTx, "Timer increased.\n");

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
}

static void timer_down_response()
{
    long long currentTimer = Timer_getTimer();
    int newTime = currentTimer - 900;
    Timer_setTimer(newTime);

    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    sprintf(messageTx, "Timer decreased.\n");

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
}

static void quit_response()
{
    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    sprintf(messageTx, "Quitting.\n");

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);

    Shutdown_shutdown();
    stopping = true;
}

static void water_volume_response()
{
    // INSERT GET WATER VOLUME FUNCTIONS
    int amount = Timer_getWaterAmount();
    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    sprintf(messageTx, "%d", amount);

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
}

static void time_remaining_response()
{
    long long remainingTime = Timer_getTimeRemaining();

    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    sprintf(messageTx, "%lld", remainingTime);

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
}

static void timer_response()
{
    // Get total time for alarm
    long long time = Timer_getTimer();

    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    sprintf(messageTx, "%lld", time);

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
}

static void sensor_response()
{
    // Get distance
    bool isCupDetected = Timer_getSensor();
    // Compose the reply message:
    // (NOTE: watch for buffer overflows!).
    char messageTx[MSG_MAX_LEN];
    if (isCupDetected) {
        sprintf(messageTx, "true");
    } else {
        sprintf(messageTx, "false");
    }

    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
}