CC = gcc
CFLAGS = -g -Wall

compile:
	$(CC) $(FLAGS) main.c -o main

run: clean compile
	@./main

clean:
	rm -f main

test_sort: clean compile
	@./main "date"

test_send: clean compile
	@./main "sigors100@gmail.com" "Shit" "C to taki shit"