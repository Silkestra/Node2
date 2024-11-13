#pragma once
#include <stdint.h>
#include "sam.h"
#include "can.h"
#include <stdbool.h>
#include <float.h>

void motor_encoder_init();
int16_t read_encoder();
void shoot_ball(bool button_clicked);
void pwm_drive_motor_init();
void drive_motor(int16_t duty_cycle);
int16_t joy_y_to_duty_cycle(CanMsg msg);
int16_t scale_encoder_value(int16_t motor_value);
int16_t pi_controller(int16_t motor_value, CanMsg msg, int8_t K_p, int16_t* cumulative_error, int8_t K_i);
//void setup_timer_interrupt();
//void timer_handler();

/*
typedef struct {
	int8_t motor_value;
	CanMsg msg;
	int8_t K_p;
	int16_t cumulative_error;
	int8_t K_i;
} PI_controller_data;

extern PI_controller_data pi_data;*/