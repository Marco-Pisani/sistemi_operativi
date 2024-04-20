#ifdef __ATMEGA2560__
	#define IO_OFFSET ((unsigned char)0x20)
#else
	#define IO_OFFSET ((unsigned char)0x0)
#endif


//IO PORTS

#define PINA  (*(unsigned char*(IO_OFFSET + 0x0)))			//Input value register
#define DDRA  (*(unsigned char*(IO_OFFSET + 0x1)))			//Data Direction Register
#define PORTA (*(unsigned char*(IO_OFFSET + 0x2)))			//output value register

#define PINB  (*(unsigned char*(IO_OFFSET + 0x3)))
#define DDRB  (*(unsigned char*(IO_OFFSET + 0x4)))
#define PORTB (*(unsigned char*(IO_OFFSET + 0x5)))

#define PINC  (*(unsigned char*(IO_OFFSET + 0x6)))
#define DDRC  (*(unsigned char*(IO_OFFSET + 0x7)))
#define PORTC (*(unsigned char*(IO_OFFSET + 0x8)))



//ADC PORTS

#define ADMUX  (*(unsigned char* (0x7C)))
#define MUX0  0
#define MUX1  1
#define MUX2  2
#define MUX3  3
#define MUX4  4
#define ADLAR 5
#define REFS0 6
#define REFS1 7

#define ADCSRB (*(unsigned char* (0x7B)))
#define ADTS0 0
#define ADTS1 1
#define ADTS2 2
#define MUX5  3
#define ACME  6

#define ADCSRA (*(unsigned char* (0x7A)))
#define ADPS0 0
#define ADPS1 1
#define ADPS2 2
#define ADIE  3
#define ADIF  4
#define ADATE 5
#define ADSC  6
#define ADEN  7

#define ADCH   (*(unsigned char* (0x79)))
#define ADCL   (*(unsigned char* (0x78)))
#define ADC    (*(unsigned short*(0x78)))

#define DIDR0  (*(unsigned char* (0x7E)))
#define DIDR2  (*(unsigned char* (0x7D)))
