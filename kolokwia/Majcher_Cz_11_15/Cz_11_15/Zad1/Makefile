CFLAGS = -Wall
CC = gcc

main: zad1.o

clean:
	rm -f ./zad1.o ./zad1

test:  zad1
	@echo -e "\n\nTESTING\n\n"
	./zad1 zad1.c katalog slink

all: clean zad1 test
