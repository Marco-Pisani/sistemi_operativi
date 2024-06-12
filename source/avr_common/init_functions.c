#include "avr_common/uart.h"

#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>


/****************

usando 10 bit il clock può al massimo essere 200Hz, in pratica si può usare solamente
il prescaler a 128, ottenendo 16000000/128 = 125 kHz. ogni conversione impiega 13 cicli,
quindi 104 us, che sarebbe il minimo periodo di campionamento con 10 bit

il periodo del clock di sistema è 0.0000000625, 62.5 ns
ci vogliono (104 us)/(62.5 ns) = 1664 (0x680) cicli per una conversione, se per il timer
si usa come clock quello da 16 MHz

valori possibili buffered
	clk/8		0x0048		pare funzioni

	con clk/8 si fa da 100us (0x00C8, 10kHz) a 0.033(0xFFFF, 30 Hz) s di periodo
	di campionamento usando 10 bit


valori continuos
	clk/8		0x0800

	con clk/8 da 1 ms (0x0800, 1 kHz) a 0.033 s (0xFFFF, 30 Hz) di periodo
****************/

void adc_init(void){

	ADMUX &= ~((1 << REFS1)|(1 << REFS0));    			//external Vref
//	ADCSRA |= (1 << ADPS2)|(1 << ADPS1);	//prescaler a 128, non serve a nulla


	ADCSRA |= (1 << ADEN);    //adc enable
	ADCSRA |= (1 << ADSC);	  //la prima conversione impiega di più
}


void timer1_init(unsigned short sample_rate){

	TCCR1A = 0;

	TCCR1B |= (1 << WGM12);			// CTC

	OCR1A = (unsigned short)sample_rate;


	TCCR1B |= (1 << CS11);        // clk/1

}
