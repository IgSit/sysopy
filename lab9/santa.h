#ifndef SANTA
#define SANTA

#include "shared.h"

void reset_queue(int* queue){
    for (int i = 0; i < MAX_WAITING_ELVES; i++) queue[i] = -1;
}

void santa_sleep() {
    pthread_mutex_lock(&mutex_santa);
    while (waiting_elves < MAX_WAITING_ELVES && waiting_reindeer < REINDEER_NUM)
        pthread_cond_wait(&cond_santa, &mutex_santa);
    pthread_mutex_unlock(&mutex_santa);
}

void santa_deliver() {
    printf("Mikołaj: Budzę się.\n");
    deliveries++;
    printf("Mikołaj: dostarczam zabawki [licznik dostaw: %d]\n", deliveries);
    sleep(DELIVERING_TIME);

    pthread_mutex_lock(&mutex_reindeer_wait);
    waiting_reindeer = 0;
    pthread_cond_broadcast(&cond_reindeer_wait);
    pthread_mutex_unlock(&mutex_reindeer_wait);
}

void santa_repair() {
    printf("Mikołaj: Budzę się.\n");
    printf("Mikołaj: rozwiązuje problemy elfów [%d][%d][%d]\n", elves_queue[0], elves_queue[1], elves_queue[2]);
    for (int i = 0; i < 3; i++)
        printf("Elf: Mikołaj rozwiązuje problem, ID: %d\n", elves_queue[i]);
    sleep(SANTA_SOLVING_TIME);

    pthread_mutex_lock(&mutex_elf_wait);
    reset_queue(elves_queue);
    waiting_elves = 0;
    pthread_cond_broadcast(&cond_elf_wait);
    pthread_mutex_unlock(&mutex_elf_wait);
}

void* santa(){
    while (true){
        santa_sleep();

        pthread_mutex_lock(&mutex_reindeer);
        if (waiting_reindeer == REINDEER_NUM)
            santa_deliver();
        pthread_mutex_unlock(&mutex_reindeer);

        if (deliveries == 3) break;

        pthread_mutex_lock(&mutex_elf);
        if (waiting_elves == MAX_WAITING_ELVES)
            santa_repair();
        pthread_mutex_unlock(&mutex_elf);

        printf("Mikołaj: zasypiam\n\n");
    }
    exit(0);
}

#endif