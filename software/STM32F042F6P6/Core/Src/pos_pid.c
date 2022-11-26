#include "pos_pid.h"

static PosPID _pos_pid;

void pos_pid_reset() {
	_pos_pid.cum_error_ = 0;
	_pos_pid.pre_error_ = 0;
}

void pos_pid_init(float p, float i, float d, int out_max, int out_min) {
	_pos_pid.p_ = p;
	_pos_pid.i_ = i;
	_pos_pid.d_ = d;
	
    _pos_pid.target_ = 0;
	_pos_pid.out_max_ = out_max;
	_pos_pid.out_min_ = out_min;

    pos_pid_reset();
}

void pos_pid_set_target(int16_t target) {
	_pos_pid.target_ = target;
	pos_pid_reset();
}

int16_t pos_pid_update(int16_t current) {
	int16_t error = _pos_pid.target_ - current;
	if (error <= 0) {
        return 0;
    }
	_pos_pid.cum_error_ += error;
	int16_t error_rate = (error - _pos_pid.pre_error_);
	_pos_pid.pre_error_ = error;
	
	float out = _pos_pid.p_ * error + _pos_pid.i_ * _pos_pid.cum_error_ + _pos_pid.d_ * error_rate;
	if (out > _pos_pid.out_max_) {
		out = _pos_pid.out_max_;
	} else if (out < _pos_pid.out_min_) {
		out = _pos_pid.out_min_;
	}
	return (int16_t)out;
}
