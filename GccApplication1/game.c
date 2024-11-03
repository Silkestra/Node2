#include "game.h"

bool register_hit(void){
	uint16_t data = adc_read_last();
	if(data < 2000){
		return true;
	}
	
	return false;
}
