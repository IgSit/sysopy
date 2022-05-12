#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

int** image;
int** negative_image;
int w, h;
int M;
int threads_num;

void write_header(FILE* file, char* method) {
    printf("---------------------------------\n");
    printf("Number of threads:  %d\n", threads_num);
    printf("Method used:  %s\n", method);
    printf("---------------------------------\n");

    fprintf(file, "---------------------------------\n");
    fprintf(file, "Number of threads:   %d\n", threads_num);
    fprintf(file, "Method used:    %s\n", method);
    fprintf(file, "---------------------------------\n\n");
}

void load_image(char* filename){
    FILE* file = fopen(filename, "r");
    char* buffer = calloc(256, sizeof(char));

    for (int i = 0; i < 3; i++)  // skip magic number and author
        fgets(buffer, 256, file);
    sscanf(buffer, "%d %d\n", &w, &h);  // get dimensions
    fgets(buffer, 256, file);
    sscanf(buffer, "%d \n", &M);  // get color variety

    image = calloc(h, sizeof(int*));
    for (int i = 0; i < h; i++)
        image[i] = calloc(w, sizeof(int));

    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            fscanf(file, "%d", &image[i][j]);
        }
    }
    fclose(file);
}

void* numbers_method(void* arg){
    int idx = *((int *) arg);
    struct timeval stop, start;
    gettimeofday(&start, NULL);  // start mierzenia czasu

    for (int i = 0; i < h; i++){
        for (int j = idx; j < w; j+= threads_num){
                negative_image[i][j] = M - image[i][j];
        }
    }

    gettimeofday(&stop, NULL);  // koniec mierzenia czasu
    long unsigned int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    pthread_exit(&time);
}

void* block_method(void* arg) {
    int idx = *((int *) arg);
    struct timeval stop, start;
    gettimeofday(&start, NULL);  // start mierzenia czasu

    int start_x = (idx) * ceil(w / threads_num);
    int end_x = ((idx + 1)* ceil(w / threads_num) - 1);
    if (idx == threads_num - 1) end_x = w - 1;

    for (int i = 0; i < h; i++){
        for (int j = start_x; j <= end_x; j++)
            negative_image[i][j] = M - image[i][j];
    }

    gettimeofday(&stop, NULL);  // koniec mierzenia czasu

    long unsigned int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    pthread_exit(&time);
}

void save_negative(char* filename) {
    FILE *f = fopen(filename, "w");

    fprintf(f, "P2\n");
    fprintf(f, "%d %d\n", w, h);
    fprintf(f, "%d\n", M);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++)
            fprintf(f, "%d ", negative_image[i][j]);
        fprintf(f, "\n");
    }

    fclose(f);
}

pthread_t* init_threads(char* method) {
    pthread_t* threads = calloc(threads_num, sizeof(pthread_t));
    int* threads_idx = calloc(threads_num, sizeof(int));

    for(int i = 0; i < threads_num; i++){
        threads_idx[i] = i;
        if (strcmp(method, "numbers") == 0)
            pthread_create(&threads[i], NULL, &numbers_method, &threads_idx[i]);
        else  // block method
            pthread_create(&threads[i], NULL, &block_method, &threads_idx[i]);
    }
    return threads;
}

void wait_threads(pthread_t* threads, FILE* times_file) {
    long unsigned int time;
    for(int i = 0; i < threads_num; i++) {
        pthread_join(threads[i], (void **) time);
        printf("thread: %3d     time: %5lu [μs]\n", i, time);
        fprintf(times_file, "thread: %3d,     time: %5lu [μs]\n", i, time);
    }
}

void calc_total_time(struct timeval start, struct timeval stop, FILE* times_file) {
    gettimeofday(&stop, NULL);
    long unsigned int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    printf("Total time: %5lu [μs]\n", time);
    fprintf(times_file, "Total time: %5lu [μs]\n\n\n", time);
}

int main(int argc, char* argv[]){

    if (argc != 5){
        printf("Wrong number of arguments.\n");
        return -1;
    }
    threads_num = atoi(argv[1]);
    char* method = argv[2];
    char* input_file = argv[3];
    char* output_file = argv[4];
    struct timeval stop, start;

    load_image(input_file);

    negative_image = calloc(h, sizeof(int *));
    for (int i = 0; i < h; i++) negative_image[i] = calloc(w, sizeof(int));

    FILE* times_file = fopen("times.txt", "a");
    write_header(times_file, method);

    gettimeofday(&start, NULL);  // start mierzenia czasu
    pthread_t* threads = init_threads(method);
    wait_threads(threads, times_file);
    calc_total_time(start, stop, times_file); // koniec mierzenia czasu

    save_negative(output_file);
    fclose(times_file);

    return 0;
}