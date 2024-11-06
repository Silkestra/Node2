#pragma once

#include <stdint.h>
#include "can.h"

void pwm_init(void);
void pwm_set_duty_cycle(uint16_t duty_cycle);
int16_t joy_x_to_duty_cycle(CanMsg msg, int16_t* prev);