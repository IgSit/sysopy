#include "posix.h"

void place_in_oven(container* oven, int type, sem_t* oven_sem, sem_t* full_oven_sem){
    lock_sem(full_oven_sem);
    lock_sem(oven_sem);

    oven->pizza[oven->to_place_idx] = type;
    oven->to_place_idx = (oven->to_place_idx + 1) % OVEN_TABLE_SIZE;
    oven->pizza_cnt++;

    unlock_sem(oven_sem);
}

int take_out_pizza(container* oven, sem_t* oven_sem, sem_t* full_oven_sem){
    lock_sem(oven_sem);

    int type = oven->pizza[oven->to_take_out_idx];
    if (type == -1) exit(-1);
    oven->pizza[oven->to_take_out_idx] = -1;
    oven->to_take_out_idx = oven->to_take_out_idx + 1 % OVEN_TABLE_SIZE;
    oven->pizza_cnt--;

    unlock_sem(oven_sem);
    unlock_sem(full_oven_sem);
    return type;
}

void place_on_table(container* table, int type, sem_t* empty_table_sem, sem_t* table_sem,
                    sem_t* full_table_sem){
    lock_sem(full_table_sem);
    lock_sem(table_sem);

    table->pizza[table->to_place_idx] = type;
    table->to_place_idx++;
    table->to_place_idx = table->to_place_idx % OVEN_TABLE_SIZE;
    table->pizza_cnt++;

    unlock_sem(table_sem);
    unlock_sem(empty_table_sem);
}

void slave_lifecycle(container* oven, container* table,
                     sem_t* oven_sem,sem_t* full_oven_sem,
                     sem_t* empty_table_sem, sem_t* table_sem, sem_t* full_table_sem) {
    while(true){

        int type = rand() % 10;
        printf("[C]  (pid: %d timestamp: %s)  ->   Przygotowuje pizze: %d\n",
               getpid(), get_current_time(), type);
        sleep(PREPARATION_TIME);

        place_in_oven(oven, type, oven_sem, full_oven_sem);
        printf("[C]  (pid: %d timestamp: %s)  ->   Dodałem pizze: %d. Liczba pizz w piecu: %d.\n",
               getpid(), get_current_time(), type, oven->pizza_cnt);
        sleep(BAKING_TIME);

        type = take_out_pizza(oven, oven_sem, full_oven_sem);
        place_on_table(table, type, empty_table_sem, table_sem, full_table_sem);
        printf("[C]  (pid: %d timestamp: %s)  ->   Umieszczam pizze na stole: %d. Liczba pizz w piecu: %d."
               " Liczba pizz na stole: %d.\n", getpid(), get_current_time(), type, oven->pizza_cnt, table->pizza_cnt);

    }
}

int main(){
    sem_t* oven_sem = get_sem(OVEN_SEM);
    sem_t* full_oven_sem = get_sem(FULL_OVEN_SEM);
    sem_t* empty_table_sem = get_sem(EMPTY_TABLE_SEM);
    sem_t* table_sem = get_sem(TABLE_SEM);
    sem_t* full_table_sem = get_sem(FULL_TABLE_SEM);

    int shm_oven_fd = get_shm_oven_fd();
    int shm_table_fd = get_shm_table_fd();

    container* oven = mmap(NULL, sizeof(container), PROT_READ | PROT_WRITE,
                      MAP_SHARED, shm_oven_fd, 0);
    container* table = mmap(NULL, sizeof(container), PROT_READ | PROT_WRITE,
                        MAP_SHARED, shm_table_fd, 0);

    srand(getpid());
    slave_lifecycle(oven, table, oven_sem, full_oven_sem, empty_table_sem, table_sem, full_table_sem);
}