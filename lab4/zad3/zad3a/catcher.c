#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

int caught = 0;
char* mode;

void sendBackSIGUSR1(pid_t sender_PID) {
    for (int i = 0; i < caught; i++){
        printf("Sending back SIGUSR1 (%d)\n", i + 1);
        if (strcmp(mode, "kill") == 0){
            kill(sender_PID, SIGUSR1);
        }
        else if (strcmp(mode,"queue") == 0){
            union sigval val;
            val.sival_int = i;
            sigqueue(sender_PID, SIGUSR1, val);
        }
        else if (strcmp(mode,"sigrt") == 0){
            kill(sender_PID, SIGRTMIN+1);
        }
    }
}

void finishSIGUSR2(pid_t sender_PID) {
    if (strcmp(mode,"kill") == 0){
        kill(sender_PID, SIGUSR2);
    }
    else if (strcmp(mode,"queue") == 0){
        union sigval value;
        sigqueue(sender_PID, SIGUSR2, value);
    }
    else if (strcmp(mode,"sigrt") == 0){
        kill(sender_PID, SIGRTMIN+2);
    }
}

void handler1(int signum){
    caught++;
}

void handler2(int sig, siginfo_t *info, void *ucontext){
    printf("--------------------------- ALL SIGNALS SENT -------------------------------------\n\n");
    printf("Catcher received SIGUSR2, now is going to send back [%d] SIGUSR1 signals.\n", caught);

    pid_t sender_PID = info->si_pid;
    sendBackSIGUSR1(sender_PID);
    finishSIGUSR2(sender_PID);

    exit(0);
}

sigset_t initializeMask() {
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    if(strcmp(mode, "sigrt") == 0){
        sigdelset(&mask, SIGRTMIN+1);
        sigdelset(&mask, SIGRTMIN+2);
    }
    sigprocmask(SIG_SETMASK, &mask, NULL);
    return mask;
}

void setAction() {
    struct sigaction actionSIGUSR1;
    sigemptyset(&actionSIGUSR1.sa_mask);
    actionSIGUSR1.sa_handler = handler1;
    if (strcmp(mode,"sigrt") != 0)
        sigaction(SIGUSR1, &actionSIGUSR1, NULL);
    else
        sigaction(SIGRTMIN + 1, &actionSIGUSR1, NULL);

    struct sigaction actionSIGUSR2;
    sigemptyset(&actionSIGUSR2.sa_mask);
    actionSIGUSR2.sa_flags = SA_SIGINFO;
    actionSIGUSR2.sa_sigaction  = handler2;
    if (strcmp(mode,"sigrt") != 0)
        sigaction(SIGUSR2, &actionSIGUSR2, NULL);
    else
        sigaction(SIGRTMIN + 2, &actionSIGUSR2, NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 2){
        printf("Wrong number of arguments.\n");
        return -1;
    }

    printf("\n\nCatcher PID: %d\n", getpid());
    mode = argv[1];
    printf("Mode: %s\n", mode);
    printf("Catcher waiting for SIGUSR1...\n");

    setAction();
    sigset_t mask = initializeMask();

    while(1){
        sigsuspend(&mask);
    }

}