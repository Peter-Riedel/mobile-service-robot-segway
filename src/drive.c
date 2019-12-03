#include "ev3.h"
#include "ev3_tacho.h"

#include "drive.h"

void move(int l_speed, int r_speed) {
	set_tacho_speed_sp(motor[L], l_speed);
	set_tacho_speed_sp(motor[R], r_speed);
	multi_set_tacho_command_inx(motor, TACHO_RUN_FOREVER);
}

void turn(int speed, int steering) {
	set_tacho_speed_sp(motor[L], speed + steering);
	set_tacho_speed_sp(motor[R], speed - steering);
	multi_set_tacho_command_inx(motor, TACHO_RUN_FOREVER);
}

void _run_forever(int l_speed, int r_speed) {
	set_tacho_speed_sp(motor[L], l_speed);
	set_tacho_speed_sp(motor[R], r_speed);
	multi_set_tacho_command_inx(motor, TACHO_RUN_FOREVER);
}

void _run_to_abs_pos(int pos) {
	set_tacho_position_sp(m_motor, pos);
	set_tacho_command_inx(m_motor, TACHO_RUN_TO_ABS_POS);

	FLAGS_T state = TACHO_STATE__NONE_;
	while (get_tacho_state_flags(m_motor, &state) && state != TACHO_STATE__NONE_);
}

void _run_to_rel_pos(int pos) {
	set_tacho_position_sp(m_motor, pos);
	set_tacho_command_inx(m_motor, TACHO_RUN_TO_REL_POS);

	FLAGS_T state = TACHO_STATE__NONE_;
	while (get_tacho_state_flags(m_motor, &state) && state != TACHO_STATE__NONE_);
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
	// reset large motors
	multi_set_tacho_command_inx(motor, TACHO_RESET);

	// large motor settings
	multi_set_tacho_polarity_inx(motor, TACHO_NORMAL);
	multi_set_tacho_stop_action_inx(motor, TACHO_BRAKE);
	multi_set_tacho_speed_pid_Kp(motor, 1000);
	multi_set_tacho_speed_pid_Ki(motor, 60);
	multi_set_tacho_speed_pid_Kd(motor, 0);

	// reset medium motor
	set_tacho_command_inx(m_motor, TACHO_RESET);

	// medium motor settings
	set_tacho_polarity_inx(m_motor, TACHO_NORMAL);
	set_tacho_stop_action_inx(m_motor, TACHO_COAST);
	set_tacho_speed_sp(m_motor, 2/3. * M_MOTOR_MAX_SPEED);
	//set_tacho_ramp_up_sp(m_motor, 1/3. * M_MOTOR_MAX_SPEED);
}

bool _is_running() {
	FLAGS_T state = TACHO_STATE__NONE_;

	if (get_tacho_state_flags(motor[L], &state) && state != TACHO_STATE__NONE_) return true;
	if (get_tacho_state_flags(motor[R], &state) && state != TACHO_STATE__NONE_) return true;

	return false;
}
