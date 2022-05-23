#ifndef REINDEER
#define REINDEER

#include "shared.h"

void reindeer_wait() {
    pthread_mutex_lock(&mutex_reindeer_wait);
    while (waiting_reindeer != 0)
        pthread_cond_wait(&cond_reindeer_wait, &mutex_reindeer_wait);
    pthread_mutex_unlock(&mutex_reindeer_wait);
}

void reindeer_deliver(int ID) {
    pthread_mutex_lock(&mutex_reindeer);
    waiting_reindeer++;
    printf("Renifer: czeka [%d] reniferów na Mikołaja, %d\n", waiting_reindeer, ID);
    if (waiting_reindeer == REINDEER_NUM) {
        printf("Renifer: wybudzam Mikołaja, %d\n", ID);
        pthread_mutex_lock(&mutex_santa);
        pthread_cond_broadcast(&cond_santa);
        pthread_mutex_unlock(&mutex_santa);
    }
    pthread_mutex_unlock(&mutex_reindeer);
}

void* reindeer(void* arg){
    int ID = *((int *) arg);
    while(true){
        reindeer_wait();
        sleep(REINDEER_HOLIDAY_TIME);
        reindeer_deliver(ID);
    }
}

#endif