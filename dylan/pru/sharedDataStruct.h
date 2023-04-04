#include <stdint.h>

typedef struct {
    _Alignas(8) double currentDistance;
    
    _Alignas(uint16_t) short smileCount;
    _Alignas(8) uint64_t numMsSinceBigBang; 
} sharedMemStruct_t;
