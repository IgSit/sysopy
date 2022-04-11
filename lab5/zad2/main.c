#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int MAX_SIZE = 256;

void send_email(char* address, char* topic, char* content){
    char command[MAX_SIZE];
    snprintf(command, sizeof(command), "echo %s | mail %s -s %s", content, address, topic);
    printf("%s\n", command);
    popen(command, "r");

    printf("\nEmail sent!\n");
    printf("TO:      %s\nTopic:  %s\nCONTENT: %s\n", address, topic, content);

}

void print_ordered(char* mode){
    char* command;

    if (strcmp(mode,"date") == 0) {
        command = "echo | mail -H";
    }
    else if (strcmp(mode, "sender") == 0){
        command = "echo | mail -H | sort -k 3";
    }
    else{
        printf("Wrong mode.\n");
        exit(1);
    }

    FILE* file = popen(command, "r");
    printf("\nSORTED EMAILS:\n");
    char line[MAX_SIZE];
    while(fgets(line, MAX_SIZE, file) != NULL){
        printf("%s\n", line);
    }
}

int main(int argc, char* argv[]){
    if (argc != 2 && argc != 4){
        printf("Wrong number of arguments.");
        return -1;
    }
    if (argc == 2){
        print_ordered(argv[1]);
        return 0;
    }
    char* address = argv[1];
    char* topic = argv[2];
    char* content = argv[3];
    send_email(address, topic, content);
    return 0;
}