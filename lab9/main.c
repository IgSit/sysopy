#include "santa.h"
#include "elf.h"
#include "reindeer.h"

pthread_t* init_threads(int amount, void *routine) {
    pthread_t* threads = calloc(amount, sizeof(pthread_t));
    for (int i = 0; i < amount; i++)
        pthread_create(&threads[i], NULL, routine, &i);
    return threads;
}

void join_threads(pthread_t santa_t, pthread_t* reindeer_t, pthread_t* elves_t) {
    pthread_join(santa_t, NULL);
    for (int i = 0; i < REINDEER_NUM; i++) pthread_join(reindeer_t[i], NULL);
    for (int i = 0; i < ELVES_NUM; i++) pthread_join(elves_t[i], NULL);
}

int main(){
    reset_queue(elves_queue);

    pthread_t santa_t;
    pthread_create(&santa_t, NULL, &santa, NULL);

    pthread_t* reindeer_t = init_threads(REINDEER_NUM, reindeer);
    pthread_t* elves_t = init_threads(ELVES_NUM, elf);

    join_threads(santa_t, reindeer_t, elves_t);
    return 0;
}