#ifndef DRIVE_H
#define DRIVE_H

#define SPEED_LINEAR	75  /* Motor speed for linear motion in percents */
#define SPEED_CIRCULAR	50  /* Motor speed for circular motion in percents */
//#define DEGREE_TO_COUNT( d )  (( d ) * 260 / 90 )

enum { L, R }; /* Left, Right */
extern uint8_t motor[];
extern int max_speed;  /* Motor maximal speed */

typedef enum {
    MOVE_NONE,
	BALANCE,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    TURN_LEFT,
    TURN_RIGHT,
    TURN_ANGLE,
    STEP_BACKWARD
} move_t;

extern void _run_forever(int l_speed, int r_speed);
extern void _run_to_abs_pos(int l_speed, int l_pos, int r_speed, int r_pos);
extern void _run_to_rel_pos(int l_speed, int l_pos, int r_speed, int r_pos);
extern void _run_timed(int l_speed, int r_speed, int ms);
extern void _run_direct(int l_speed, int r_speed);
extern void _stop();
extern void _reset();
extern bool _is_running();

#endif
