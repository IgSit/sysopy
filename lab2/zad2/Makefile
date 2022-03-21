CC = gcc
CFLAGS = -g -Wall
LIB_NAME = library

static:
	$(CC) $(CFLAGS) -c $(LIB_NAME).c -o $(LIB_NAME).o
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o
	$(CC) $(CFLAGS) -o main main.c $(LIB_NAME).o

clean:
	rm -rf *.o *.a *.so main


test:
	make clean
	make static
	@./main count "q" "file.txt"

ftest:
	make clean
	make static
	@./main fcount "q" "file.txt"

cmp:
	make clean
	make static
	@./main count "q" "file.txt" fcount "q" "file.txt"