#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 256

void print_output(FILE* file){
    char line[MAX_SIZE];
    while(fgets(line, MAX_SIZE, file) != NULL){
        printf("%s\n", line);
    }
}

void print_ordered_by(char* mode){
    FILE* file;
    char* command;

    if (strcmp(mode,"date") == 0) {
        command = "echo | mail -f | tac";
    }
    else if (strcmp(mode, "sender") == 0){
        command = "echo | mail -f | sort -k 2";
    }
    else{
        printf("Wrong mode! Choose: [date] or [sender].\n");
        exit(1);
    }

    file = popen(command, "r");

    if (file == NULL){
        printf("popen() error!");
        exit(1);
    }

    printf("\nSORTED EMAILS: ");
    print_output(file);
}

void send_email(char* address, char* object, char* content){
    FILE* file;
    char command[MAX_SIZE];

    snprintf(command, sizeof(command), "echo %s | mail -s %s %s", content, object, address);
    printf("%s\n", command);
    file = popen(command, "r");

    if (file == NULL){
        printf("popen() error!");
        exit(1);
    }

    printf("\n----- Email sent! -----\n");
    printf("TO:      %s\nOBJECT:  %s\nCONTENT: %s\n", address, object, content);

}

int main(int argc, char* argv[]){
    if (argc != 2 && argc != 4){
        printf("Wrong number of arguments!");
        exit(1);
    }
    if (argc == 2){
        printf("Print emails ordered by %s.\n", argv[1]);
        print_ordered_by(argv[1]);
    }
    else {
        printf("Send email.\n");
        char* address = argv[1];
        char* subject = argv[2];
        char* content = argv[3];

        send_email(address, subject, content);
    }

}