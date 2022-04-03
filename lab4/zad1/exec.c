#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void writeToFile(char* text) {
    printf("%s", text);
    FILE *file = fopen("report2.txt", "a");
    fprintf(file, "%s", text);
    fclose(file);
}

void checkPending() {
    sigset_t new_mask;   // signals to block
    sigset_t old_mask;   // current mask
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);

    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

    writeToFile("Raising signal\n");
    raise(SIGUSR1);

    if (sigismember(&new_mask, SIGUSR1))
        writeToFile("Signal pending\n");
    else
        writeToFile("Signal not pending\n");

}

int main(int argc, char* argv[]){
    if (argc != 2) {
        printf("Wrong number of arguments!");
        return -1;
    }

    if (strcmp(argv[1], "ignore") == 0){
        writeToFile("\nTESTING ignore\n");
        // ignores SIGUSR1
        signal(SIGUSR1, SIG_IGN);
        writeToFile("Raising signal...\n");
        raise(SIGUSR1);
    }
    else if (strcmp(argv[1], "mask") == 0){
        writeToFile("\nTESTING mask\n");
        checkPending();
    }
    else if (strcmp(argv[1], "pending") == 0) {
        writeToFile("\nTESTING pending\n");
        checkPending();
    }
    writeToFile("Calling execl()...\n");
    execl("./child", "./child", argv[1], NULL);
    return 0;
}