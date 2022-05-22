#ifndef ELF
#define ELF

#include "shared.h"

void elf_wait(int ID) {
    pthread_mutex_lock(&mutex_elf_wait);
    while (waiting_elves == MAX_WAITING_ELVES) {
        printf("Elf: czeka na powrót elfów, ID: %d\n", ID);
        pthread_cond_wait(&cond_elf_wait, &mutex_elf_wait);
    }
    pthread_mutex_unlock(&mutex_elf_wait);
}

void elf_issue(int ID) {
    pthread_mutex_lock(&mutex_elf);
    if (waiting_elves < MAX_WAITING_ELVES) {
        elves_queue[waiting_elves] = ID;
        waiting_elves++;
        printf("Elf: czeka [%d] elfów na Mikołaja, ID: %d\n", waiting_elves, ID);

        if (waiting_elves == MAX_WAITING_ELVES){
            printf("Elf: wybudzam Mikołaja, ID: %d\n", ID);
            pthread_mutex_lock(&mutex_santa);
            pthread_cond_broadcast(&cond_santa);
            pthread_mutex_unlock(&mutex_santa);
        }
    }
    pthread_mutex_unlock(&mutex_elf);
}

void* elf(void* arg){
    int ID = *((int *) arg);
    while(true){
        sleep(ELF_WORKING_TIME);
        elf_wait(ID);
        elf_issue(ID);
    }
}

#endif