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

static uint32_t num_barrels;
static const uint32_t MAX_NUM_BARRELS = 3;
static color_t found_barrels[3];
static int speed; // drive forward / backwards
static int steering; // turn left / right
static int turn_angle;
static double u_t; // Stellgroesse u(t)
static int gyro_angle_origin;
static const int TILT_ANGLE = 20;
static bool check_ground;
static bool locate_barrel;
static bool reset;
static bool balance_restarted;
static move_t moving;   // Current moving
static move_t command;  // Command for the 'drive' coroutine

static CORO_CONTEXT(DRIVE);
static CORO_CONTEXT(BALANCE);
static CORO_CONTEXT(BORDER);
static CORO_CONTEXT(LOCATE);
static CORO_CONTEXT(IDENTIFY);
static CORO_CONTEXT(RESET);
static void rotateColorSensor();
static bool identifyBarrel();
static bool isBarrelColor(color_t color);
static bool foundBarrel(color_t color);
static const char* getBarrelColor(color_t color);
static void speakColor(const char *color);

CORO_DEFINE(DRIVE) {
	CORO_LOCAL int _wait_stopped = false;

	CORO_BEGIN();

	for ( ; ; ) {
		/* Waiting new command */
		CORO_WAIT(moving != command);

		switch (command) {
			case BALANCE:
				if (u_t < -L_MOTOR_MAX_SPEED)
					u_t = -L_MOTOR_MAX_SPEED;
				else if (u_t > L_MOTOR_MAX_SPEED)
					u_t = L_MOTOR_MAX_SPEED;

				move(u_t + steering, u_t - steering);

				// return to previous command
				command = moving;
				break;
			case MOVE_FORWARD:
				speed = 10;
				break;
			case MOVE_BACKWARD:
				speed = -10;
				break;
			case TURN_LEFT:
				steering = -10;
				break;
			case TURN_RIGHT:
				steering = 10;
				break;
			case STEP_BACKWARD:
				// time based?
				// speed = -10;
				break;
			case RESET:
				reset = true;
			case STOP:
				_stop();
				_wait_stopped = true;
				break;
			default:
				printf("Error: Unknown command\n");
		}
		moving = command;

		if (_wait_stopped) {
			// Waiting the command is completed
			CORO_WAIT(!_is_running());
			_wait_stopped = false;
			moving = command = MOVE_NONE;
		}
	}

	CORO_END();
}

CORO_DEFINE(BALANCE) {
	//CORO_LOCAL int state = 0; // 0 = Stillstand, > 0 vorwaerts, < 0 rueckwaerts
	CORO_LOCAL int gyro_angle; // Drehwinkel
	CORO_LOCAL int gyro_rate; // Drehrate
	CORO_LOCAL int motor_dist; // zurueckgelegter Weg
	CORO_LOCAL int motor_speed; // aktuelle Geschwindigkeit
	//K2 = 0.57 (3 Minuten)
	//CORO_LOCAL double K1 = 25, K2 = 0.57, K3 = 0.1, K4 = 0.019711, Kp = 1.15007, Ki = 0.235, Kd = 0.107; // constant params for e(t) and PID controller
	//CORO_LOCAL double K1 = 26, K2 = 0.695, K3 = 0.1, K4 = 0.019711, Kp = 1.15007, Ki = 0.235, Kd = 0.08; //Neue Werte 29.11.2019 funktionieren gut
	CORO_LOCAL double K1 = 22, K2 = 0.6, K3 = 0.1, K4 = 0.02, Kp = 1.15007, Ki = 0.24, Kd = 0.08; // constant params for e(t) and PID controller
	CORO_LOCAL double error, previous_error = 0, integral = 0, derivative; // aktuelle/vorherige Regelabweichung e(t)
	CORO_LOCAL clock_t start_t, end_t; // start time, end time
	CORO_LOCAL double dt; // time difference in sec

	CORO_BEGIN();

	CORO_WAIT(!balance_restarted);
	start_t = clock();

	for( ; ; ) {
		if (get_sensor_value(0, sensors.gyro, &gyro_angle) &&
			get_sensor_value(1, sensors.gyro, &gyro_rate) &&
			get_tacho_position(motor[L], &motor_dist) &&
			get_tacho_speed(motor[L], &motor_speed)) {

			// remove angle drift
			gyro_angle -= gyro_angle_origin;

			//printf("gyro_angle: %d\n", gyro_angle);
			//printf("gyro_rate: %d\n", gyro_rate);
			//printf("motor_dist: %d\n", motor_dist);
			//printf("motor_speed: %d\n", motor_speed);

			// check tilting over
			if (gyro_angle < -TILT_ANGLE || gyro_angle > TILT_ANGLE) {
				printf("Robot tilted over :(\n");
				previous_error = 0;
				integral = 0;
				balance_restarted = true;
				command = RESET;
				CORO_RESTART();
			}

			error = K1 * gyro_angle + K2 * gyro_rate + K3 * motor_dist + K4 * motor_speed + speed;
			printf("error: %.2lf\n", error);

            end_t = clock();
			dt = (double) (end_t - start_t) / CLOCKS_PER_SEC;
			//printf("dt: %lf\n", dt);

			integral += error * dt;
			//printf("integral: %lf\n", integral);
			derivative = (error - previous_error) / dt;
			//printf("derivative: %lf\n", derivative);

			u_t = Kp * error + Ki * integral + Kd * derivative + motor_speed; //MOTOR_SPEED WEGLASSEN + NEU BALANCEN
			//printf("u_t: %.2lf\n", u_t);

			command = BALANCE;

			start_t = end_t;
			previous_error = error;
		}

		if (reset) {
			previous_error = 0;
			integral = 0;
			balance_restarted = true;
			CORO_RESTART();
		}

		CORO_YIELD();
	}

	CORO_END();
}

