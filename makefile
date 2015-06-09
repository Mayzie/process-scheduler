# Makefile for HOST Dispatcher
# Daniel Collis -- 430133523

CC=gcc
DEBUG=-DEBUG -g -Wall
RELEASE=-O3
CFLAGS=$(DEBUG)

DEPS = hostd.h rsrc.h mab.h pcb.h
OBJ = hostd.o rsrc.o mab.o pcb.o

%.o:	%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

hostd:	$(OBJ)
	$(CC) $(CFLAGS) -o hostd $^

clean:
	rm -f *.o
	rm -f hostd
