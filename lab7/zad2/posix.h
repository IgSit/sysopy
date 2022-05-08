#ifndef posix
#define posix

#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <stdbool.h>

#define OVEN_TABLE_SIZE 5

#define EMPTY_OVEN_SEM "/EMPTY_OVEN_S"
#define OVEN_SEM "/OVEN_S"
#define FULL_OVEN_SEM "/FULL_OVEN_S"
#define EMPTY_TABLE_SEM "/EMPTY_TABLE_S"
#define TABLE_SEM "/TABLE_S"
#define FULL_TABLE_SEM "/FULL_TABLE_S"

#define OVEN_SHM "/shm_oven"
#define TABLE_SHM "/shm_table"

#define PREPARATION_TIME 2
#define BAKING_TIME 4
#define DELIVERY_TIME 4
#define RETURN_TIME 4

typedef struct{
    int pizza[OVEN_TABLE_SIZE];
    int pizza_cnt;
    int to_place_idx;
    int to_take_out_idx;
} container;

char* get_current_time(){
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    int ml_sec = cur_time.tv_usec / 1000;

    char* buffer = calloc(50, sizeof(char));
    strftime(buffer, 50, "%H:%M:%S", localtime(&cur_time.tv_sec));

    char* current_time = calloc(50, sizeof(char));
    sprintf(current_time, "%s:%03d", buffer, ml_sec);
    return current_time;
}

void lock_sem(sem_t* sem){
    sem_wait(sem);
}

void unlock_sem(sem_t* sem) {
    sem_post(sem);
}

sem_t* get_sem(char* name){
    sem_t * sem = sem_open(name, O_RDWR);
    return sem;
}

int get_shm_oven_fd(){
    int shm_oven_fd = shm_open(OVEN_SHM, O_RDWR , 0666);
    return shm_oven_fd;
}

int get_shm_table_fd(){
    int shm_table_fd = shm_open(TABLE_SHM, O_RDWR , 0666);
    return shm_table_fd;
}

#endif //posix