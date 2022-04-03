#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define T_SIGNAL SIGUSR1

void handlerInfo(int sig, siginfo_t *info, void *ucontext) {
    printf("Signal handled, SIG %d, PID %d, PPID %d\n", sig, getpid(), getppid());
    printf("Signal number: %d\n", info->si_signo);
    printf("Signal code: %d\n", info->si_code);
    printf("Sending process ID: %d\n", info->si_pid);
    printf("User ID of sending process: %d\n", info->si_uid);
}

void handlerChild(int sig) {
    printf("Child change handler: SIG: %d PID: %d, PPID: %d\n", sig, getpid(), getppid());
}

void forkKill(pid_t child_id, int SIGNAL, int i) {
    child_id = fork();
    if (child_id == 0)
        while (1);
    else {
        char* signal;
        if (SIGNAL == 19)
            signal = "SIGSTOP";
        else
            signal = "SIGKILL";

        printf("Sending %s to child\n", signal);
        kill(child_id, SIGNAL);
        if (i == 1)
            wait(NULL);
    }
}

void scenario1() {
    puts("\n----------SA_SIGINFO----------");
    struct sigaction sig;
    sig.sa_sigaction = &handlerInfo;
    sig.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sig, NULL);

    kill(getpid(), SIGUSR1);
}

void scenario2() {
    puts("\n----------SA_NOCLDSTOP----------");
    struct sigaction sig;
    sig.sa_handler = &handlerChild;

    sigaction(SIGCHLD, &sig, NULL);

    pid_t child_id = -1;
    forkKill(child_id, SIGSTOP, 0);
    forkKill(child_id, SIGKILL, 1);

    printf("\nSA_NOCLDSTOP is set up\n\n");

    sig.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sig, NULL);

    forkKill(child_id, SIGSTOP, 0);
    forkKill(child_id, SIGKILL, 1);

    puts("\n\nHandler to SIGCHLD wasn't called when SIGSTOP was sent with SA_NOCLDSTOP flag.");
}

void scenario3() {
    puts("\n----------SA_RESETHAND----------");
    struct sigaction sig;
    sig.sa_handler = &handlerChild;

    sigaction(SIGCHLD, &sig, NULL);

    pid_t child_id = -1;
    for (int i = 0; i < 3; ++i) {
        forkKill(child_id, SIGKILL, 1);
    }
    puts("\nSA_RESETHAND is set up\n");

    sig.sa_flags = SA_RESETHAND;
    sigaction(SIGCHLD, &sig, NULL);

    for (int i = 0; i < 3; ++i) {
        forkKill(child_id, SIGKILL, 1);
    }

    printf("\n\nHandler ran only once, after first call default action (IGNORE) was restored");
}

int main(void) {
    scenario1();
    scenario2();
    scenario3();
    return 0;
}