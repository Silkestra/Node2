#include "motor_driver.h"
#define PWM_PERIOD_MOTOR 20

void solenoid_init(){
	PMC->PMC_PCER0 |= (1 << ID_PIOC);
	PIOC->PIO_OER |= (1 << 28); // Enable output
	PIOC->PIO_PER |= (1 << 28); // Enable kontroll av pin 28
	
	PIOC->PIO_SODR |= (1 << 28); // Initialverdi høy
}

void motor_encoder_init(){
	PMC->PMC_PCER1 |= (1 << (ID_TC6 - 32));
	
	PIOC->PIO_OER |= PIO_PC25 | PIO_PC26;
	PIOC->PIO_PDR |= PIO_PC25 | PIO_PC26;
	PIOC->PIO_ABSR |= PIO_PC25 | PIO_PC26;

	TC2->TC_BMR |= TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA;
	TC2->TC_CHANNEL[0].TC_CMR |= TC_CMR_TCCLKS_XC0;
	TC2->TC_CHANNEL[0].TC_CCR |= TC_CCR_SWTRG | TC_CCR_CLKEN;
}

int16_t read_encoder(){
	return TC2->TC_CHANNEL[0].TC_CV;
}

/*void setup_timer_interrupt(){
	PMC->PMC_PCER1 |= (1 << (ID_TC7 - 32)); // Enable TC2, channel 1
	TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC; // Enable timer, waveform and rc register
	TC2->TC_CHANNEL[1].TC_RC = 26250; // 84 MHz / 32 (for TIMER_CLOCK3) / 100 Hz leads to a 10 ms interrupt
	TC2->TC_CHANNEL[1].TC_IER = TC_IER_CPCS; // Interrupt Enable, RC Compare enabled
	NVIC_EnableIRQ(TC7_IRQn); // Enable the interrupt in the NVIC for TC2 channel 1
	
	TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG; // Start counter clock and software trigger
}*/

/*void timer_handler(){
	TC2->TC_CHANNEL[1].TC_SR; // Clearing the interrupt flag by reading status register
	
	int16_t duty_cycle_2 = pi_controller(pi_data.scaled_motor_value, pi_data.msg, pi_data.K_p, &pi_data.cumulative_error, pi_data.K_i);
	
	drive_motor(duty_cycle_2);
}*/

void shoot_ball(bool button_clicked){
	if(button_clicked){
		PIOC->PIO_CODR |= (1 << 28); // Setter lav
	} else {
		PIOC->PIO_SODR |= (1 << 28); // Setter høy
	}
}

void pwm_drive_motor_init(){
	PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;  // Use CLKA as clock source for PWM channel 0
	PWM->PWM_CH_NUM[0].PWM_CPRD = PWM_PERIOD_MOTOR; // Set period for 20 ms (50 Hz)
  // Set initial duty cycle to 0.9 ms

	PWM->PWM_ENA = PWM_ENA_CHID0; // Enable PWM channel

	PIOB->PIO_PDR = PIO_PB12; // Disable PIO control for PB13
	PIOB->PIO_ABSR |= PIO_PB12; // Select peripheral B for PWM on PB13
	PIOC->PIO_OER |= (1 << 23); // Enable output
	PIOC->PIO_PER |= (1 << 23); // Enable kontroll av pin 28
		
	PIOC->PIO_SODR |= (1 << 23);
}

void drive_motor(int16_t duty_cycle){
	if(duty_cycle>0){
		PWM->PWM_CH_NUM[0].PWM_CDTYUPD = abs(duty_cycle);
		PIOC->PIO_CODR |= (1 << 23);
	}
	else {
		PWM->PWM_CH_NUM[0].PWM_CDTYUPD = abs(duty_cycle);
		PIOC->PIO_SODR |= (1 << 23);
	}
}

int16_t joy_y_to_duty_cycle(CanMsg msg) {
	int16_t joy_value = (int8_t)msg.byte[1];
	return (joy_value * 20 / 100);
}

int16_t scale_encoder_value(int16_t motor_value) {
	return ((motor_value * 200) / 5616) - 100;
}

int16_t pi_controller(int16_t motor_value, CanMsg msg, int8_t K_p, int16_t* cumulative_error, int8_t K_i){
	int8_t joy_value = (int8_t)msg.byte[1];
	int16_t error = joy_value - motor_value;
	*cumulative_error += error;
	
	if(abs(*cumulative_error) > 4000){
		*cumulative_error = 1000; // Prøver å fikse integrator windup
	}
	
	float duty_cycle = K_p * error * 0.2 + K_i * (*cumulative_error) * 0.003 ;
	
	printf("Cumsum : %d\n", *cumulative_error);
	
	return (int16_t)duty_cycle;
}