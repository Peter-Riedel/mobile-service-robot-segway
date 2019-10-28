// C standard lib
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#define Sleep(msec) usleep(msec * 1000)

// ev3dev-c lib
#include "ev3.h"
#include "ev3_light.h"
#include "ev3_port.h"
#include "ev3_sensor.h"

// custom lib
#include "debug.h"
#include "search.h"

typedef enum {
	START,
	EXIT,
	POWEROFF,
	UNKNOWN
} cmd_t;

static sensors_t sensors;
static motors_t motors;
static bool debug;

static void init();
static cmd_t getCommand(const char *input);

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

	const char input[256];
	do {
		set_light(LIT_LEFT, LIT_GREEN);
		set_light(LIT_RIGHT, LIT_GREEN);
		printf("cmd: ");
		scanf("%256s", input);

		switch (getCommand(input)) {
			case START:
				set_light(LIT_LEFT, LIT_OFF);
				set_light(LIT_RIGHT, LIT_OFF);
				set_light_blink(LIT_LEFT, LIT_AMBER, 500, 500);
				Sleep(500);
				set_light_blink(LIT_RIGHT, LIT_AMBER, 500, 500);
				search(&sensors);
				break;
			case EXIT:
				ev3_uninit();
				return EXIT_SUCCESS;
			case POWEROFF:
				ev3_poweroff();
			default:
				set_light(LIT_LEFT, LIT_RED);
				set_light(LIT_RIGHT, LIT_RED);
				printf("Error: Unknown command\n");
		}
	} while (true);
}

static void init() {
	if (ev3_init() < 1) {
		printf("Error: Unable to initialize EV3 brick\n");
		exit(EXIT_FAILURE);
	}

	bool error = false;

	// SENSORS
	ev3_sensor_init();
	uint8_t *sn = (uint8_t*) &sensors;
	
	/* TESTEN: Vereinfachter Init für Sensoren
	for (uint8_t *sn = (uint8_t) &sensors, sensor_type = (uint8_t) LEGO_EV3_US; sensor_type <= LEGO_EV3_TOUCH; sn++, sensor_type++) {
		if (!ev3_search_sensor(sensor_type, sn, 0)) {
			printf("Error: %s sensor not found\n", getSensorName(sensor_type));
			error = true;
		} else if (debug) {
			printSensor(*sn);
			Sleep(500);
		}
	}
	*/

	if (!ev3_search_sensor(LEGO_EV3_US, sn, 0)) {
		printf("Error: Ultrasonic sensor not found\n");
		error = true;
	} else if (debug) {
		printSensor(*sn);
		Sleep(500);
	}
	sn++;

	if (!ev3_search_sensor(LEGO_EV3_GYRO, sn, 0)) {
		printf("Error: Gyro sensor not found\n");
		error = true;
	} else if (debug) {
		printSensor(*sn);
		Sleep(500);
	}
	sn++;

	if (!ev3_search_sensor(LEGO_EV3_COLOR, sn, 0)) {
		printf("Error: Color sensor not found\n");
		error = true;
	} else if (debug) {
		printSensor(*sn);
		Sleep(500);
	}
	sn++;

	// touch sensor maybe not necessary --> lab test
	if (!ev3_search_sensor(LEGO_EV3_TOUCH, sn, 0)) {
		printf("Error: Touch sensor not found\n");
		error = true;
	} else if (debug) {
		printSensor(*sn);
		Sleep(500);
	}

	// MOTORS
	
	/* TODO und TESTEN: analog zu Sensoren Init für Motoren
	
	for (uint8_t *sn = (uint8_t) &motors, motor_type = ?; motor_type <= ?; sn++, motor_type++) {
		if (!ev3_search_motor(motor_type, sn, 0)) {
			printf("Error: %s motor not found\n", getMotorName(motor_type));
			error = true;
		} else if (debug) {
			printMotor(*sn);
			Sleep(500);
		}
	}
	*/

	if (error) {
		printf("Failed initialization: Robot is inoperative\n");
		exit(EXIT_FAILURE);
	}
	
	// TODO: coroutine for driving/standing (parallel to main process)
}

static cmd_t getCommand(const char *input) {
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
