#include "ADC.h"
#include "sam.h"

void adc_init(void) {
	PIOA->PIO_PDR |= PIO_PDR_P2;
	PMC->PMC_PCER1 |= (1 << 5);   //ID_ADC;

	ADC->ADC_MR = ADC_MR_FREERUN_ON | ADC_MR_PRESCAL(5); // Configure ADC mode for freerun, prescaler, and single-ended

	ADC->ADC_CHER = ADC_CHER_CH0;  // Enable channel 0 for continuous sampling
	ADC->ADC_CR = ADC_CR_START; // Start conversion (ADC will run continuously in freerun mode)
	ADC->ADC_COR &= ~ADC_COR_DIFF0; //Set channel 0 in single ended mode 
}

uint16_t adc_read_last(void) {
	return ADC->ADC_CDR[0]; //ADC->ADC_LCDR & ADC_LCDR_LDATA_Msk;
}
