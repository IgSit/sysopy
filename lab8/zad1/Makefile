CC = gcc
CFLAGS = -Wall -g

compile:
	@$(CC) $(FLAGS) main.c -o main -lpthread

test: clean compile
	./main 5 numbers image.pgm negative.pgm

clean:
	@rm -f main times.txt negative.pgm