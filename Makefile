CC = gcc
CFLAGS = -Wall -g

all: checkers

checkers: main.o checkers.o
	$(CC) $(CFLAGS) -o checkers main.o checkers.o

main.o: main.c checkers.h
	$(CC) $(CFLAGS) -c main.c

checkers.o: checkers.c checkers.h
	$(CC) $(CFLAGS) -c checkers.c

clean:
	rm -f *.o checkers