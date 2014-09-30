# Override with PILOTHOME=... make
PILOTHOME = /home/wgardner/work/pilot

CC = mpicc
CPPFLAGS += -I$(PILOTHOME)/include
CFLAGS = -g -O0 -Wall
LDFLAGS += -L$(PILOTHOME)/lib -lpilot

all: wp

wp: wp.o filenames.o
	$(CC) $(LDFLAGS) wp.o filenames.o -o wp

wp.o: wp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c wp.c

filenames.o: filenames.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c filenames.c

clean:
	rm *.o
