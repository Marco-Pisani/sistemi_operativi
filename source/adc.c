#include "avr_common/uart.h"
#include "avr_io.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>


unsigned short value = 0;
unsigned char* value_ptr = (unsigned char*)(&value);

unsigned char conversion_complete = 0;

ISR(ADC_vect){

	ADCSRA &= ~(1 << ADIE);							//ADC interrupt enable

	*value_ptr = ADCL;
	*(value_ptr+1) = ADCH;

	conversion_complete = 1;

}


int main(void){

	printf_init();

	ADMUX &= ~((1 << REFS1)|(1 << REFS0));		//external Vref
	ADCSRA |= (1 << ADEN);							//ADC enable

	ADCSRA |= (1 << ADIE);							//ADC interrupt enable
	sei();

	while(1){

		ADCSRA |= (1 << ADSC);

		if(conversion_complete){

			printf("%d\n", value);
			conversion_complete = 0;
			ADCSRA |= (1 << ADIE);							//ADC interrupt enable

		}

	}

}
