
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pc_functions/serial_linux.h"

int main(int argc, char **argv){

	char* filename = argv[1];
	int baudrate = atoi(argv[2]);
	int fd = serial_open(filename);

   if (fd<=0) {
   	printf ("Error\n");
   	return 0;
   }

   int attribs=serial_set_interface_attribs(fd, baudrate, 0);
   if (attribs) {
   	printf("Error\n");
   	return 0;
   }

  	printf ("Success opened serial device: %s. Baudrate: %d\n", filename, baudrate);
   serial_set_blocking(fd, 1);

	ssize_t status;

	unsigned int max_samples = 200;
	unsigned char n_channels;
	float period;
	float t_min;
	unsigned char sampling_mode;

	//NUMERO DI CANALI
	printf("1)	Inserire numero di canali da utilizzare.\n");
	n_channels = getchar();
	n_channels = n_channels - '1';
	printf("inviato 0x%x\n", n_channels);

	status = write(fd, &n_channels, 1);
	while(getchar() != '\n');

	if(status == -1){
		printf("ERRORE DURANTE L'INVIO DEL NUMERO DEI CANALI.\n");
		return -1;
	}




	//MODALITÀ DI FUNZIONAMENTO
	printf("2)	Inserire modalità di funzionamento (0 --> continuos; 1 --> buffered).\n");
	sampling_mode = getchar();


	if(sampling_mode != '0' && sampling_mode != '1'){
		printf("Valore non valido.\n");
		return -1;
	}
	sampling_mode = sampling_mode - '0';
	printf("Invio: %x\n", sampling_mode);

	status = write(fd, &sampling_mode, 1);
	while(getchar() != '\n');
	if(status == -1){
		printf("ERRORE DURANTE L'INVIO DELLA MODALITÀ DI FUNZIONAMENTO.\n");
		return -1;
	}


	if(sampling_mode){
		t_min = 100.0;		//in us
	}
	else{
		t_min = 1020.0;		//in us
	}


	//FREQUENZA DI CAMPIONAMENTO
	printf("3)	Inserire periodo di campionamento di campionamento in us.\n");
	printf("Minimo: %f us, Max: 32000 us\n", t_min);

	scanf("%f", &period);

	if(period < t_min || period > 32000.0){
		printf("ERRORE VALORE NON VALIDO\n");
		return -1;
	}

	unsigned short sample_rate = (unsigned short)((period/100)*200);
	printf("Periodo calcolato: 0x%x, %u\n", sample_rate, sample_rate);
	unsigned char* p = (unsigned char*)&sample_rate;

	status = write(fd, p, 1);		//Lower 	byte
	if(status == -1){
		printf("ERRORE DURANTE L'INVIO DEL PERIODO(1).\n");
		return -1;
	}
	printf("Invio 0x%x\n", *p);
	printf("Invio 0x%x\n", *(p+1));


	status = write(fd, p+1, 1);		//Higher byte
	if(status == -1){
		printf("ERRORE DURANTE L'INVIO DEL PERIODO(2).\n");
		return -1;
	}

	while(getchar() != '\n');




	//TRIGGER
	printf("4)	Inserire 1 per far partire l'accrocco.\n");
	unsigned char ready = getchar();
	write(fd, &ready, 1);
	while(getchar() != '\n');


	FILE* fp;
	if( !(fp = fopen("output.txt", "w+")) ){
		printf("ERRORE DURANTE L'APERTURA DEL FILE DI OUTPUT.\n");
		return -1;
	}

   unsigned short buf;
   unsigned char* ptr = (unsigned char*)&buf;

	unsigned short test_variable;
	unsigned char* test_variable_ptr = (unsigned char*)&test_variable;

//		read(fd, ptr, 1);
//		read(fd, ptr+1, 1);

//	printf("Valore del timer: 0x%x, 0x%x\n", *ptr, *(ptr+1));

   while(1) {

		read(fd, ptr, 1);
		read(fd, ptr+1, 1);


		fprintf(fp, "%x\n", buf);
     	printf("%x\n", buf);


		//può al massimo inviare 0x3F
     	if(buf > 0xF000){
			break;
	 	}
//		read(fd, test_variable_ptr, 1);
//		read(fd, test_variable_ptr+1, 1);
//		read(fd, test_variable_ptr, 1);
//		read(fd, test_variable_ptr+1, 1);
//		read(fd, test_variable_ptr, 1);
//		read(fd, test_variable_ptr+1, 1);
//		printf("%u\tnumero campione\n", test_variable);

  	}

	printf("Fine.\n");
   close(fd);

	return 0;
}
