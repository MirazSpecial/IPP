CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

all: forests

forests: bst.o work.o checkinput.o main.o
	$(CC) $(CFLAGS) -o $@ $^

.c.o:
	$(CC) $(FLAGS) -I inc -c $<

clean:
	rm -rf forests *.o

.PHONY: all clean