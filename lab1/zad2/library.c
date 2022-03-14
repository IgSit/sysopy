#include <stdio.h>
#include "library.h"
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>

char** array = NULL;
unsigned int arraySize;
unsigned int firstFreeIndex = 0;
clock_t start, end;
struct tms st_cpu, en_cpu;

void startTimer()
{
    start = times(&st_cpu);
}

void endTimer()
{
    end = times(&en_cpu);
}


int createArray(unsigned int size) {
    startTimer();
    if (array != NULL)
    {
        printf("Array already initialized\n");
        return -1;
    }
    
    array = calloc(size, sizeof(char*));
    arraySize = size;
    endTimer();
    return 0;
}

int wcFiles(char* files) {
    startTimer();
    char command[1024] = "wc ";
    strcat(command, files);
    system(command);
    strcat(command, " > tmp");
    system(command);
    endTimer();
    return 0;
}

unsigned int getFileSize(FILE* file) {
    fseek(file, 0L, SEEK_END);
    unsigned int size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}

int saveTmpIntoArray() {
    startTimer();
    if (array == NULL)
    {
        printf("Array not initialized\n");
        return -1;
    }
    
    FILE *file = fopen("tmp", "rb");
    if (!file)
    {
        printf("File not opened\n");
        return -1;
    }
    unsigned int size = getFileSize(file);
    char* buffer = calloc(size, sizeof(char));
    fread(buffer, sizeof(char), size, file);
    fclose(file);
    
    if (firstFreeIndex == arraySize) // array is full
    {
        char ** tmpArray = calloc(2 * arraySize, sizeof(char*));
        for (int i = 0; i < arraySize; i++)
        {
            tmpArray[i] = array[i];
        }
        array = tmpArray;
    }
    array[firstFreeIndex] = buffer;
    firstFreeIndex++;
    remove("tmp");
    endTimer();
    return firstFreeIndex - 1;
}

int removeBlock(int index) {
    startTimer();
    if (index > arraySize - 1 || index < 0)
    {
        printf("Index out of array\n");
        return -1;
    }
    if (array[index] == NULL)
    {
        printf("Tried to remove NULL\n");
        return -1;
    }
    
    if (firstFreeIndex == 0)  //empty array
    {
        printf("Tried to remove from empty array\n");
        return -1;
    }
    if (index == firstFreeIndex - 1)  // removing last pointer
    {
        free(array[index]);
        firstFreeIndex--;
        return 0;
    }
    free(&array[index]);
    free(array[index]);
    array[index] = array[firstFreeIndex - 1];
    firstFreeIndex--;
    endTimer();
    return 0;
}
