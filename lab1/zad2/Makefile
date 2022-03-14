CC = gcc
CFLAGS = -g -Wall
LIB_NAME = library

all: static shared

static:
	$(CC) $(CFLAGS) -c $(LIB_NAME).c -o $(LIB_NAME).o
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o
	$(CC) $(CFLAGS) -o main main.c $(LIB_NAME).o

shared: _shared
	$(CC) $(CFLAGS) -o main main.c $(LIB_NAME).o

_shared:
	$(CC) $(CFLAGS) -fPIC -c $(LIB_NAME).c -o $(LIB_NAME).o
	$(CC) $(CFLAGS) -shared -o lib$(LIB_NAME).so $(LIB_NAME).o

dynamic: _shared
	$(CC) main.c -o main -L. -ldl -D dynamic

clean:
	rm -rf *.o *.a *.so main report2.txt

test1:
	@./main createArray 4 wcFiles "small1.txt small2.txt" saveTmpIntoArray removeBlock 0 

test2:
	@./main createArray 3 wcFiles "small1.txt small2.txt small3.txt small4.txt" saveTmpIntoArray \
	 removeBlock 0 wcFiles "small5.txt small4.txt" saveTmpIntoArray removeBlock 0

test3:
	@./main createArray 500 wcFiles "medium1.txt medium2.txt medium3.txt medium4.txt medium5.txt" \
	saveTmpIntoArray \
	wcFiles "medium2.txt medium3.txt medium4.txt medium5.txt medium2.txt medium3.txt medium4.txt medium5.txt" \
	saveTmpIntoArray removeBlock 1

test4:
	@./main createArray 100000 wcFiles "big1.txt big2.txt big3.txt" saveTmpIntoArray removeBlock 0 \
	wcFiles "big1.txt big2.txt big3.txt big1.txt big2.txt big3.txt" saveTmpIntoArray removeBlock 0 \
	wcFiles "big1.txt big2.txt big1.txt medium1.txt big3.txt" saveTmpIntoArray removeBlock 0 \
	wcFiles "big1.txt big2.txt small1.txt" saveTmpIntoArray removeBlock 0

