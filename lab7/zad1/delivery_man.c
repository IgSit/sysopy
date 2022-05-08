#include "sysV.h"

int take_pizza(int sem_id, container* table){
    lock_sem(sem_id, EMPTY_TABLE_SEM);
    lock_sem(sem_id, TABLE_SEM);

    int type = table->pizza[table->to_take_out_idx];
    if (type == -1) exit(-1);
    table->pizza[table->to_take_out_idx] = -1;
    table->to_take_out_idx = (table->to_take_out_idx + 1) % OVEN_TABLE_SIZE;
    table->pizza_cnt--;

    unlock_sem(sem_id, TABLE_SEM);
    unlock_sem(sem_id, FULL_TABLE_SEM);

    return type;
}

void slave_lifecycle(int sem_id, container* table) {
    while(true) {
        int type = take_pizza(sem_id, table);
        printf("[S]  (pid: %d)  ->   Pobieram pizze: %d. Liczba pizz na stole: %d.\n",
               getpid(), type, table->pizza_cnt);

        sleep(DELIVERY_TIME);
        printf("[S]  (pid: %d)  ->   Dostarczam pizze: %d.\n", getpid(), type);
        sleep(RETURN_TIME);
    }
}

int main(){
    int sem_id = get_sem_id();
    int shm_table_id = get_shm_table_id();
    container* table = shmat(shm_table_id, NULL, 0);

    slave_lifecycle(sem_id, table);
}