#ifndef SEARCH_H
#define SEARCH_H

typedef struct {
    uint8_t us;
    uint8_t gyro;
    uint8_t color;
    uint8_t touch;
} sensors_t;

extern sensors_t sensors;

extern void search();

#endif
