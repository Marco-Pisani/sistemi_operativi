#include "avr_common/uart.h"
#include "avr_common/init_functions.h"

#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned short value = 0;
unsigned char* value_ptr = (unsigned char*)(&value);

unsigned char conversion_complete = 0;
volatile unsigned char start_conversion = 0;

unsigned char channel = 0x00;


ISR(ADC_vect){

//	ADCSRA &= ~(1 << ADIE);							//ADC interrup disable

	*value_ptr = ADCL;
	*(value_ptr+1) = ADCH;

	conversion_complete = 1;
}


ISR(TIMER1_COMPA_vect){

	TIMSK1 &= ~(1 << OCIE1A);						//timer interrupt disable
	start_conversion = 1;

}


int main(void){

	printf_init();

	unsigned char response;
	while((response = usart_getchar()) != '1');

	usart_putstring("Beginning.\n");

	adc_init();
	timer1_init();

	cli();
	TIMSK1 |= (1 << OCIE1A);						//timer interrupt enable
	ADCSRA |= (1 << ADIE);							//ADC interrup enable
	sei();


	while(1){


		if(conversion_complete){

			printf("%d\tchannel:%d\n", value, channel);

			if(channel == 0)
				channel = 0x01;
			else
				channel = 0x00;

			ADMUX = (channel & 0x1F);
			conversion_complete = 0;
			start_conversion = 0;

			ADCSRA |= (1 << ADIE);							//ADC interrup enable
			TIMSK1 |= (1 << OCIE1A);						//timer interrupt enable
		}

		if(start_conversion){

			ADCSRA |= (1 << ADSC);
			start_conversion = 0;

		}

	}

}
