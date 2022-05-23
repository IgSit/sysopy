CC = gcc
CFLAGS = -Wall -g

compile:
	$(CC) $(FLAGS) main.c -o main -lpthread

test: clean compile
	./main

clean:
	rm -f main