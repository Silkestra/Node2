#include "ADC.h"
#include "sam.h"

void adc_init(void) {
	// Enable the ADC peripheral clock
	PMC->PMC_PCER0 |= (1 << ID_ADC);

	// Configure ADC mode for freerun, prescaler, and single-ended
	ADC->ADC_MR = ADC_MR_FREERUN_ON | ADC_MR_PRESCAL(5); 

	// Enable the desired ADC channels
	ADC->ADC_CHER = ADC_CHER_CH0;  // Enable channel 0 for continuous sampling

	// Start conversion (ADC will run continuously in freerun mode)
	ADC->ADC_CR = ADC_CR_START;
	
	//Set channel 0 in single ended mode 
	ADC->ADC_COR &= ~ADC_COR_DIFF0;
}

uint16_t adc_read_last(void) {
	// Read the last converted data
	return ADC->ADC_LCDR & ADC_LCDR_LDATA_Msk;
}


//filter

