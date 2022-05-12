CC = gcc
CFLAGS = -Wall -g

compile:
	@$(CC) $(FLAGS) main.c -o main -lpthread

test: clean compile
	./main 1 numbers image.pgm negative.pgm
	./main 1 block image.pgm negative.pgm
	./main 2 numbers image.pgm negative.pgm
	./main 2 block image.pgm negative.pgm
	./main 4 numbers image.pgm negative.pgm
	./main 4 block image.pgm negative.pgm
	./main 8 numbers image.pgm negative.pgm
	./main 8 block image.pgm negative.pgm
	./main 16 numbers image.pgm negative.pgm
	./main 16 block image.pgm negative.pgm

clean:
	@rm -f main times.txt negative.pgm