CORO_DEFINE(BORDER) {
	CORO_LOCAL int brightness; // percent of reflected light

	CORO_BEGIN();

	for ( ; ; ) {
		CORO_WAIT(check_ground);

		if (get_sensor_value(0, sensors.color, &brightness)) {
			//printf("brightness: %d\n", brightness);

			// high brightness > 30 equals color white of the line border, <= 30 non-white
			if (brightness > 30) {
				//printf("lineborder: white\n");

				// TODO: border reached => turn and move other direction
				//command = STEP_BACKWARD;
			}
		}

		CORO_YIELD();
	}

	CORO_END();
}

CORO_DEFINE(LOCATE) {
	CORO_LOCAL int dist_current;//, dist_median; // distance in mm
	//CORO_LOCAL int angle_deviant; // angle in degree

	CORO_BEGIN();

	for ( ; ; ) {
		CORO_WAIT(locate_barrel);

/*
		do {
			// locate next barrel < 255cm
			// full left/right turn until 255cm
			if (get_sensor_value(0, sensors.us, &dist_current) && dist_current < 2550) {

				if (dist_current < dist_median) {
					dist_median = dist_current;
					angle_deviant = 0;
				} else {
					if (command == TURN_LEFT) angle_deviant++;
					else if (command == TURN_RIGHT) angle_deviant--;
				}

				// TODO: turn left or right by one degree
				// command = TURN_LEFT;
			} else {
				//locate_barrel = false;
			}

			CORO_YIELD();
		} while (locate_barrel);
*/

		if (get_sensor_value(0, sensors.us, &dist_current) && dist_current < 40) {
			check_ground = false;
			locate_barrel = false;
		}
		//printf("us_dist_cm: %d\n", dist_current / 10);

		CORO_YIELD();
	}

	CORO_END();
}

CORO_DEFINE(IDENTIFY) {
	CORO_BEGIN();

	for ( ; ; ) {
		CORO_WAIT(!locate_barrel && !check_ground);

		// maybe turning correction?

		rotateColorSensor();

		CORO_WAIT(identifyBarrel());

		rotateColorSensor();

		// TODO: drive backwards away from barrel
		// speed = -10;

		check_ground = true;
		locate_barrel = true;
	}

	CORO_END();
}

CORO_DEFINE(RESET) {
	CORO_LOCAL int pressed; // pressed touchsensor = 1, else 0
	CORO_BEGIN();

	for ( ; ; ) {
		CORO_WAIT(get_sensor_value(0, sensors.touch, &pressed) && pressed);
		command = RESET;
	}

	CORO_END();
}

void search() {
	if (reset) {
		// reset color sensor
		//_run_to_abs_pos(0);
		set_sensor_mode_inx(sensors.color, COLOR_COL_REFLECT);

		// reset motors
		_reset();

		reset = false;

		/*
		// debug
		printMotor(motor[L]);
		printMotor(motor[R]);
		printMotor(m_motor);
		*/
	}

/*	// countdown
	for (int sec = 3; sec > 0; sec--) {
		printf("%d...", sec);
		fflush(stdout);
		Sleep(1000);
	}
	printf("Go!\n");
*/

	// settings
	num_barrels = 0;
	speed = 0;
	steering = 0;
	u_t = 0;
	check_ground = true;
	locate_barrel = true;
	balance_restarted = false;
	moving = command = MOVE_NONE;

	printf("search barrels...\n");
	get_sensor_value(0, sensors.gyro, &gyro_angle_origin);
 	//printf("gyro_angle_origin: %d\n", gyro_angle_origin);
	Sleep(1000);

	do {
		CORO_CALL(DRIVE);
		CORO_CALL(BALANCE);
		CORO_CALL(BORDER);
		CORO_CALL(LOCATE);
		CORO_CALL(IDENTIFY);
		CORO_CALL(RESET);
	} while (!reset);
}

static void rotateColorSensor() {
	if (get_sensor_mode_inx(sensors.color) == COLOR_COL_COLOR) {
		_run_to_rel_pos(90);
		set_sensor_mode_inx(sensors.color, COLOR_COL_REFLECT);
	} else {
		_run_to_rel_pos(-105);
		set_sensor_mode_inx(sensors.color, COLOR_COL_COLOR);
	}
}

// identify and memorize barrels
static bool identifyBarrel() {
	color_t color;

	if (get_sensor_value(0, sensors.color, (int*) &color)) {
		if (isBarrelColor(color)) {
			const char *const barrel_color = getBarrelColor(color);

			if (!foundBarrel(color)) {
				printf("%s barrel found!\n", barrel_color);
				speakColor(barrel_color);
				found_barrels[num_barrels++] = color;

				if (num_barrels == MAX_NUM_BARRELS)
					command = RESET;
			} else {
				printf("%s barrel has already been found!\n", barrel_color);
			}

			if (num_barrels < 3)
				printf("%d %s left...\n", MAX_NUM_BARRELS - num_barrels, (num_barrels == 1 ? "barrels" : "barrel"));
			else
				printf("All barrels have been found!\n");

			return true;
		}
	}

	return false;
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
	snprintf(buf, sizeof(buf), "%s%s%s", "espeak '", color, "' --stdout | aplay -q &");
	system(buf);
}
