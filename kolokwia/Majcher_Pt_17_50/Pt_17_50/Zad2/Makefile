CFLAGS = -Wall
CC = gcc

main: main.o

clean:
	rm -f main.o main

test:  main
	@echo -e "\n\nTESTING\n\n"
	./main 1
	./main 3

all: main test
