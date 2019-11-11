#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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

//static bool a;
static uint32_t num_barrels;
static const uint32_t MAX_NUM_BARRELS = 3;
static color_t found_barrels[3];

CORO_CONTEXT(BALANCE);
CORO_CONTEXT(OBSERVE_GYRO);
//static void drive(int move);
static bool isBarrelColor(color_t color);
static bool foundBarrel(color_t color);
static const char* getBarrelColor(color_t color);
static void speakColor(const char *color);

int gyro_angle_origin; // SOLL-Lagewinkel
int gyro_angle_current; // IST-Lagewinkel
int gyro_angle_diff; // Regelabweichung e(t) = IST - SOLL
int Kp, Ki, Kd;
int motor_speed; // Stellgroesse u(t) = Kp * e(t) + Ki * [e(t)*t]_0_? + Kd * 0
bool balance = false;

CORO_DEFINE(OBSERVE_GYRO) {
	CORO_BEGIN();
	for( ; ; ) {
		if (get_sensor_value(0, sensors.gyro, &gyro_angle_current)) {
			gyro_angle_diff = gyro_angle_current -  gyro_angle_origin;
			printf("gyro_angle_diff: %d\n", gyro_angle_diff);

			if (gyro_angle_diff > 0 || gyro_angle_diff < 0) {
				get_tacho_speed_pid_Kp(motor[L], &Kp);
				get_tacho_speed_pid_Ki(motor[L], &Ki);
				get_tacho_speed_pid_Kd(motor[L], &Kd);
				motor_speed = Kp * gyro_angle_diff/* + Ki * ?*/ + Kd * 0;

				balance = true;
			} else _stop();
		}

		CORO_YIELD();
	}
	CORO_END();
}

CORO_DEFINE(BALANCE) {
	CORO_BEGIN();
	for ( ; ; ) {
		printf("BALANCE: Waiting...\n");
		CORO_WAIT(balance);

		_run_forever(motor_speed, motor_speed);
		//multi_set_tacho_speed_sp(motor, motor_speed);

		balance = false;
	}
	CORO_END();
}

void search() {
	printf("search barrels...\n");
	//printf("max_speed: %d\n", max_speed);
	get_sensor_value(0, sensors.gyro, &gyro_angle_origin);
 	printf("gyro_angle_origin: %d\n", gyro_angle_origin);
	Sleep(3000);
	do {
		CORO_CALL(BALANCE);
		CORO_CALL(OBSERVE_GYRO);
	} while (true);
	//drive(MOVE_BACKWARD);
	//_run_forever(1000,-1000);
	//_run_to_abs_pos(-300, 180, 300, 180);
	//_run_to_rel_pos(-1000, 360, 1000, 90);
	//_run_timed(1000, -1000, 10000);
	//_run_direct(1000, 1000);
	//Sleep(10000);
	//_stop();

	num_barrels = 0;

	do {
		// TODO: implement detection of barrels with US sensor

		// memorize and identify barrels
		color_t color;
		if (get_sensor_value(0, sensors.color, (int*) &color)) {
			if (isBarrelColor(color) && !foundBarrel(color)) {
				const char *barrel_color = getBarrelColor(color);
				printf("%s barrel found!\n", barrel_color);
				speakColor(barrel_color);
				found_barrels[num_barrels++] = color;
			}
		}
	} while (num_barrels < MAX_NUM_BARRELS);
}
/*
static void drive(int move) {
	switch (move) {
		case MOVE_NONE:
			_stop();
			break;
		case MOVE_FORWARD:
			_run_forever(max_speed * 0.75 * abs(angle-origin)/20, max_speed * 0.75 * abs(angle-origin)/20);
			break;
		case MOVE_BACKWARD:
			_run_forever(-max_speed * 0.75 * abs(angle-origin)/20, -max_speed * 0.75 * abs(angle-origin)/20);
			break;
		case TURN_LEFT:
			_run_forever(-max_speed, max_speed);
			break;
		case TURN_RIGHT:
			_run_forever(max_speed, -max_speed);
			break;
		default: return;
	}
}
*/

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
