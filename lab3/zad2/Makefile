CC = gcc
CFLAGS = -g -Wall

main:
	$(CC) $(CFLAGS) -o main main.c -lm -D COUNTTIME

clean:
	rm -rf main *.txt

test:
	make clean
	make main
	@./main 0.33