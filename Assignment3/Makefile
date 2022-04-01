CC=gcc
CFLAGS=-I. -g -Wall

all: ass3

ass3: ass3.c
	$(CC) $(CFLAGS) -o ass3 ass3.c
	cp -rf testcases/input/* ./
clean:
	rm -f ass3 *.o case*.in *.out