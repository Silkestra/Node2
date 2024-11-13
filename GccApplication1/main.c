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

//PI_controller_data pi_data;

int main(void)
{
	SystemInit();
	uart_init(F_CPU,BAUDRATE);
    
	/*REG_PMC_PCER0 |= (1 << ID_PIOB);
    REG_PIOB_PER = (1 << 13);
	REG_PIOB_OER = (1 << 13);
	*/
	
	WDT->WDT_MR = WDT_MR_WDDIS;
	CanMsg msg = {0x000, 0x03, {0x00, 0x00, 0x00}};
	CanMsg msg1={0x000,0x02,{0xBE, 0x53}};
	CanMsg msg2={0x000,0x02,{0xA4, 0x23}};
	
	float T_Q = 0.0000005;
	long F_OSC = 84 * 1000000;
	int BRP = (T_Q * F_OSC / 2) - 1;
	
	// Configure CAN initialization structure
	CanInit canSettings={.brp=41, .phase1 = 6, .phase2 = 5, .propag= 0, .smp= 0, .sjw=2};
		/*
	canSettings.propag = 1;
	canSettings.phase1 = 6;
	canSettings.phase2 = 5;
	canSettings.sjw = 2;
	canSettings.brp = BRP;    // Set BRP as calculated earlier
	canSettings.smp = 0;      // Use sample mode
	*/
	// Initialize CAN with receive interrupts enabled
	can_init(canSettings, 0); // Disable receive interrupts
	pwm_init();
	int16_t joy_value_prev = 100;
	//pwm_set_duty_cycle(200);
	adc_init();
	bool hit;
	solenoid_init();
	motor_encoder_init();
	int16_t motor_value;
	bool button_clicked;
	int16_t motor_pwm;
	pwm_drive_motor_init();
	//setup_timer_interrupt();
	int8_t K_p = 1;
	volatile int16_t cumulative_error = 0;
	int8_t K_i = 1;
	
	//PI_controller_data pi_data = {.motor_value = 0, .msg = {0x000, 0x03, {0x00, 0x00, 0x00}}, .K_p = 1, .cumulative_error = 0, .K_i = 1};

    while (1) 
    {	
		//can_tx(msg1);
		//can_tx(msg2);
		//printf("%u", TC2->TC_BMR);
		//printf("TC_BMR: %#010x\n", (uint32_t)TC2->TC_BMR);  // Print as hexadecimal (e.g., 0x00000000)

		
		if(!can_rx(&msg)){
			//printf("No");
		}
		//printf("ADC %d \n", adc_read_last());
		//can_rx(&msg);
		//can_printmsg(msg);  // Print the message
		int16_t duty_cycle = joy_x_to_duty_cycle(msg, &joy_value_prev);  // Calculate the duty cycle
		//printf("Duty Cycle: %d\n", duty_cycle);  // Display the calculated duty cycle
		pwm_set_duty_cycle(duty_cycle);
		
		button_clicked = msg.byte[2];
		//printf("Button clicked %d \n", button_clicked);
		shoot_ball(button_clicked);
		
		
		hit = register_hit();
		
		
		if(hit){
			printf("hit");
			CanMsg msg2 = {0x001,0x01,{0x1}};
			can_tx(msg2);
			CanMsg msg3 = {0x002, 0x01, {0x1}};
			can_tx(msg3);
		} else {printf("no");}
		//printf("Hit detection: %d \n", hit);
		
		motor_value = read_encoder();
		
		//motor_pwm = joy_y_to_duty_cycle(msg);
		//printf("%d \n",motor_pwm);
		//drive_motor(motor_pwm);
		
		//printf("Encoder value: %d\n", motor_value);
		
		int16_t scaled_encoder = scale_encoder_value(motor_value);
		//printf("Scaled encoder: %d\n", scaled_encoder);
		
		int16_t duty_cycle_2 = pi_controller(scaled_encoder, msg, K_p, &cumulative_error, K_i);
		//printf("Duty cycle: %d \n", duty_cycle_2);
		drive_motor(duty_cycle_2);
		
		//printf('G');
		//for (volatile uint32_t i = 0; i < 1000; i++);
		
		//uart_tx(0xF0);
		//uart_tx('B');
    }
	
}