#include "adc.h"

void continuos_sampling(void){

	*value_ptr   = ADCL;
	*(value_ptr+1) = ADCH;
  	usart_putchar( (unsigned char)(*(value_ptr)) );
  	usart_putchar( (unsigned char)(*(value_ptr+1)) );

}

void continuos_sampling_speed(void){

	*(value_ptr) = ADCH;
  	usart_putchar( (unsigned char)(*(value_ptr)) );
}


void buffered_sampling(void){

	*value_ptr   = ADCL;
	*(value_ptr+1) = ADCH;
	sample_buffer[n_samples] = value;
}


void buffered_sampling_speed(void){

	*(value_ptr) = ADCH;
	sample_buffer_speed[n_samples] = value;
}

