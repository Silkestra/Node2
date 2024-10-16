/*
 * GccApplication1.c
 *
 * Created: 16.10.2024 08:30:31
 * Author : augusal
 */ 
#include "can.h"
#include "sam.h"
#include "uart.h"

int main(void)
{
	SystemInit();
	uart_init(F_CPU,BAUDRATE);
    
	/*REG_PMC_PCER0 |= (1 << ID_PIOB);
    REG_PIOB_PER = (1 << 13);
	REG_PIOB_OER = (1 << 13);
	*/
	
	WDT->WDT_MR = WDT_MR_WDDIS;
	CanMsg msg = {0};
	
	float T_Q = 0.00000025;
	long F_OSC = 16 * 1000000;
	int BRP = (T_Q * F_OSC / 2) - 1;
	
	// Configure CAN initialization structure
	CanInit canSettings;
	canSettings.propag = 1;
	canSettings.phase1 = 1;
	canSettings.phase2 = 2;
	canSettings.sjw = 1;
	canSettings.brp = BRP;    // Set BRP as calculated earlier
	canSettings.smp = 1;      // Use sample mode

	// Initialize CAN with receive interrupts enabled
	can_init(canSettings, 0); // Disable receive interrupts
	
    while (1) 
    {	
		if (can_rx(&msg)) {
			printf("Message received!\n");
			can_printmsg(msg); // Then print the actual message content
			} else {
			printf("No.\n");
		}
		//printf('G');
		//for (volatile uint32_t i = 0; i < 1000; i++);
		
		//uart_tx(0xF0);
		//uart_tx('B');
		//printf("A");
		//for (volatile uint32_t i = 0; i < 10000; i++);
		/*
		REG_PIOB_SODR = (1 << 13);
		for (volatile uint32_t i = 0; i < 100000; i++); 
		REG_PIOB_CODR = (1 << 13); 	
		for (volatile uint32_t i = 0; i < 100000; i++); 
	*/
    }
	
}
