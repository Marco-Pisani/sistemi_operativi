#include "avr_common/uart.h"

#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void adc_init(void){

	ADMUX &= ~((1 << REFS1)|(1 << REFS0));    //external Vref
	ADCSRA |= (1 << ADEN);                    //adc enable
}


void timer1_init(void){

	TCCR1A = 0;
	TCCR1B |= (1 << CS10);        // clk/1024
	OCR1AL = 0xFF;
	OCR1AH = 0x00;
}
