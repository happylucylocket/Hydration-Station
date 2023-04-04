#ifndef PUMP_H_
#define PUMP_H_ 

#define FLOW_RATE_PER_MS 0.03
#define CUP_ML 235
#define MUG_ML 350
#define BOTTLE_ML 500
#define BIG_BOTTLE_ML 930

void Pump_init(void);
void Pump_turnOff(void);
void Pump_turnOn(void);
void Pump_pumpML(int amount);

#endif