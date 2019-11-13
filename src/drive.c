#include <stdbool.h>

#include "ev3.h"
#include "ev3_tacho.h"

#include "drive.h"

//int angle;    /* Angle of rotation */

void _run_forever(int l_speed, int r_speed) {
	set_tacho_speed_sp(motor[L], l_speed);
	set_tacho_speed_sp(motor[R], r_speed);
	multi_set_tacho_ramp_up_sp(motor, 1000);
	multi_set_tacho_command_inx(motor, TACHO_RUN_FOREVER);
}

void _run_to_abs_pos(int l_speed, int l_pos, int r_speed, int r_pos) {
    set_tacho_speed_sp(motor[L], l_speed);
    set_tacho_speed_sp(motor[R], r_speed);
    set_tacho_position_sp(motor[L], l_pos);
    set_tacho_position_sp(motor[R], r_pos);
    multi_set_tacho_command_inx(motor, TACHO_RUN_TO_ABS_POS);
}

void _run_to_rel_pos(int l_speed, int l_pos, int r_speed, int r_pos) {
	set_tacho_speed_sp(motor[L], l_speed);
	set_tacho_speed_sp(motor[R], r_speed);
	set_tacho_position_sp(motor[L], l_pos);
	set_tacho_position_sp(motor[R], r_pos);
	multi_set_tacho_command_inx(motor, TACHO_RUN_TO_REL_POS);
}

void _run_timed(int l_speed, int r_speed, int ms) {
	set_tacho_speed_sp(motor[L], l_speed);
	set_tacho_speed_sp(motor[R], r_speed);
	multi_set_tacho_time_sp(motor, ms);
	multi_set_tacho_command_inx(motor, TACHO_RUN_TIMED);
}

void _run_direct(int l_speed, int r_speed) {
    set_tacho_speed_sp(motor[L], l_speed);
    set_tacho_speed_sp(motor[R], r_speed);
    multi_set_tacho_command_inx(motor, TACHO_RUN_DIRECT);
}

void _stop() {
	multi_set_tacho_command_inx(motor, TACHO_STOP);
}

void _reset() {
	multi_set_tacho_command_inx(motor, TACHO_RESET);
}

bool _is_running() {
	FLAGS_T state = TACHO_STATE__NONE_;

	if (get_tacho_state_flags(motor[L], &state) && state != TACHO_STATE__NONE_) return true;
	if (get_tacho_state_flags(motor[R], &state) && state != TACHO_STATE__NONE_) return true;

	return false;
}
