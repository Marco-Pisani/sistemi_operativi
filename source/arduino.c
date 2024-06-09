#include "avr_common/init_functions.h"
#include "avr_common/adc.h"
#include "avr_common/uart.h"

#include <util/delay.h>

//usati per leggere dall'adc
unsigned short value = 0;
unsigned char* value_ptr = (unsigned char*)(&value);

unsigned char conversion_complete = 0;
unsigned char start_conversion = 0;

unsigned char n = 0;			//numero di byte ricevuti
unsigned char uart_buffer[BUFF_SIZE];

enum state_variable{

	W8_FOR_CHANNELS_STATE,		//waiting for number of channels
	W8_FOR_SAMPLE_RATE_STATE,	//waiting for sample rate
	W8_FOR_MODE_STATE,			//waiting for continuous sampling or buffered mode
	W8_FOR_TRIGGER_STATE,		//waiting for the trigger
	SAMPLING_STATE,				//sampling
	END_STATE

}state;

adc_settings adc;						//struct per le impostazioni dell'adc
unsigned int n_samples = 0;		//numero di campioni
unsigned int max_samples = 1000;
unsigned short* sample_buffer;	//per la modalità buffered

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

	uart_buffer[n] = UDR0;
	n++;
}

ISR(USART0_UDRE_vect){

}


int main(void){

	state = W8_FOR_CHANNELS_STATE;
	usart_init();
	DDRB = 0xF0;	//builtin led and 3 LEDs for state
	unsigned char state_leds = 0x00;
	sei();



	while(1){

		//macchina a stati
		switch(state){

			case W8_FOR_CHANNELS_STATE:
				if(n == 1){
					//impostare i canali
					adc.channels = uart_buffer[0];
					adc_init();
					state = W8_FOR_SAMPLE_RATE_STATE;
				}

				state_leds = 0x10;
				break;


			case W8_FOR_SAMPLE_RATE_STATE:
				if(n == 3){
					//impostare il timer

					adc.timer_value = uart_buffer[1] | (uart_buffer[2] << 8);
					timer1_init(adc.timer_value);

					state = W8_FOR_MODE_STATE;
				}
				state_leds = 0x20;
				break;


			case W8_FOR_MODE_STATE:

				if(n == 4){

					adc.sampling_mode = uart_buffer[3];

					if(adc.sampling_mode){
						sample_buffer = (unsigned short*)malloc(sizeof(unsigned short)*n_samples);
						adc.sample_func = buffered_sampling;
					}
					else{
						adc.sample_func = continuos_sampling;
					}

					state = W8_FOR_TRIGGER_STATE;
				}
				state_leds = 0x30;
				break;


			case W8_FOR_TRIGGER_STATE:
				if(n == 5){

					TIMSK1 |= (1 << OCIE1A);		//timer interrupt enable
					ADCSRA |= (1 << ADIE);			//ADC interrup enable

					state = SAMPLING_STATE;
				}
				state_leds = 0x40;
				break;


			case SAMPLING_STATE:

				if(conversion_complete){

					adc.sample_func();

   				ADCSRA |= (1 << ADIE);        //ADC interrup enable
   				TIMSK1 |= (1 << OCIE1A);      //timer interrupt enable

   				n_samples++;
   				conversion_complete = 0;
   				start_conversion = 0;
				}

				if(start_conversion){

   				ADCSRA |= (1 << ADSC);        //starts the conversion
   				start_conversion = 0;
				}

				//quando smettere
				if(n_samples >= max_samples){
   				TIMSK1 &= ~(1 << OCIE1A);     //timer interrupt disable
   				ADCSRA &= ~(1 << ADIE);       //ADC interrup disable
					state = END_STATE;
				}

				state_leds = 0x50;
				break;


			case END_STATE:


				if(adc.sampling_mode){
					value_ptr = (unsigned char*)sample_buffer;
					for(int i = 0; i < n_samples; value_ptr++, i++){
						usart_putchar((*value_ptr));
					}
				}

				usart_putchar( (unsigned char)0xFF);
				usart_putchar( (unsigned char)0xFF);


				state_leds = 0x70;
				state = W8_FOR_CHANNELS_STATE;
				break;

		}

		PORTB = state_leds;

	}
}
