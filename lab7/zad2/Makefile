CC = gcc
CFLAGS = -Wall -g

compile:
	@$(CC) $(FLAGS) cook.c -o cook -lpthread -lrt
	@$(CC) $(FLAGS) delivery_man.c -o delivery_man -lpthread -lrt
	@$(CC) $(FLAGS) main.c -o main -lpthread -lrt

test_less: clean compile
	./main 4 4

test_more: clean compile
	./main 8 8

clean:
	@rm -f main cook delivery_man