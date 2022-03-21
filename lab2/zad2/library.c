#include <stdio.h>
#include "library.h"
#include "fcntl.h"
#include "unistd.h"
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


int count(char* findChar, char* fileFrom) {
    startTimer();
    char c;
    int readFile = open(fileFrom, O_RDONLY);
    int totalReps = 0, lineReps = 0;
    int currCharCount;  // counts repetitions of findChar in current line of file
    while (read(readFile, &c, 1) == 1) {
        currCharCount = 0;
        do {
            if (c == findChar[0]) {
                currCharCount++;
            }
        } while (read(readFile, &c, 1) == 1 && c != '\n');
        if (currCharCount > 0) {
            totalReps += currCharCount;
            lineReps++;
        }
    }
    printf("COUNT:\n");
    printf("Total: %d\n", totalReps);
    printf("Line: %d\n", lineReps);
    endTimer();
    return 0;
}


int fcount(char* findChar, char* fileFrom) {
    startTimer();
    char c[1];
    FILE *readFile = fopen(fileFrom, "r");
    int totalReps = 0, lineReps = 0;
    int currCharCount;  // counts repetitions of findChar in current line of file
    while (fread(c, 1, 1, readFile)) {
        currCharCount = 0;
        do {
            if (c[0] == findChar[0]) {
                currCharCount++;
            }
        } while (fread(c, 1, 1, readFile) && c[0] != '\n');
        if (currCharCount > 0) {
            totalReps += currCharCount;
            lineReps++;
        }
    }
    printf("FCOUNT:\n");
    printf("Total: %d\n", totalReps);
    printf("Line: %d\n", lineReps);
    endTimer();
    return 0;
}