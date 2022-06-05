CC = gcc
CFLAGS = -Wall -g

player1:
	$(CC) $(FLAGS) client.c -o client -lpthread -lm
	./client "player1" unix socket

player2:
	$(CC) $(FLAGS) client.c -o client -lpthread -lm
	./client "player2" unix socket

server:
	$(CC) $(FLAGS) server.c -o server -lpthread -lm
	./server 7001 socket

clean:
	rm -f server client socket