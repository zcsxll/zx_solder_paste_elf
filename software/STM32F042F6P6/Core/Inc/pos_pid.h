#ifndef POS_PID_H_
#define POS_PID_H_

#include "stm32f0xx_hal.h"

typedef struct {
	float p_;
	float i_;
	float d_;
	
	int16_t target_;
	int16_t cum_error_;
	int16_t pre_error_;
	
	int16_t out_max_;
	int16_t out_min_;
}PosPID;

void pos_pid_init(float p, float i, float d, int out_max, int out_min);
void pos_pid_set_target(int16_t target);
int16_t pos_pid_update(int16_t current);

#endif
