#ifndef DRIVE_H
#define DRIVE_H

#define SPEED_LINEAR	75  /* Motor speed for linear motion in percents */
#define SPEED_CIRCULAR	50  /* Motor speed for circular motion in percents */
//#define DEGREE_TO_COUNT( d )  (( d ) * 260 / 90 )

extern const int L_MOTOR_MAX_SPEED; /* Large motor maximal speed */
extern const int M_MOTOR_MAX_SPEED; /* Medium motor max speed */

enum { L, R }; /* Left, Right */
extern uint8_t motor[];
extern uint8_t m_motor;

typedef enum {
    MOVE_NONE,
	BALANCE,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    TURN_LEFT,
    TURN_RIGHT,
    TURN_ANGLE,
    STEP_BACKWARD,
	RESET,
	STOP
} move_t;

extern void move(int l_speed, int r_speed);
extern void turn(int speed, int steering);
extern void _run_forever(int l_speed, int r_speed);
extern void _run_to_abs_pos(int pos);
extern void _run_to_rel_pos(int pos);
extern void _run_timed(int l_speed, int r_speed, int ms);
extern void _run_direct(int l_speed, int r_speed);
extern void _stop();
extern void _reset();
extern bool _is_running();

#endif
