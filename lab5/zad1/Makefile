CC = gcc
CFLAGS = -g -Wall

compile:
	$(CC) $(FLAGS) main.c -o main

run: clean compile
	@./main

clean:
	rm -f main

test: clean compile
	@./main "commands.txt"