#ifndef DEBUG_H
#define DEBUG_H

#include <unistd.h>
#define Sleep(msec) usleep(msec * 1000)

extern void printSensor(uint8_t sn);
extern const char* getSensorName(uint32_t sensor_type);
extern void printMotor(uint8_t sn);
extern const char* getMotorName(uint32_t motor_type);

#endif
