#include <stdio.h>
#include <stdbool.h>

#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"

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
                    printf("value%d = %6d", i, val);
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

void printMotor(uint8_t sn) {
	char s[256];
	int val;

	if (get_tacho_driver_name(sn, s, sizeof(s)))
		printf("  type = %s\n", s);
	if (get_tacho_address(sn, s, sizeof(s)))
		printf("  port = %s\n", s);
	if (get_tacho_commands(sn, s, sizeof(s)))
		printf("  commands = %s\n", s);
	if (get_tacho_count_per_rot(sn, &val))
		printf("  count_per_rot = %d\n", val);
	if (get_tacho_count_per_m(sn, &val))
        printf("  count_per_m = %d\n", val);
	if (get_tacho_full_travel_count(sn, &val))
		printf("  full_travel_count = %d\n", val);
	if (get_tacho_duty_cycle(sn, &val))
		printf("  duty_cycle = %d\n", val);
    if (get_tacho_duty_cycle_sp(sn, &val))
        printf("  duty_cycle_sp = %d\n", val);
	if (get_tacho_hold_pid_Kd(sn, &val))
		printf("  hold_pid_Kd = %d\n", val);
    if (get_tacho_hold_pid_Ki(sn, &val))
        printf("  hold_pid_Ki = %d\n", val);
    if (get_tacho_hold_pid_Kp(sn, &val))
        printf("  hold_pid_Kp = %d\n", val);
	if (get_tacho_max_speed(sn, &val))
		printf("  max_speed = %d\n", val);
	if (get_tacho_polarity(sn, s, sizeof(s)))
		printf("  polarity = %s\n", s);
	if (get_tacho_position(sn, &val))
		printf("  position = %d\n", val);
	if (get_tacho_position_sp(sn, &val))
		printf("  position_sp = %d\n", val);
	if (get_tacho_ramp_down_sp(sn, &val))
		printf("  ramp_down_sp = %d\n", val);
	if (get_tacho_ramp_up_sp(sn, &val))
		printf("  ramp_up_sp = %d\n", val);
	if (get_tacho_speed(sn, &val))
		printf("  speed = %d\n", val);
	if (get_tacho_speed_pid_Kd(sn, &val))
		printf("  speed_pid_Kd = %d\n", val);
	if (get_tacho_speed_pid_Ki(sn, &val))
		printf("  speed_pid_Ki = %d\n", val);
	if (get_tacho_speed_pid_Kp(sn, &val))
		printf("  speed_pid_Kp = %d\n", val);
	if (get_tacho_speed_sp(sn, &val))
		printf("  speed_sp = %d\n", val);
	if (get_tacho_state(sn, s, sizeof(s)))
		printf("  state = %s\n", s);
	if (get_tacho_stop_action(sn, s, sizeof(s)))
		printf("  stop_action = %s\n", s);
	if (get_tacho_stop_actions(sn, s, sizeof(s)))
		printf("  stop_actions = %s\n", s);
	if (get_tacho_time_sp(sn, &val))
		printf("  time_sp = %d\n", val);

	printf("\n");
}

static bool checkPressed() {
    uint8_t val;

    return ev3_read_keys(&val) && (val & EV3_KEY_UP || val & EV3_KEY_DOWN || val & EV3_KEY_LEFT || val & EV3_KEY_RIGHT);
}
