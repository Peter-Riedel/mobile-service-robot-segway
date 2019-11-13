// C standard lib
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

// ev3dev-c lib
#include "ev3.h"
#include "ev3_light.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"

// custom lib
#include "debug.h"
#include "search.h"
#include "drive.h"

#define L_MOTOR_PORT      OUTPUT_C
#define L_MOTOR_EXT_PORT  EXT_PORT__NONE_
#define R_MOTOR_PORT      OUTPUT_A
#define R_MOTOR_EXT_PORT  EXT_PORT__NONE_
sensors_t sensors;
uint8_t motor[3] = { DESC_LIMIT, DESC_LIMIT, DESC_LIMIT };
int max_speed;

typedef enum {
	START,
	EXIT,
	POWEROFF,
	UNKNOWN
} cmd_t;

static bool debug;

static void init();
static void prompt(char *const input);
static cmd_t getCommand(char *const input);
static void start();
static void printError(const char *const message);
static void fail(const char *const message);

int main(int argc, char* argv[]) {
	if (argc >= 2) {
		if (strcmp(argv[1], "-debug") == 0) {
			printf("*********\n");
			printf("  DEBUG  \n");
			printf("*********\n");
			debug = true;
		}
	}

	init();
	printf("**********************************\n");
	printf("  MOBILE SERVICE-ROBOTER: SEGWAY  \n");
	printf("**********************************\n");

	char input[256];
	do {
		prompt(input);

		switch (getCommand(input)) {
			case START:
				start();
				break;
			case EXIT:
				ev3_uninit();
				return EXIT_SUCCESS;
			case POWEROFF:
				ev3_poweroff();
			default:
				printError("Unknown command");
		}
	} while (true);
}

static void init() {
	bool error = false;

	if (ev3_init() < 1) {
		fail("Unable to initialize EV3 brick");
	}

	// PORTS
	if (ev3_port_init() != 8) {
		fail("Unable to initialize all ports");
	}

	// SENSORS
	if (ev3_sensor_init() != 4) {
		printError("Unable to initialize all sensors");
		error = true;
	} else if (debug)
		printf("Sensors:\n");

	// touch sensor maybe not necessary --> lab test
	uint8_t *snp = (uint8_t*) &sensors;
	for (uint32_t sensor_type = LEGO_EV3_US; sensor_type <= LEGO_EV3_TOUCH; sensor_type++) {
		if (!ev3_search_sensor(sensor_type, snp, 0)) {
			printf("Error: %s sensor not found\n", getSensorName(sensor_type));
			error = true;
		} else {
			// Configure mode of sensor
			uint32_t sensor_mode;
			switch (sensor_type) {
				case LEGO_EV3_US:
					sensor_mode = US_US_DIST_CM;
					break;
				case LEGO_EV3_GYRO:
					sensor_mode = GYRO_GYRO_ANG;
					break;
				case LEGO_EV3_COLOR:
					sensor_mode = COLOR_COL_REFLECT;
					break;
				case LEGO_EV3_TOUCH:
					sensor_mode = TOUCH_TOUCH;
					break;
				default:
					// Warning
					printError("Unknown sensor type");
			}

			if (!set_sensor_mode_inx(*snp, sensor_mode)) {
				// Warning
				printError("Unable to set sensor mode");
			}

			if (debug) {
				printSensor(*snp);
				Sleep(300);
			}
		}

		snp++;
	}

	// MOTORS
	if (ev3_tacho_init() != 3) {
		printError("Unable to initialize all tacho motors");
		error = true;
	} else if (debug)
		printf("Motors:\n");
/*
	if (!ev3_search_tacho(LEGO_EV3_M_MOTOR, &motor[M], 0)) {
		printError("Medium tacho motor not found");
		error = true;
	} else if (debug) {
		printMotor(motor[M]);
		Sleep(300);
	}
*/

	char port_name[5];
	if (ev3_search_tacho_plugged_in(L_MOTOR_PORT, L_MOTOR_EXT_PORT, &motor[L], 0)) {
		get_tacho_max_speed(motor[L], &max_speed);
		// Reset the motor
        set_tacho_command_inx(motor[L], TACHO_RESET);
		//set_tacho_polarity_inx(motor[L], TACHO_INVERSED);
		//set_tacho_stop_action_inx(motor[L], TACHO_BRAKE);

		if (debug) {
			printMotor(motor[L]);
			Sleep(300);
		}
    } else {
        printf("Error: Left motor (%s) not found\n", ev3_port_name(L_MOTOR_PORT, L_MOTOR_EXT_PORT, 0, port_name));
		error = true;
    }

    if (ev3_search_tacho_plugged_in(R_MOTOR_PORT, R_MOTOR_EXT_PORT, &motor[R], 0)) {
        // Reset the motor
        set_tacho_command_inx(motor[R], TACHO_RESET);
		//set_tacho_stop_action_inx(motor[R], TACHO_BRAKE);

		if (debug) {
			printMotor(motor[R]);
			Sleep(300);
		}
    } else {
        printf("Error: Right motor (%s) not found\n", ev3_port_name(R_MOTOR_PORT, R_MOTOR_EXT_PORT, 0, port_name));
		error = true;
    }

	if (error) {
		fail("Failed initialization - robot is inoperative");
	}
}

static void prompt(char *const input) {
	set_light(LIT_LEFT, LIT_GREEN);
	set_light(LIT_RIGHT, LIT_GREEN);
	printf("cmd: ");
	scanf("%256s", input);
}

static cmd_t getCommand(char *const input) {
	for (char *p = input; *p; p++)
		*p = tolower(*p);

	if (strcmp("start", input) == 0)
		return START;
	else if (strcmp("exit", input) == 0)
		return EXIT;
	else if (strcmp("poweroff", input) == 0)
		return POWEROFF;
	else return UNKNOWN;
}

static void start() {
	set_light(LIT_LEFT, LIT_OFF);
	set_light(LIT_RIGHT, LIT_OFF);
	set_light_blink(LIT_LEFT, LIT_AMBER, 500, 500);
	Sleep(500);
	set_light_blink(LIT_RIGHT, LIT_AMBER, 500, 500);
	search();
}

static void printError(const char *const message) {
	printf("Error: %s\n", message);
}

static void fail(const char *const message) {
	set_light(LIT_LEFT, LIT_RED);
	set_light(LIT_RIGHT, LIT_RED);
	printError(message);
	exit(EXIT_FAILURE);
}
