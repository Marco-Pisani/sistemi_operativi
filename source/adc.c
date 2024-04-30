#include "avr_common/uart.h"

#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned short value = 0;
unsigned char* value_ptr = (unsigned char*)(&value);

unsigned char conversion_complete = 0;
volatile unsigned char start_conversion = 0;

ISR(ADC_vect){
	*value_ptr = ADCL;
	*(value_ptr+1) = ADCH;

	conversion_complete = 1;
}


ISR(TIMER0_COMPA_vect){
	start_conversion = 1;
}


int main(void){

	printf_init();

	ADMUX &= ~((1 << REFS1)|(1 << REFS0));		//external Vref
	ADCSRA |= (1 << ADEN);			//start conversion and adc enable

	ADCSRB |= (1 << ADTS0)|(1 << ADTS1);

	TCCR0A = 0;
	TCCR0B |= (1 << CS00)|(1 << CS02);			// clk/1024
	OCR0A = 0x80;

	cli();
	TIMSK0 |= (1 << OCIE0A);						//timer interrupt enable
	ADCSRA |= (1 << ADIE);							//ADC interrup enable
	sei();

	while(1){


		if(conversion_complete){

			printf("Valore: %d\n", value);
			conversion_complete = 0;
			start_conversion = 0;
		}

		if(start_conversion){
			ADCSRA |= (1 << ADSC);
		}

	}

}
