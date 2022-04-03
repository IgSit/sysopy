CC = gcc
CFLAGS = -g -Wall

compile_fork:
	$(CC) $(FLAGS) fork.c -o fork

compile_exec:
	$(CC) $(FLAGS) exec.c -o exec
	$(CC) $(FLAGS) child.c -o child

clean:
	rm -f fork exec child report2.txt


test_fork: clean compile_fork
	@./fork ignore
	@./fork handler
	@./fork mask
	@./fork pending


test_exec: clean compile_exec
	@./exec ignore
	@./exec mask
	@./exec pending