#include "game.h"

bool register_hit(void){
	uint16_t data = adc_read_last();
	printf("%d", data);
	if(data < 200){
		return true;
	}
	
	return false;
}
