#include "stdio.h"
#include <stdlib.h>
#include "unistd.h"

int main(int argc, char* argv[]) {
    if (argc < 1) {  // no args
        printf("Incorrect syntax.");
        return -1;
    }
    int n = atoi(argv[1]);
    for (int i = 0; i < n; ++i) {
        if (fork() == 0) {
            printf("Text printed by process of PID: %d\n", getpid());
            exit(0);
        }
    }
    return 0;
}