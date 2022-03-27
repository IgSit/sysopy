CC = gcc
CFLAGS = -g -Wall

main:
	$(CC) $(CFLAGS) -o main main.c

clean:
	rm -rf main

test:
	make clean
	make main
	@./main 4