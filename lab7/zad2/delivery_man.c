#include "posix.h"

int take_pizza(container* table, sem_t* empty_table_sem, sem_t* table_sem, sem_t* full_table_sem){
    lock_sem(empty_table_sem);
    lock_sem(table_sem);

    int type = table->pizza[table->to_take_out_idx];
    if (type == -1) exit(-1);
    table->pizza[table->to_take_out_idx] = -1;
    table->to_take_out_idx = (table->to_take_out_idx + 1) % OVEN_TABLE_SIZE;
    table->pizza_cnt--;

    unlock_sem(table_sem);
    unlock_sem(full_table_sem);
    return type;
}

void slave_lifecycle(container* table, sem_t* empty_table_sem, sem_t* table_sem, sem_t* full_table_sem) {
    while(true) {
        int type = take_pizza(table, empty_table_sem, table_sem, full_table_sem);
        printf("[S]  (pid: %d timestamp: %s)  ->   Pobieram pizze: %d. Liczba pizz na stole: %d.\n",
               getpid(), get_current_time(), type, table->pizza_cnt);

        sleep(DELIVERY_TIME);
        printf("[S]  (pid: %d timestamp: %s)  ->   Dostarczam pizze: %d.\n",
               getpid(), get_current_time(), type);
        sleep(RETURN_TIME);
    }
}

int main(){
    sem_t* table_sem = get_sem(TABLE_SEM);
    sem_t* full_table_sem = get_sem(FULL_TABLE_SEM);
    sem_t* empty_table_sem = get_sem(EMPTY_TABLE_SEM);

    int shm_table_fd = get_shm_table_fd();

    container* table = mmap(NULL, sizeof(container),
                        PROT_READ | PROT_WRITE, MAP_SHARED, shm_table_fd, 0);

    slave_lifecycle(table, empty_table_sem, table_sem, full_table_sem);
}