#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define FILE_NAME "test"
#define SIZE 10

/*
 * Funkcja powinna otworzyć plik nazwie 'path' w trybie tylko do zapisu
 * tak, by każdy zapis dopisywał dane na końcu pliku.
 * Jeśli plik nie istnieje, należy go stworzyć w taki sposób, by był możliwy
 * do zapisu i odczytu przez właściciela i roota, ale nie przez innych użytkowników.
 */
int open_file(const char* path) {
    int file = open(path, O_WRONLY | O_CREAT | O_APPEND, 0600);
    return file;
}

void check_fd(int fd) {
    if (fd == -1) {
        perror("Failed to open file");
        exit(-1);
    }
    char buf[1];
    int r = read(fd, buf, 1);
    if (r != -1 || errno != EBADF) {
        fprintf(stderr, "Error, can read\n");
        exit(-1);
    }
}

int main(void) {
    unlink(FILE_NAME);
    int fd1 = open_file(FILE_NAME);
    int fd2 = open_file(FILE_NAME);
    check_fd(fd1);
    check_fd(fd2);
    char buf[SIZE] = {};
    
    write(fd1, buf, SIZE);
    lseek(fd1, 0, SEEK_SET);
    write(fd1, buf, SIZE);
    off_t pos = lseek(fd1, 0, SEEK_CUR);
    if (pos != 2 * SIZE) {
        fprintf(stderr, "Error, non-appending writes are possible\n");
        exit(-1);
    }
    struct stat s;
    fstat(fd1, &s);
    if ((s.st_mode & 0777) != 0600) {
        fprintf(stderr, "Error, wrong permissions: %o\n", s.st_mode & 0777);
        exit(-1);
    }
    close(fd1);
    close(fd2);
    unlink(FILE_NAME);
    printf("OK!\n");
}
