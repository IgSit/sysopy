#ifndef sysV
#define sysV

#include <stdlib.h>
#include <pwd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define ID 'R'
#define OVEN_ID 'O'
#define TABLE_ID 'T'

#define OVEN_PATH "oven"
#define TABLE_PATH "table"

#define OVEN_TABLE_SIZE 5

#define PREPARATION_TIME 2
#define BAKING_TIME 4
#define DELIVERY_TIME 4
#define RETURN_TIME 4

#define OVEN_SEM 0
#define FULL_OVEN_SEM 1
#define TABLE_SEM 2
#define FULL_TABLE_SEM 3
#define EMPTY_TABLE_SEM 4

char* get_home_path(){
    char* path = getenv("HOME");
    if (path == NULL) path = getpwuid(getuid())->pw_dir;
    return path;
}

typedef struct{
    int pizza[OVEN_TABLE_SIZE];
    int pizza_cnt;
    int to_place_idx;
    int to_take_out_idx;
} container;


union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} arg;

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

void lock_sem(int sem_id, int sem_num){
    struct sembuf s = {.sem_num = sem_num, .sem_op = -1};
    semop(sem_id, &s, 1);
}

void unlock_sem(int sem_id, int sem_num) {
    struct sembuf s = {.sem_num = sem_num, .sem_op = 1};
    semop(sem_id, &s, 1);
}

int get_sem_id(){
    // connect to an already existing semaphore set
    key_t key = ftok(get_home_path(), ID);
    int sem_id = semget(key, 5,0);
    return sem_id;
}

int get_shm_oven_id(){
    key_t key_o = ftok(OVEN_PATH, OVEN_ID);
    int shm_oven_id = shmget(key_o, sizeof(container), 0);
    return shm_oven_id;
}

int get_shm_table_id(){
    key_t key_t = ftok(TABLE_PATH, TABLE_ID);
    int shm_table_id = shmget(key_t, sizeof(container), 0);
    return shm_table_id;
}

#endif //sysV