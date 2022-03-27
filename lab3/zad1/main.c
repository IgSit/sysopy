#include "stdio.h"
#include <stdlib.h>
#include "unistd.h"
#include <sys/wait.h>


int main(int argc, char* argv[]) {
    pid_t wpid;
    int status = 0;
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
    while ((wpid = wait(&status)) > 0);
    return 0;
}