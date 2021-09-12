CC=clang
CFLAGS=-Wall -Weverything -std=c99 -g
INCLUDE=include

SRC=$(wildcard src/*.c)
OBJ=$(patsubst %.c, %.o, $(SRC))
BUILD=build/

all: main

%.o: %.c
	$(CC) -I$(INCLUDE) $(CFLAGS) -o $@ -c $<

main:


main: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	@rm main *.o
