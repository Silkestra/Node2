/*
 * GccApplication1.c
 *
 * Created: 16.10.2024 08:30:31
 * Author : augusal
 */ 
#include "can.h"
#include "sam.h"
#include "uart.h"
#include "PWM.h"
#include "ADC.h"
#include "game.h"
#include "motor_driver.h"

int main(void)
{
	SystemInit();
	uart_init(F_CPU,BAUDRATE);

	WDT->WDT_MR = WDT_MR_WDDIS;
	CanMsg msg = {0x000, 0x03, {0x00, 0x00, 0x00}};
	CanMsg msg1={0x000,0x02,{0xBE, 0x53}};
	CanMsg msg2={0x000,0x02,{0xA4, 0x23}};
	
	float T_Q = 0.0000005;
	long F_OSC = 84 * 1000000;
	int BRP = (T_Q * F_OSC / 2) - 1;
	
	CanInit canSettings={.brp=41, .phase1 = 6, .phase2 = 5, .propag= 0, .smp= 0, .sjw=2};

	can_init(canSettings, 0); // Disable receive interrupts
	pwm_init();
	int16_t joy_value_prev = 100;
	
	adc_init();
	bool hit;
	
	solenoid_init();
	motor_encoder_init();
	int16_t motor_value;
	bool button_clicked;
	int16_t motor_pwm;
	pwm_drive_motor_init();
	int8_t K_p = 1;
	volatile int16_t cumulative_error = 0;
	int8_t K_i = 1;

    while (1) 
    {	
		if(!can_rx(&msg)){
			//printf("No");
		}
	
		int16_t duty_cycle = joy_x_to_duty_cycle(msg, &joy_value_prev);  // Calculate the duty cycle
		pwm_set_duty_cycle(duty_cycle);
		
		button_clicked = msg.byte[2];
		shoot_ball(button_clicked);
		
		
		hit = register_hit();
		
		
		if(hit){
			printf("hit");
			CanMsg msg2 = {0x001,0x01,{0x1}};
			can_tx(msg2);
			CanMsg msg3 = {0x002, 0x01, {0x1}};
			can_tx(msg3);
		} else {printf("no");}
		
		motor_value = read_encoder();
		int16_t scaled_encoder = scale_encoder_value(motor_value);
		
		int16_t duty_cycle_2 = pi_controller(scaled_encoder, msg, K_p, &cumulative_error, K_i);
		drive_motor(duty_cycle_2);
    }
	
}