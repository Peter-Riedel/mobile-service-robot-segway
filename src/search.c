#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "ev3.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"
#include "coroutine.h"

#include "debug.h"
#include "search.h"
#include "drive.h"

typedef enum {
	COLOR_NONE,
	BLACK,
	BLUE,
	GREEN,
	YELLOW,
	RED,
	WHITE,
	BROWN
} color_t;

static move_t moving;   /* Current moving */
static move_t command;  /* Command for the 'drive' coroutine */
static int motor_speed_correction;
static bool check_ground;
static bool locate_barrel;
static bool reset;
static uint32_t num_barrels;
static const uint32_t MAX_NUM_BARRELS = 3;
static color_t found_barrels[3];

static CORO_CONTEXT(DRIVE);
static CORO_CONTEXT(BALANCE);
static CORO_CONTEXT(BORDER);
static CORO_CONTEXT(LOCATE);
static CORO_CONTEXT(RESET);
static void identifyBarrel();
static bool isBarrelColor(color_t color);
static bool foundBarrel(color_t color);
static const char* getBarrelColor(color_t color);
static void speakColor(const char *color);

CORO_DEFINE(DRIVE) {
	CORO_LOCAL int speed_linear, speed_circular;
	CORO_LOCAL int _wait_stopped = false;

	CORO_BEGIN();
	speed_linear = max_speed * SPEED_LINEAR / 100;
	speed_circular = max_speed * SPEED_CIRCULAR / 100;

	for ( ; ; ) {
		/* Waiting new command */
		CORO_WAIT(moving != command || command == BALANCE);

		switch (command) {
			case MOVE_NONE:
				_stop();
				_wait_stopped = true;
				reset = true;
				break;
			case BALANCE:
				int motor_speed_current;
				get_tacho_speed(motor[L], &motor_speed_current);
				_run_forever(motor_speed_current + motor_speed_correction, motor_speed_current + motor_speed_correction);
				break;
			case MOVE_FORWARD:
				_run_forever(speed_linear*0.1, speed_linear*0.1);
				break;
			case MOVE_BACKWARD:
				_run_forever(-speed_linear*0.1, -speed_linear*0.1);
				break;
			case TURN_LEFT:
				_run_forever(-speed_circular, speed_circular);
				break;
			case TURN_RIGHT:
				_run_forever(speed_circular, -speed_circular);
				break;
			default:
				printf("Error: Unknown command\n");
		}
		moving = command;

		if (_wait_stopped) {
			/* Waiting the command is completed */
			CORO_WAIT(_is_running());
			_wait_stopped = false;
			//command = moving = MOVE_NONE;
		}
	}
	CORO_END();
}

CORO_DEFINE(BALANCE) {
	// SOLL-Lagewinkel, IST-Lagewinkel, aktuelle Regelabweichung e(t) = IST - SOLL, vorherige Regelabweichung
	CORO_LOCAL int gyro_angle_origin, gyro_angle_current, gyro_angle_diff = 0, gyro_angle_diff_prev;
	CORO_LOCAL int Kp = 100, Ki = 50, Kd = -100; // constant params for PID controller
	CORO_LOCAL time_t start_t, end_t = time(NULL); // start time, end time
	CORO_LOCAL double diff_t; // difference between end time and start time
	CORO_LOCAL double area = 0; // summation of integral
	
	CORO_BEGIN();
	for( ; ; ) {
		if (get_sensor_value(0, sensors.gyro, &gyro_angle_current)) {
			gyro_angle_diff_prev = gyro_angle_diff;
			gyro_angle_diff = gyro_angle_current - gyro_angle_origin;
			printf("gyro_angle_diff: %d\n", gyro_angle_diff);
			
			start_t = end_t;
			diff_t = difftime(time(&end_t), start_t);
			area += diff_t * (gyro_angle_diff + gyro_angle_diff_prev) / 2;
			
			motor_speed_correction = (int) (Kp * gyro_angle_diff +
											Ki * area +
											Kd * (gyro_angle_diff - gyro_angle_diff_prev) / diff_t);
			
			command = BALANCE;
		}

		CORO_YIELD();
	}
	CORO_END();
}


CORO_DEFINE(BORDER) {
	CORO_LOCAL int brightness; // percent of reflected light
	//set_sensor_mode_inx(sensors.color, COLOR_COL_REFLECT);

	CORO_BEGIN();
	for ( ; ; ) {
		CORO_WAIT(check_ground);

		if (get_sensor_value(0, sensors.color, &brightness)) {
			if (brightness > 30) { // high brightness equals color white
				printf("lineborder: white\n");
				// TODO: border reached => turn and move other direction
			}
		}

		CORO_YIELD();
	}
	CORO_END();
}

CORO_DEFINE(LOCATE) {
	CORO_LOCAL int dist_current, dist_median; // distance in mm
	CORO_LOCAL int angle_deviant; // angle in degree

	CORO_BEGIN();
	for ( ; ; ) {
		CORO_WAIT(locate_barrel);

		get_sensor_value(0, sensors.us, &dist_current);

		if (30 < dist_current && dist_current < 2550) { // between 3cm and 255cm equals possibility to find barrel
			//printf("us_dist_cm: %d\n", dist_current / 10);
			if (dist_current < dist_median) {
				dist_median = dist_current;
				angle_deviant = 0;
			} else angle_deviant++;

			// TODO: turn left or right by one degree
			// command = TURN_LEFT;
		} else {
			locate_barrel = false;
		}

		CORO_YIELD();
	}
	CORO_END();
}

CORO_DEFINE(RESET) {
	CORO_LOCAL int pressed; // pressed touchsensor = 1, else 0
	CORO_BEGIN();
	for ( ; ; ) {
		CORO_WAIT(get_sensor_value(0, sensors.touch, &pressed) && pressed);
		command = MOVE_NONE;
	}
	CORO_END();
}

void search() {
	check_ground = true;
	locate_barrel = true;
	num_barrels = 0;
	reset = false;

	printf("search barrels...\n");
	//printf("max_speed: %d\n", max_speed);
	get_sensor_value(0, sensors.gyro, &gyro_angle_origin);
 	//printf("gyro_angle_origin: %d\n", gyro_angle_origin);
	Sleep(1000);

	do {
		CORO_CALL(DRIVE);
		CORO_CALL(BALANCE);
		CORO_CALL(BORDER);
		CORO_CALL(LOCATE);
		CORO_CALL(RESET);

		//identifyBarrel();
	} while (num_barrels < MAX_NUM_BARRELS && !reset);

	_reset();
}

// identify and memorize barrels
static void identifyBarrel() {
	color_t color;

	if (get_sensor_value(0, sensors.color, (int*) &color)) {
		if (isBarrelColor(color) && !foundBarrel(color)) {
			const char *const barrel_color = getBarrelColor(color);
			printf("%s barrel found!\n", barrel_color);
			speakColor(barrel_color);
			found_barrels[num_barrels++] = color;
		}
	}
}

static bool isBarrelColor(color_t color) {
	if (color == GREEN || color == YELLOW || color == RED)
		return true;

	return false;
}

static bool foundBarrel(color_t color) {
	for (uint32_t i = 0; i < num_barrels; i++) {
		if (found_barrels[i] == color)
			return true;
	}

	return false;
}

static const char* getBarrelColor(color_t color) {
	switch (color) {
		case GREEN: return "GREEN";
		case YELLOW: return "YELLOW";
		case RED: return "RED";
		default: return "NONE";
	}
}

static void speakColor(const char *color) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%s%s%s", "espeak '", color, "' --stdout | aplay");
	system(buf);
}
