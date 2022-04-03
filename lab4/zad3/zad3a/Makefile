CC = gcc
CFLAGS = -Wall -g

compile:
	@$(CC) $(FLAGS) catcher.c -o catcher
	@$(CC) $(FLAGS) sender.c -o sender

clean:
	rm -f catcher sender

test: clean compile

# example: ./catcher kill
#          ./sender $(catcherPID) 15 kill