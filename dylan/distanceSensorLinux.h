#ifndef DISTANCE_SENSOR_LINUX_H
#define DISTANCE_SENSOR_LINUX_H

void DistanceSensor_init(void);
void DistanceSensor_cleanup(void);
double DistanceSensor_getDistance(void);

#endif