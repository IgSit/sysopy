CC = gcc
CFLAGS = -Wall -g

compile:
	@touch oven
	@touch table
	@$(CC) $(FLAGS) cook.c -o cook
	@$(CC) $(FLAGS) delivery_man.c -o delivery_man
	@$(CC) $(FLAGS) main.c -o main

test_less: clean compile
	./main 4 4

test_more: clean compile
	./main 8 8

clean:
	@rm -f main cook delivery_man oven table