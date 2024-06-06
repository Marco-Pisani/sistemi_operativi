#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#pragma once

#define BAUD 19200
#define MYUBRR (F_CPU/16/BAUD-1)

#define BUFF_SIZE 10

extern unsigned char buffer[BUFF_SIZE];
extern unsigned char sent;


void usart_init(void);
void usart_putchar(char c);
void usart_putstring(char* buf);
char usart_getchar(void);






