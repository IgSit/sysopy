#include "sysV.h"

void place_in_oven(int sem_id, container* oven, int type){
    lock_sem(sem_id, FULL_OVEN_SEM);
    lock_sem(sem_id, OVEN_SEM);

    oven->pizza[oven->to_place_idx] = type;
    oven->to_place_idx = (oven->to_place_idx + 1) % OVEN_TABLE_SIZE;
    oven->pizza_cnt++;

    unlock_sem(sem_id, OVEN_SEM);
}

int take_out_pizza(int sem_id, container* oven){
    lock_sem(sem_id, OVEN_SEM);

    int type = oven->pizza[oven->to_take_out_idx];
    if (type == -1) exit(-1);
    oven->pizza[oven->to_take_out_idx] = -1;
    oven->to_take_out_idx = (oven->to_take_out_idx + 1) % OVEN_TABLE_SIZE;
    oven->pizza_cnt--;

    unlock_sem(sem_id, OVEN_SEM);
    unlock_sem(sem_id, FULL_OVEN_SEM);
    return type;
}

void place_on_table(int sem_id, container* table, int type){
    lock_sem(sem_id, FULL_TABLE_SEM);
    lock_sem(sem_id, TABLE_SEM);

    table->pizza[table->to_place_idx] = type;
    table->to_place_idx = (table->to_place_idx + 1) % OVEN_TABLE_SIZE;
    table->pizza_cnt++;

    unlock_sem(sem_id, TABLE_SEM);
    unlock_sem(sem_id, EMPTY_TABLE_SEM);
}

void slave_lifecycle(int sem_id, container* oven, container* table) {
    while(true){
        int type = rand() % 10;
        printf("[C]  (pid: %d timestamp: %s)  ->   Przygotowuje pizze: %d\n",
               getpid(), get_current_time(), type);
        sleep(PREPARATION_TIME);

        place_in_oven(sem_id, oven, type);
        printf("[C]  (pid: %d timestamp: %s)  ->   Dodałem pizze: %d. Liczba pizz w piecu: %d.\n",
               getpid(), get_current_time(), type, oven->pizza_cnt);
        sleep(BAKING_TIME);

        type = take_out_pizza(sem_id, oven);
        place_on_table(sem_id, table, type);
        printf("[C]  (pid: %d timestamp: %s)  ->   Umieszczam pizze na stole: %d. Liczba pizz w piecu: %d. "
               "Liczba pizz na stole: %d.\n", getpid(), get_current_time(), type, oven->pizza_cnt, table->pizza_cnt);
    }
}

int main(){
    int sem_id = get_sem_id();
    int shm_oven_id = get_shm_oven_id();
    int shm_table_id = get_shm_table_id();

    container* oven = shmat(shm_oven_id, NULL, 0);
    container* table = shmat(shm_table_id, NULL, 0);

    srand(getpid());
    slave_lifecycle(sem_id, oven, table);

}