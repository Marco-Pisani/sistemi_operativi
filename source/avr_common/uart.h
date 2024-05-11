#include <stdio.h>
#include <stdint.h>
#pragma once

void printf_init(void);
void usart_init(uint16_t ubrr);
char usart_getchar( void );
void usart_putchar( char data );
void usart_pstr (char *s);
unsigned char usart_kbhit(void);
int usart_putchar_printf(char var, FILE *stream);
void usart_putstring(unsigned char* buf);
unsigned char usart_getstring(unsigned char* buf);
