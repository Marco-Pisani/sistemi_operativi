#include "uart.h"

typedef void (*sampling_function_ptr)(void);

typedef struct settings{

	unsigned short timer_value;
	unsigned char 	channels;
	unsigned char 	sampling_mode;
	sampling_function_ptr sample_func;

}adc_settings;

extern unsigned char uart_buffer[BUFF_SIZE];

extern unsigned char conversion_complete;
extern unsigned char start_conversion;

extern adc_settings adc;
extern unsigned char * sample_buffer_speed;
extern unsigned short* sample_buffer;
extern unsigned int n_samples;
extern unsigned int max_samples;

extern unsigned short value;
extern unsigned char* value_ptr;

void continuos_sampling(void);
void buffered_sampling(void);
void continuos_sampling_speed(void);
void buffered_sampling_speed(void);
