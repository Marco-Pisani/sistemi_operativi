#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include "avr_common/uart.h"
#include "avr_common/init_functions.h"

unsigned short value = 0;
unsigned char* value_ptr = (unsigned char*)(&value);

unsigned char conversion_complete = 0;
volatile unsigned char start_conversion = 0;

unsigned char begin = 0;
unsigned char buffer;

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

ISR(USART0_RX_vect){

	begin++;
	buffer = UDR0;
	printf("ricevuto: %c %d\n", buffer, begin);
}



int main(void){

	printf_init();

	DDRB |= 1 << 7;	//builtin led
	PORTB = 0;

	sei();
//	while(begin == 0);
//		 sleep_mode();

//	sleep_disable();

	PORTB |= 1 << 7;	//led on when waken up

	adc_init();
	timer1_init();

	cli();
	TIMSK1 |= (1 << OCIE1A);						//timer interrupt enable
	ADCSRA |= (1 << ADIE);							//ADC interrup enable
	sei();


	while(1){


		if(conversion_complete){

//			printf("%d\tchannel:%d\n", value, channel);
			printf("%d\n", value);

//			if(channel == 0)
//				channel = 0x01;
//			else
//				channel = 0x00;

//			ADMUX = (channel & 0x1F);
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
