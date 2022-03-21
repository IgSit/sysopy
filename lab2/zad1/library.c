#include <stdio.h>
#include "library.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdlib.h"
#include <sys/times.h>

clock_t start, end;
struct tms st_cpu, en_cpu;

void startTimer()
{
    start = times(&st_cpu);
}

void endTimer()
{
    end = times(&en_cpu);
}

int copy(char* fileFrom, char* fileTo) {
    startTimer();
    char c;
    int readFile = open(fileFrom, O_RDONLY);
    int writeFile = open(fileTo, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    int charsInLine, i;
    while (read(readFile, &c, 1) == 1) {
        charsInLine = 0, i = 0;
        char* buffer = calloc(512, sizeof(char));
        do {
            if (c != ' ' && c != '\n' && c != '\t') {
                charsInLine = 1;
            }
            buffer[i] = c;
            i++;
        } while (read(readFile, &c, 1) == 1 && c != '\n');
        if (charsInLine == 1) {
            write(writeFile, buffer, i);
        }
    }
    endTimer();
    return 0;
}


int fcopy(char* fileFrom, char* fileTo) {
    startTimer();
    char c[1];
    FILE *readFile = fopen(fileFrom, "r");
    FILE *writeFile = fopen(fileTo, "w");
    int charsInLine, i;
    while (fread(c, 1, 1, readFile)) {
        charsInLine = 0, i = 0;
        char* buffer = calloc(512, sizeof(char));
        do {
            if (c[0] != ' ' && c[0] != '\n' && c[0] != '\t') {
                charsInLine = 1;
            }
            buffer[i] = c[0];
            i++;
        } while (fread(c, 1, 1, readFile) && c[0] != '\n');
        if (charsInLine == 1) {
            fwrite(buffer, sizeof(char), i, writeFile);
        }
    }
    endTimer();
    return 0;
}