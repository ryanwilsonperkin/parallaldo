# Override with PILOTHOME=... make
PILOTHOME = /usr/local/lib

CC = mpicc
CPPFLAGS += -I$(PILOTHOME)/include
CFLAGS = -g -O0
LDFLAGS += -L$(PILOTHOME)/lib -lpilot

all:
	$(CC) $(LDFLAGS) wp.c -o wp

clean:
	rm wp.o
