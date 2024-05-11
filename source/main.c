
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
   else {
   }

   int attribs=serial_set_interface_attribs(fd, baudrate, 0);
   if (attribs) {
   	printf("Error\n");
   	return 0;
   }

  	printf ("Success opened serial device: %s. Baudrate: %d\n", filename, baudrate);

   serial_set_blocking(fd, 1);

   const int bsize=10;
   char buf[bsize];

	unsigned	char message = 'a';
			write(fd, &message, 1);

	int j = 0;
   while (j<10000) {
	   int   n_read=read(fd, buf, bsize);
      for (int i=0; i<n_read; ++i) {
         printf("%c", buf[i]);
       }
   }

   close(fd);

	return 0;
}
