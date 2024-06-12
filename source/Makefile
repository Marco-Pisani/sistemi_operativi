# AVR makefile stub

# to clean a folder
# from command line
#$> make clean

# to build the folder
#$> make 

# to upload a file
# from command line
# $> make <main_filename>.hex

#
# remember to give yourself the read/write permissions on the
# serial line
# $> sudo addgroup <your username> dialout
# reboot



# !!!!!!!!!!!!!!!! MODIFY HERE !!!!!!!!!!!!!!!!

# TEST=1 --> after each sample arduino sends another 16-bit unsigned telling how many timer interrupt
#				 have occured, used to determine the minimum sampling period

# TEST=0 --> normal operation, arduino only sends the samples

TEST=0


# BUFFERED=1 --> if TEST=1, it adapts the code to send the interrupt number when in buffered mode
#					  doesn't work with continuos
#
# BUFFERED=0 --> same as above only it assumes that arduino is in continuos mode

BUFFERED=1

# number of samples per run (total)
MAX_SAMPLES=110

# put here the file containing the main routine
# to be uploaded on the avr
# you can add multiple files, they will be all generated

BINS=arduino.elf

# put here the additional .o files you want to generate
# one .c file for each .o should be present
OBJS=avr_common/uart.o avr_common/init_functions.o avr_common/adc.o

# put here the additional header files needed for compilation
HEADERS=arv_common/uart.h avr_common/init_functions.h avr_common/variables.h avr_common/adc.h

# the file below contains the actual rules


include avr_common/avr.mk

# PC program makefile

PC_BINS=pc_functions/serial_linux.c

HEADERS=pc_functions/serial_linux.h


all: $(PC_BINS)
	gcc -o main $(PC_BINS) -DTEST=$(TEST) -DBUFFERED=$(BUFFERED) -DMAX_SAMPLES=$(MAX_SAMPLES) main.c

%.o:	pc_functions/%.c
	gcc -c $<

