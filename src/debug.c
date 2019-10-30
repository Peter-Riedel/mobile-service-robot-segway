#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#define Sleep(msec) usleep(msec * 1000)

#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"

#include "debug.h"

static bool checkPressed();

void printSensor(uint8_t sn) {
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
            if (checkPressed()) break;
            Sleep(200);
        }
    }
    printf("\n");
}

const char* getSensorName(uint32_t sensor_type) {
	switch (sensor_type) {
		case LEGO_EV3_US: return "Ultrasound";
		case LEGO_EV3_GYRO: return "Gyro";
		case LEGO_EV3_COLOR: return "Color";
		case LEGO_EV3_TOUCH: return "Touch";
		default: return "Unknown";
	}
}

// TODO: implement motor debug & print info
void printMotor(uint8_t sn) {
	
}

// TODO: return motor names as string
const char* getMotorName(uint32_t motor_type) {
	switch (motor_type) {
		default: return "Unknown";
	}
}

static bool checkPressed() {
    uint8_t val;

    return ev3_read_keys(&val) && (val & EV3_KEY_UP || val & EV3_KEY_DOWN || val & EV3_KEY_LEFT || val & EV3_KEY_RIGHT || val & EV3_KEY_CENTER || val & EV3_KEY_BACK);
}
