CFLAGS = -Wall
CC = gcc

main: main.o

clean:
	rm -f main.o main

test:  main
	@echo -e "\nTESTING\n"
	@dd if=/dev/zero bs=1 count=16 2>/dev/null | sha1sum > tmpfile
	@chmod 0400 tmpfile
	./main tmpfile
	@chmod 0600 tmpfile
	@rm -f tmpfile

all: clean main test

