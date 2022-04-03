#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void writeToFile(char* text) {
    printf("%s", text);
    FILE *file = fopen("report2.txt", "a");
    fprintf(file, "%s", text);
    fclose(file);
}

void handler(int signum){
    char msg[100] = "Received signal ";
    char str[50];
    char* texts[3] = {". PID: ", ", PPID: ", "\n"};
    int values[3] = {signum, getpid(), getppid()};
    for (int i = 0; i < 3; ++i) {
        sprintf(str, "%d", values[i]);
        strcat(msg, str);
        strcat(msg, texts[i]);
    }
    writeToFile(msg);
}

void raiseSignal() {
    writeToFile("Raising a signal\n");
    raise(SIGUSR1);

    pid_t pid = fork();
    if (pid == 0){
        writeToFile("Raising a signal in child process\n");
        raise(SIGUSR1);
    }
    else {
        wait(NULL);
    }
}

sigset_t setPending() {
    sigset_t new_mask;   // signals to block
    sigset_t old_mask;   // current mask
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);

    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

    writeToFile("Raising a signal\n");
    raise(SIGUSR1);

    if (sigismember(&new_mask, SIGUSR1))
        writeToFile("Signal pending\n");
    else
        writeToFile("Signal not pending\n");
    return new_mask;
}

void checkPending(sigset_t mask) {
    sigpending(&mask);
    if (sigismember(&mask, SIGUSR1))
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
        writeToFile("\n TESTING ignore\n");
        signal(SIGUSR1, SIG_IGN);        // ignoring SIGUSR1
        raiseSignal();
    }
    else if (strcmp(argv[1], "handler") == 0){
        writeToFile("\n TESTING handle\n");
        signal(SIGUSR1, handler);        // setting custom handler
        raiseSignal();
    }
    else if (strcmp(argv[1], "mask") == 0){
        writeToFile("\n TESTING mask\n");

        sigset_t new_mask = setPending();

        pid_t pid = fork();
        if (pid == 0){
            writeToFile("Raising a signal in child process\n");
            raise(SIGUSR1);
            checkPending(new_mask);
        }
        else
            wait(NULL);
    }
    else if (strcmp(argv[1], "pending") == 0) {
        writeToFile("\n TESTING pending\n");

        sigset_t new_mask = setPending();

        pid_t pid = fork();
        if (pid == 0){
            writeToFile("Child process here\n");
            checkPending(new_mask);
        }
        else
            wait(NULL);
    }
    return 0;
}