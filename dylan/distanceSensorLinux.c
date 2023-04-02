#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "sharedDataStruct.h"
#include "distanceSensorLinux.h"

#define PRU_ADDR 0x4A300000
#define PRU0_DRAM 0x00000
#define PRU_LEN 0x80000  
#define PRU_SHAREDMEM 0x10000

#define PRU0_MEM_FROM_BASE(base) ( (base) + PRU0_DRAM + PRU_MEM_RESERVED)

static volatile sharedMemStruct_t* pSharedPru0 = NULL;

static volatile void* getPruMmapAddr(void);
static void freePruMmapAddr(volatile void* pPruBase);

void DistanceSensor_init(void)
{
    pSharedPru0 = getPruMmapAddr();
}

void DistanceSensor_cleanup(void)
{
    freePruMmapAddr(pSharedPru0);
}

double DistanceSensor_getDistance(void)
{
    return pSharedPru0->currentDistance;
}

static volatile void* getPruMmapAddr(void)
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }

    // Points to start of PRU memory.
    volatile void* pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);

    return pPruBase;
}

static void freePruMmapAddr(volatile void* pPruBase)
{
    if (munmap((void*) pPruBase, PRU_LEN)) {
        perror("PRU munmap failed");
        exit(EXIT_FAILURE);
    }
}