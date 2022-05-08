#include "sysV.h"

int shm_oven_id, shm_table_id, sem_id;

void init_oven(container* oven){
    for (int i = 0; i < OVEN_TABLE_SIZE; i++) oven->pizza[i] = -1;
    oven->pizza_cnt = 0;
    oven->to_place_idx = 0;
    oven->to_take_out_idx = 0;
}

void init_table(container* table){
    for (int i = 0; i < OVEN_TABLE_SIZE; i++) table->pizza[i] = -1;
    table->pizza_cnt = 0;
    table->to_place_idx = 0;
    table->to_take_out_idx = 0;
}

void create_shared_mem_segment(){
    key_t key_oven, key_table;

    key_oven = ftok(OVEN_PATH, OVEN_ID);
    key_table = ftok(TABLE_PATH, TABLE_ID);

    shm_oven_id = shmget(key_oven, sizeof(container), IPC_CREAT | 0666);
    shm_table_id = shmget(key_table, sizeof(container), IPC_CREAT | 0666);

    container* oven = shmat(shm_oven_id, NULL, 0);
    container* table = shmat(shm_table_id, NULL, 0);

    init_oven(oven);
    init_table(table);

    printf("Shared memory segment created:\noven_id: %d, table_id: %d \n\n", shm_oven_id, shm_table_id);
}

void create_sem_set(){
    key_t key = ftok(get_home_path(), ID);
    sem_id = semget(key, 5, 0666 | IPC_CREAT);

    union arg;
    arg.val = 1;

    semctl(sem_id, OVEN_SEM, SETVAL, arg);

    semctl(sem_id, TABLE_SEM, SETVAL, arg);

    // if oven is full, block cook process - cannot place pizza
    arg.val = OVEN_TABLE_SIZE;
    semctl(sem_id, FULL_OVEN_SEM, SETVAL, arg);

    // if table is full, block cook process - cannot place pizza
    arg.val = OVEN_TABLE_SIZE;
    semctl(sem_id, FULL_TABLE_SEM, SETVAL, arg);

    // if table is empty, block delivery_man process - cannot take out pizza
    arg.val = 0;
    semctl(sem_id, EMPTY_TABLE_SEM, SETVAL, arg);

    printf("Semaphore set created\nsem_id: %d\n\n", sem_id);
}

void handler(int signum){
    semctl(sem_id, 0, IPC_RMID, NULL);
    shmctl(shm_oven_id, IPC_RMID, NULL);
    shmctl(shm_table_id, IPC_RMID, NULL);
}

int main(int argc, char* argv[]){
    if (argc != 3){
        printf("Wrong number of arguments\n");
        return -1;
    }

    create_shared_mem_segment();
    create_sem_set();

    signal(SIGINT, handler);

    int cooks = atoi(argv[1]);
    int delivery_men = atoi(argv[2]);

    for (int i = 0; i < cooks; i++){
        pid_t pid = fork();
        if (pid == 0) execl("./cook", "./cook", NULL);
    }

    for (int i = 0; i < delivery_men; i++){
        pid_t pid = fork();
        if (pid == 0) execl("./delivery_man", "./delivery_man", NULL);
    }

    for(int i = 0; i < cooks + delivery_men; i++) wait(NULL);

    return 0;
}