#include "avr_common/uart.h"
#include "avr_io.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>


int main(void){

	printf_init();

	ADMUX |= ((1 << 7)|(1 << 6));
	ADCSRA |= (1 << 7);				//ADC enable

	unsigned short value = 0;
	unsigned char* value_ptr = (unsigned char*)(&value);


	while(1){

		ADCSRA |= (1<<6); 			//start conversion

		_delay_ms(250);

		*value_ptr = ADCL;
		*(value_ptr+1) = ADCH;

		printf("%d\n", value);
		_delay_ms(250);

	}

}
