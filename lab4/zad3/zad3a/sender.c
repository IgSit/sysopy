#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int caught = 0;
int n;
char* mode;

void handler1(int sig, siginfo_t *info, void *ucontext) {
    caught++;
    if (strcmp(mode,"queue") == 0) {
        printf("\nSender: New SIGUSR1 caught.\n");
        printf("Number of signal sent back from catcher is: %d.\n", info->si_value.sival_int);
    }
}

void handler2(int signum) {
    printf("Sender: SIGUSR2 caught.\n");
    printf("\nSender caught [%d] SIGUSR1 signals, should have caught [%d].\n", caught, n);
    exit(0);
}

void sendSIGUSR1(int catcher_PID) {
    for (int i = 0; i < n; i++) {
        printf("Sending SIGUSR1 number: %d\n", i + 1);
        if (strcmp(mode, "kill") == 0) {
            kill(catcher_PID, SIGUSR1);
        }
        else if (strcmp(mode, "queue") == 0) {
            union sigval value;
            sigqueue(catcher_PID, SIGUSR1, value);
        }
        else if (strcmp(mode, "sigrt") == 0) {
            kill(catcher_PID, SIGRTMIN + 1);
        }
    }
}

void sendSIGUSR2(int catcher_PID) {
    if (strcmp(mode, "kill") == 0) {
        kill(catcher_PID, SIGUSR2);
    }
    else if (strcmp(mode, "queue") == 0) {
        union sigval value;
        sigqueue(catcher_PID, SIGUSR2, value);
    }
    else if (strcmp(mode, "sigrt") == 0) {
        kill(catcher_PID, SIGRTMIN + 2);
    }
}

void setAction() {
    struct sigaction actionSIGUSR1;
    sigemptyset(&actionSIGUSR1.sa_mask);
    actionSIGUSR1.sa_flags = SA_SIGINFO;
    actionSIGUSR1.sa_sigaction = handler1;
    if (strcmp(mode,"sigrt") != 0)
        sigaction(SIGUSR1, &actionSIGUSR1, NULL);
    else
        sigaction(SIGRTMIN + 1, &actionSIGUSR1, NULL);

    struct sigaction actionSIGUSR2;
    sigemptyset(&actionSIGUSR2.sa_mask);
    actionSIGUSR2.sa_handler = handler2;
    if (strcmp(mode,"sigrt") != 0)
        sigaction(SIGUSR2, &actionSIGUSR2, NULL);
    else
        sigaction(SIGRTMIN + 2, &actionSIGUSR2, NULL);
}

int main(int argc, char* argv[]){
    if (argc != 4){
        printf("Wrong number of arguments!\n");
        return -1;
    }

    int catcher_PID = atoi(argv[1]);
    n = atoi(argv[2]);
    mode = argv[3];

    printf("\n\nSender PID: %d\n", getpid());
    printf("Catcher PID: %d\n", catcher_PID);
    printf("Number of signals to be sent: %d\n", n);
    printf("Mode: %s\n", mode);

    setAction();

    printf("\nSender is going to send %d SIGUSR1 signals.\n", n);
    sendSIGUSR1(catcher_PID);
    printf("Sender sent all %d SIGUSR1 signals.\n\n", n);
    sendSIGUSR2(catcher_PID);

    while(1);
}