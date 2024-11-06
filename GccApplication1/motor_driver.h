#pragma once
#include <stdint.h>
#include "sam.h"
#include "can.h"
#include <stdbool.h>

void motor_encoder_init();
uint32_t read_encoder();
void shoot_ball(bool button_clicked);
void pwm_drive_motor_init();
void drive_motor(int16_t duty_cycle);
int16_t joy_y_to_duty_cycle(CanMsg msg);