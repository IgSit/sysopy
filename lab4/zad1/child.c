#include <stdio.h>
#include <string.h>
#include <signal.h>

void writeToFile(char* text) {
    printf("%s", text);
    FILE *file = fopen("report2.txt", "a");
    fprintf(file, "%s", text);
    fclose(file);
}

void checkPending() {
    sigset_t new_mask;
    sigpending(&new_mask);
    if (sigismember(&new_mask, SIGUSR1)) {
        writeToFile("Signal pending in child process\n");
    } else {
        writeToFile("Signal not pending in child process\n");
    }
}

int main(int argc, char* argv[]) {

    if (argc < 1) {
        printf("No arguments.\n");
        return -1;
    }

    writeToFile("Child here!\n");

    if (strcmp(argv[1], "ignore") == 0){
        writeToFile("Raising signal in child process...\n");
        raise(SIGUSR1);
    }
    else if (strcmp(argv[1], "mask") == 0){
        writeToFile("Raising signal in child process...\n");
        raise(SIGUSR1);
        checkPending();
    }
    else if(strcmp(argv[1], "pending") == 0) {
        checkPending();
    }

    return 0;
}