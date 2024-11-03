#include "PWM.h"
#include "sam.h"

// Define the PWM parameters
#define PWM_FREQUENCY 50          // 50 Hz for 20 ms period
#define PWM_PERIOD 20000          // 20 ms in microseconds
#define PWM_MIN_DUTY_CYCLE 901    // 0.9 ms for min duty cycle
#define PWM_MAX_DUTY_CYCLE 2099   // 2.1 ms for max duty cycle



void pwm_init(void) {
	// Enable PWM peripheral clock
	PMC->PMC_PCER1 |= (1 << (ID_PWM - 32));

	// Select the PWM channel and disable it for configuration
	PWM->PWM_DIS = PWM_DIS_CHID1;

	// Configure the clock for the PWM channel
	PWM->PWM_CLK = PWM_CLK_PREA(1) | PWM_CLK_DIVA(42);  // Set prescaler for desired clock frequency

	// Configure PWM channel with period and duty cycle (0% initially)
	PWM->PWM_CH_NUM[1].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;  // Use CLKA as clock source for PWM channel 0
	PWM->PWM_CH_NUM[1].PWM_CPRD = PWM_PERIOD;        // Set period for 20 ms (50 Hz)
	PWM->PWM_CH_NUM[1].PWM_CDTY = PWM_MIN_DUTY_CYCLE;  // Set initial duty cycle to 0.9 ms

	// Enable PWM channel
	PWM->PWM_ENA = PWM_ENA_CHID1;

	// Enable the output on the desired pin
	PIOB->PIO_PDR = PIO_PB13;     // Disable PIO control for PB13
	PIOB->PIO_ABSR |= PIO_PB13;   // Select peripheral B for PWM on PB13
}

void pwm_set_duty_cycle(uint16_t duty_cycle) {
	// Update the duty cycle, ensuring it's within the 0.9 to 2.1 ms range
	if (duty_cycle < PWM_MIN_DUTY_CYCLE) {
		duty_cycle = PWM_MIN_DUTY_CYCLE;
		} else if (duty_cycle > PWM_MAX_DUTY_CYCLE) {
		duty_cycle = PWM_MAX_DUTY_CYCLE;
	}

	// Set the new duty cycle
	PWM->PWM_CH_NUM[1].PWM_CDTYUPD = duty_cycle;
}

int16_t joy_x_to_duty_cycle(CanMsg msg, int16_t* prev) {
	
	int16_t joy_value = msg.byte[0];
	//printf("Joyvalue 1: \n%d", joy_value);
	joy_value += 100;  // Shift range from [-100, 100] to [0, 200]
	//printf("Joyvalue 2: \n%d", joy_value);
	if(joy_value < 0){
		joy_value = 0;
		//printf("Joyvalue 3: \n%d", joy_value);
	}

	if(abs(joy_value-(*prev)) > 6){
		*prev = joy_value;
	}
	
	return (*prev * 1200 / 200) + 900;  // Scale and shift to [900, 2100]
}
