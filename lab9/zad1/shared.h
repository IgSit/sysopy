#ifndef SHARED
#define SHARED

#include "pthread.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define REINDEER_NUM 9
#define ELVES_NUM 10
#define MAX_WAITING_ELVES 3

#define ELF_WORKING_TIME rand() % 3 + 2
#define SANTA_SOLVING_TIME rand() % 1 + 1

#define REINDEER_HOLIDAY_TIME rand() % 8 + 2
#define DELIVERING_TIME rand() % 2 + 2

pthread_mutex_t mutex_santa = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_santa =PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_reindeer = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_reindeer_wait = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_reindeer_wait   = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_elf = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_elf_wait = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_elf_wait =PTHREAD_COND_INITIALIZER;

int waiting_reindeer = 0;
int deliveries = 0;

int waiting_elves = 0;
int elves_queue[MAX_WAITING_ELVES];

#endif