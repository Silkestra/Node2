#include "motor_driver.h"
#define PWM_PERIOD_MOTOR 20

void solenoid_init(){
	PMC->PMC_PCER0 |= (1 << ID_PIOC);
	PIOC->PIO_OER |= (1 << 28); // Enable output
	PIOC->PIO_PER |= (1 << 28); // Enable kontroll av pin 28
	
	PIOC->PIO_SODR |= (1 << 28); // Initialverdi høy
}

void motor_encoder_init(){
	//PMC->PMC_PCER0 = (1 << ID_TC2);
	//PMC->PMC_PCER1 = (1 << ID_TC6 - 32);
	//PMC->PMC_PCER0 |= (1 << ID_PIOC);
	PMC->PMC_PCER1 |= (1 << (ID_TC6 - 32));
	
	PIOC->PIO_OER |= PIO_PC25 | PIO_PC26;
	PIOC->PIO_PDR |= PIO_PC25 | PIO_PC26;
	PIOC->PIO_ABSR |= PIO_PC25 | PIO_PC26;
	
	//TC2->TC_WPMR = 0x54494D << TC_WPMR_WPKEY_Pos;
	//TC2->TC_WPMR &= ~(TC_WPMR_WPEN);
	
	/*
	TC2->TC_BMR = TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA; // Enable quadrature decoder, position enabled og edge phase på både A og B
	TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0 | TC_CMR_ETRGEDG_NONE; // Klokke 0, clock blir ikke gated av eksternt signal
	TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG; // Software trigger
	*/
	
	//TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	TC2->TC_BMR |= TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA;
	TC2->TC_CHANNEL[0].TC_CMR |= TC_CMR_TCCLKS_XC0;
	TC2->TC_CHANNEL[0].TC_CCR |= TC_CCR_SWTRG | TC_CCR_CLKEN;
}

uint32_t read_encoder(){
	return TC2->TC_CHANNEL[0].TC_CV;
}

void shoot_ball(bool button_clicked){
	if(button_clicked){
		PIOC->PIO_CODR |= (1 << 28); // Setter lav
	} else {
		PIOC->PIO_SODR |= (1 << 28); // Setter høy
	}
}

void pwm_drive_motor_init(){
	PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;  // Use CLKA as clock source for PWM channel 0
	PWM->PWM_CH_NUM[0].PWM_CPRD = PWM_PERIOD_MOTOR;        // Set period for 20 ms (50 Hz)
  // Set initial duty cycle to 0.9 ms

	// Enable PWM channel
	PWM->PWM_ENA = PWM_ENA_CHID0;

	// Enable the output on the desired pin
	PIOB->PIO_PDR = PIO_PB12;     // Disable PIO control for PB13
	PIOB->PIO_ABSR |= PIO_PB12;   // Select peripheral B for PWM on PB13
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



