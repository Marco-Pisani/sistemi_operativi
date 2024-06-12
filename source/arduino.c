#include "avr_common/init_functions.h"
#include "avr_common/adc.h"
#include "avr_common/uart.h"

#include <util/delay.h>

//usati per leggere dall'adc
unsigned short value = 0x0000;
unsigned char* value_ptr = (unsigned char*)(&value);

unsigned char conversion_complete = 0;
unsigned char start_conversion = 0;

unsigned char n = 0;			//numero di byte ricevuti
unsigned char uart_buffer[BUFF_SIZE];


enum state_variable{

	W8_FOR_CHANNELS_STATE,		//waiting for number of channels
	W8_FOR_MODE_STATE,			//waiting for continuous sampling or buffered mode
	W8_FOR_SPEED_STATE,			//waiting for speed factor
	W8_FOR_SAMPLE_RATE_STATE,	//waiting for sample rate
	W8_FOR_TRIGGER_STATE,		//waiting for the trigger
	SAMPLING_STATE,				//sampling
	END_STATE
}state;

adc_settings adc;						//struct per le impostazioni dell'adc
unsigned int n_samples = 0;		//numero di campioni
unsigned int max_samples = 120;
unsigned short* sample_buffer;			//per la modalità buffered
unsigned char * sample_buffer_speed;	//per buffered a 8-bit
unsigned char current_channel = 0;
unsigned char speed = 0;				//per sapere se usare 8-bit o 10-bit

#if TEST
unsigned short* test_buffer;
unsigned char n_test = 0;

unsigned short test_variable = 0;
unsigned char* test_variable_ptr = (unsigned char*)&test_variable;
#endif



ISR(ADC_vect){

	ADCSRA &= ~(1 << ADIE);							//ADC interrup disable
	conversion_complete = 1;
}


ISR(TIMER1_COMPA_vect){

	#if !TEST
		TIMSK1 &= ~(1 << OCIE1A);						//timer interrupt disable
	#endif

	start_conversion = 1;

	#if TEST
		test_variable++;
	#endif
}

ISR(USART0_RX_vect){

	uart_buffer[n] = UDR0;
	n++;
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

				adc.timer_value 	= 0x0000;
				adc.channels	 	= 0x00;
				adc.sampling_mode = 0x0;
				adc.sample_func	= NULL;
				speed					= 0x0;

				if(n == 1){

					adc.channels = uart_buffer[0];
					adc_init();

					state = W8_FOR_SPEED_STATE;
				}

				state_leds = 0x10;
				break;


			case W8_FOR_SPEED_STATE:

				if(n == 2){

					speed = uart_buffer[1];

					state = W8_FOR_MODE_STATE;
				}
				state_leds = 0x20;
				break;




			case W8_FOR_MODE_STATE:
				if(n == 3){

					adc.sampling_mode = uart_buffer[2];

					if(adc.sampling_mode){

						if(speed){
							sample_buffer_speed = (unsigned char*)malloc(sizeof(unsigned char)*max_samples);
							adc.sample_func = buffered_sampling_speed;
						}
						else{
							sample_buffer = (unsigned short*)malloc(sizeof(unsigned short)*max_samples);
							adc.sample_func = buffered_sampling;
						}

					}
					else{

						if(speed){
							adc.sample_func = continuos_sampling_speed;
						}
						else{
							adc.sample_func = continuos_sampling;
						}
					}

					#if TEST
						test_buffer = (unsigned short*)malloc(sizeof(unsigned short)*max_samples);
					#endif

					state = W8_FOR_SAMPLE_RATE_STATE;
				}
				state_leds = 0x30;
				break;




			case W8_FOR_SAMPLE_RATE_STATE:
				if(n == 5){

					unsigned char* p = (unsigned char*)&adc.timer_value;
					*p = uart_buffer[3];
					*(p+1) = uart_buffer[4];
					timer1_init(adc.timer_value);

					state = W8_FOR_TRIGGER_STATE;
				}
				state_leds = 0x40;
				break;



			case W8_FOR_TRIGGER_STATE:
				if(n == 5){

					TIMSK1 |= (1 << OCIE1A);		//timer interrupt enable
					ADCSRA |= (1 << ADIE);			//ADC interrup enable

					conversion_complete=0;
					state = SAMPLING_STATE;
				}
				state_leds = 0x50;
				break;


			case SAMPLING_STATE:

				if(conversion_complete){

					adc.sample_func();

					//segna il numero di interrupt del timer
//					usart_putchar(0x00);
//					usart_putchar(0x00);
//					usart_putchar(0x00);
//					usart_putchar(0x00);

#if TEST
	#if BUFFERED
					test_buffer[n_samples] = test_variable;
	#else
					usart_putchar(*test_variable_ptr);
					usart_putchar(*(test_variable_ptr+1));
	#endif
#endif

					current_channel = (current_channel == adc.channels) ? 0x00 : ++current_channel;
					ADMUX = (speed << ADLAR)|(current_channel & 0x07);

   				n_samples++;
   				conversion_complete = 0;
   				start_conversion = 0;
   				ADCSRA |= (1 << ADIE);        //ADC interrup enable
   				TIMSK1 |= (1 << OCIE1A);      //timer interrupt enable
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

				state_leds = 0x60;
				break;


			case END_STATE:

				if(adc.sampling_mode){

					#if TEST
						#if BUFFERED
							test_variable_ptr = (unsigned char*)test_buffer;
						#endif
					#endif

					if(speed){

						value_ptr = (unsigned char*)sample_buffer_speed;


						for(int i = 0; i < n_samples; value_ptr+=1, i++){
							usart_putchar((*value_ptr));

							#if TEST
							#if BUFFERED
								usart_putchar(*test_variable_ptr);
								usart_putchar(*(test_variable_ptr+1));
								test_variable_ptr+=2;
							#endif
							#endif
						}
					}
					else{

						value_ptr = (unsigned char*)sample_buffer;

						for(int i = 0; i < n_samples; value_ptr+=2, i++){
							usart_putchar((*value_ptr));
							usart_putchar(*(value_ptr+1));

							#if TEST
							#if BUFFERED
								usart_putchar(*test_variable_ptr);
								usart_putchar(*(test_variable_ptr+1));
								test_variable_ptr+=2;
							#endif
							#endif
						}
					}

					free(sample_buffer);
					free(sample_buffer_speed);

					#if TEST
						#if BUFFERED
							free(test_buffer);
						#endif
					#endif
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
