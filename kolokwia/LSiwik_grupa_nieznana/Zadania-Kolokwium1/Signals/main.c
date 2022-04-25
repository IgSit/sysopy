#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void sig_handler(int sig, siginfo_t *siginfo, void *ucontext) {
    printf("Received signal %d from %d with additional value: %d\n",
           sig, siginfo->si_pid, siginfo->si_value.sival_int);
}


int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sig_handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);

    //..........


    int child = fork();
    if(child == 0) {
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1 i SIGUSR2
        //zdefiniuj obsluge SIGUSR1 i SIGUSR2 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalami SIGUSR1 i SIGUSR2 wartosci
        sigset_t new_mask;
        sigset_t old_mask;
        sigfillset(&new_mask);
        sigdelset(&new_mask, SIGUSR1);
        sigdelset(&new_mask, SIGUSR2);
        sigprocmask(SIG_SETMASK, &new_mask, &old_mask);

        action.sa_flags = SA_SIGINFO;
        sigaction(SIGUSR1, &action, NULL);
        sigaction(SIGUSR2, &action, NULL);

        sleep(2);
    }
    else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]

        pid_t val = atoi(argv[1]);

        sleep(1);
        sigqueue(child, atoi(argv[2]), (union sigval) val);
    }

    return 0;
}
