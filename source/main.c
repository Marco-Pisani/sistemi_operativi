
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
	unsigned int period;
	unsigned char sampling_mode;


	//NUMERO DI CANALI
	printf("Inserire numerodi canali da utilizzare.\n");
	n_channels = getchar();
	status = write(fd, &n_channels, 1);
	while(getchar() != '\n');

	if(status == -1){
		printf("ERRORE DURANTE L'INVIO DEL NUMERO DEI CANALI.\n");
		return -1;
	}


	//FREQUENZA DI CAMPIONAMENTO
	printf("Inserire frequenza di campionamento di campionamento.\n");
	//aggiungere un modo per passare dalla frequenza al periodo del timer,
	//per ora invio un valore a caso

	scanf("%uh", &period);

	status = write(fd, &n_channels, 1);		//Lower 	byte
	if(status == -1){
		printf("ERRORE DURANTE L'INVIO DEL PERIODO(1).\n");
		return -1;
	}

	status = write(fd, &n_channels, 1);		//Higher byte
	if(status == -1){
		printf("ERRORE DURANTE L'INVIO DEL PERIODO(2).\n");
		return -1;
	}
	while(getchar() != '\n');



	//MODALITÀ DI FUNZIONAMENTO
	printf("Inserire modalità di funzionamento (0 --> continuos; 1 --> buffered).\n");
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



	//TRIGGER
	printf("Inserire 1 per far partire l'accrocco.\n");
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


   while(1) {

		read(fd, ptr, 1);
		read(fd, ptr+1, 1);

     	fprintf(fp, "%x\n", buf);
     	printf("%x\n", buf);

		//può al massimo inviare 0x3F
     	if(buf > 0xF000){
			break;
	 	}

  	}

	printf("Fine.\n");
   close(fd);

	return 0;
}
