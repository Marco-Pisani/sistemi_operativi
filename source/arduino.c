#include "avr_common/init_functions.h"

#include "avr_common/uart.h"

#include <util/delay.h>

unsigned short value = 0;
unsigned char* value_ptr = (unsigned char*)(&value);

unsigned char conversion_complete = 0;
volatile unsigned char start_conversion = 0;

unsigned char start = 0;
unsigned char n = 0;


enum state_variable{

	IDLE_STATE,						//waiting for the trigger
	SAMPLING_STATE,				//sampling signals
	SENDING_STATE					//sending to pc
}state;


ISR(ADC_vect){

	ADCSRA &= ~(1 << ADIE);							//ADC interrup disable

	*value_ptr = ADCL;
	*(value_ptr+1) = ADCH;

	conversion_complete = 1;
}


ISR(TIMER1_COMPA_vect){

	TIMSK1 &= ~(1 << OCIE1A);						//timer interrupt disable
	start_conversion = 1;
}

ISR(USART0_RX_vect){

	buffer[n] = UDR0;
	n++;
}

ISR(USART0_UDRE_vect){

	sent=1;

}


int main(void){

	usart_init();

	DDRB |= 1 << 7;	//builtin led
	PORTB = 0;
	sei();



	while(1){

		if(n == 0){

			PORTB |= 1 << 7;
			_delay_ms(1000);
			PORTB &= ~(1 << 7);
			_delay_ms(1000);
		}

		if(n == 1){

			PORTB |= 1 << 7;	//led on when waken up

			cli();
			adc_init();
			timer1_init();
			sei();

			TIMSK1 |= (1 << OCIE1A);						//timer interrupt enable
			ADCSRA |= (1 << ADIE);							//ADC interrup enable

			start = 1;

			for(int i = 0; i<20; i++){
				PORTB |= 1 << 7;
				_delay_ms(100);
				PORTB &= ~(1 << 7);
				_delay_ms(100);
			}
		}

		if(n == 2){

			PORTB &= ~(1 << 7);

			if(conversion_complete){

				usart_putchar( (unsigned char)(*(value_ptr)) );
				usart_putchar( (unsigned char)(*(value_ptr+1)) );
				usart_putchar('\n');

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
}
