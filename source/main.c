
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

	printf("Inserire numerodi canali da utilizzare.\n");
	unsigned	char n_channels = getchar();
	status = write(fd, &n_channels, 1);
	getchar();

	if(status == -1){
		printf("ERRORE DURANTE L'INVIO DEL NUMERO DEI CANALI.\n");
		return -1;
	}

	printf("Inserire frequenza di campionamento di campionamento.\n");
	//aggiungere un modo per passare dalla frequenza al periodo del timer,
	//per ora invio un valore a caso

	status = write(fd, &n_channels, 1);		//Lower 	byte
	if(status == -1){
		printf("ERRORE DURANTE L'INVIO DEL PERIODO(1).\n");
		return -2;
	}

	status = write(fd, &n_channels, 1);		//Higher byte
	if(status == -1){
		printf("ERRORE DURANTE L'INVIO DEL PERIODO(2).\n");
		return -3;
	}

	printf("Inserire modalità di funzionamento (0 --> continuos; 1 --> buffered).\n");
	unsigned char sampling_mode = getchar();
	status = write(fd, &sampling_mode, 1);
	getchar();
	if(status == -1){
		printf("ERRORE DURANTE L'INVIO DELLA MODALITÀ DI FUNZIONAMENTO.\n");
		return -4;
	}

	printf("Inserire 1 per far partire l'accrocco.\n");
	unsigned char ready = getchar();
	write(fd, &ready, 1);


   unsigned short buf;
   unsigned char* ptr = (unsigned char*)&buf;

   while(1) {

		read(fd, ptr, 1);
		read(fd, ptr+1, 1);
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
