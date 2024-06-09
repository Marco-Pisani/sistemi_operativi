#include "adc.h"

void continuos_sampling(void){

  	usart_putchar( (unsigned char)(*(value_ptr)) );
  	usart_putchar( (unsigned char)(*(value_ptr+1)) );

}

void buffered_sampling(void){

	sample_buffer[n_samples] = value;

}

