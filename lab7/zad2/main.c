#include "posix.h"

void create_semaphores(){
    sem_t* empty_oven_sem = sem_open(EMPTY_OVEN_SEM, O_CREAT, 0666, 0);
    sem_t* oven_sem = sem_open(OVEN_SEM, O_CREAT, 0666, 1);
    sem_t* table_sem = sem_open(TABLE_SEM, O_CREAT, 0666, 1);
    sem_t* full_oven_sem = sem_open(FULL_OVEN_SEM, O_CREAT, 0666, OVEN_TABLE_SIZE);
    sem_t* full_table_sem = sem_open(FULL_TABLE_SEM, O_CREAT, 0666, OVEN_TABLE_SIZE);
    sem_t* empty_table_sem = sem_open(EMPTY_TABLE_SEM, O_CREAT, 0666, 0);
}

void init_oven(container * oven){
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
    int shm_oven_fd = shm_open(OVEN_SHM, O_RDWR | O_CREAT, 0666);
    int shm_table_fd = shm_open(TABLE_SHM, O_RDWR | O_CREAT, 0666);

    ftruncate(shm_oven_fd, sizeof(container));
    ftruncate(shm_table_fd, sizeof(container));

    container* oven = mmap(NULL, sizeof(container), PROT_READ | PROT_WRITE,
                      MAP_SHARED, shm_oven_fd, 0);
    container* table = mmap(NULL, sizeof(container), PROT_READ | PROT_WRITE,
                        MAP_SHARED, shm_table_fd, 0);

    init_oven(oven);
    init_table(table);

    printf("Shared memory segment created:\noven_id: %d, table_id: %d \n\n", shm_oven_fd, shm_table_fd);

}

void handler(int signum){
    sem_unlink(OVEN_SEM);
    sem_unlink(TABLE_SEM);
    sem_unlink(FULL_OVEN_SEM);
    sem_unlink(FULL_TABLE_SEM);
    sem_unlink(EMPTY_TABLE_SEM);
    shm_unlink(TABLE_SHM);
    shm_unlink(OVEN_SHM);
}

int main(int argc, char* argv[]){
    if (argc != 3){
        printf("Wrong number of arguments\n");
        return -1;
    }

    signal(SIGINT, handler);

    create_shared_mem_segment();
    create_semaphores();

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

    for(int i = 0; i < cooks + delivery_men; i++)
        wait(NULL);

    return 0;
}

