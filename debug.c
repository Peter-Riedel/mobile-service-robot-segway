#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#define Sleep(msec) usleep(msec * 1000)

#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"

#include "debug.h"

static bool check_pressed();

void print_sensor(uint8_t sn) {
    char s[256];
    uint32_t n;
    int val;

    printf("  type = %s\n", ev3_sensor_type(ev3_sensor[sn].type_inx));
    printf("  port = %s\n", ev3_sensor_port_name(sn, s));
    if (get_sensor_mode(sn, s, sizeof(s)))
        printf("  mode = %s\n", s);

    if (get_sensor_num_values(sn, &n)) {
        while (true) {
            printf("\r  ");
            for (uint32_t i = 0; i < n; i++) {
                if (get_sensor_value(i, sn, &val)) {
                    if (i > 0) printf(", ");
                    printf("value%d = %d", i, val);
                }
            }
            fflush(stdout);
            Sleep(200);
            printf("\r");
            for (uint32_t j = 0; j < n; j++)
                printf("                 ");
            fflush(stdout);
            if (check_pressed()) break;
            Sleep(200);
        }
    }
    printf("\n");
}

void print_motor(uint8_t sn) {
}

static bool check_pressed() {
    int val;

    return ev3_read_keys((uint8_t*) &val) && (val & EV3_KEY_UP);
}
