#include "uart.h"

unsigned char buffer[BUFF_SIZE];
unsigned char sent = 1;

void usart_init() {
    // Set baud rate
    UBRR0H = (unsigned char)(MYUBRR>>8);
    UBRR0L = (unsigned char)MYUBRR;

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8-bit data */

 	 UCSR0B |= (1 << RXEN0);
// 	 UCSR0B |= (1 << UDRIE0);
 	 UCSR0B |= (1 << TXEN0);
 	 UCSR0B |= (1 << RXCIE0);
}


void usart_putchar(char data) {
//    while ( !(UCSR0A & (1 << UDRE0)) );
//    while ( sent == 0 );

    UDR0 = data;

    sent = 0;

}

char usart_getchar(void) {
    while ( !(UCSR0A & (_BV(RXC0))) );
    return UDR0;
}


void usart_putstring(char* buf){
  while(*buf){
    usart_putchar(*buf);
    ++buf;
  }
}

