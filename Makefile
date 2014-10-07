##
 # Makefile
 #  Compile and link the "Where's Parallaldo" program.
 #  
 #  usage: make all
 #  clean: make clean
 #
 #  author: Ryan Wilson-Perkin
 #  student number: 0719644
 # 
# Override with PILOTHOME=... make
PILOTHOME = /home/wgardner/work/pilot

CC = mpicc
CPPFLAGS += -I$(PILOTHOME)/include
CFLAGS = -g -O0 -Wall -std=c99
LDFLAGS += -L$(PILOTHOME)/lib -lpilot

all: wp

wp: wp.o filenames.o parallaldo.o
	$(CC) $(LDFLAGS) wp.o filenames.o parallaldo.o -o wp

wp.o: wp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c wp.c

filenames.o: filenames.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c filenames.c

parallaldo.o: parallaldo.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c parallaldo.c

clean:
	rm *.o
