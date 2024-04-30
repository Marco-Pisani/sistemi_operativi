#include "avr_common/uart.h"
#include "avr_io.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>


unsigned short value = 0;
unsigned char* value_ptr = (unsigned char*)(&value);

unsigned char conversion_complete = 0;
unsigned char start_conversion = 0;

ISR(ADC_vect){

}


ISR(TIMER0_OVF_vect){

	TIMSK0 &= ~(1 << TOIE0);							//timer interrupt DISABLE

	start_conversion = 1;
}


int main(void){

	printf_init();

	ADMUX &= ~((1 << REFS1)|(1 << REFS0));		//external Vref
	ADCSRA |= (1 << ADSC)|(1 << ADEN);			//start conversion and adc enable

	sei();
	TIMSK0 |= (1 << TOIE0);							//timer interrupt enable

	TCCR0B |= (1 << CS00)|(1 << CS02);			// clk/1024


	while(1){

		ADCSRA |= (1 << ADSC);

		if(start_conversion){

			*value_ptr = ADCL;
			*(value_ptr+1) = ADCH;

			start_conversion = 0;

			printf("%d\n", value);

			TIMSK0 |= (1 << TOIE0);							//timer interrupt enable
		}

	}

}
