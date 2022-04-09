#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define MAX_ARGS 20
#define MAX_LINES 10
#define READ 0
#define WRITE 1

char* DICT[MAX_LINES];
int FREE = 0;

char** parse_commands(char* line){
    char** commands = (char**)calloc(MAX_ARGS, sizeof(char*));
    strtok(line, "=");
    char* seq;
    int cnt = 0;
    while ((seq = strtok(NULL, "|")) != NULL) commands[cnt++] = seq;
    return commands;
}

char** parse_command_args(char* command){
    char** args = (char**)calloc(256, sizeof(char*));
    char* arg = strtok(command, " ");
    int cnt = 0;
    args[cnt++] = arg;
    while ((arg = strtok(NULL, " ")) != NULL) args[cnt++] = arg;
    args[cnt] = NULL;
    return args;
}

int get_line_num(char* line){
    for (int j = 0; j < FREE; ++j) {
        if (strcmp(line, DICT[j]) == 0) return j;
    }
    return -1;
}

int* get_lines_to_execute(char* line){
    char** lines = (char**)calloc(MAX_ARGS, sizeof(char*));
    char* arg = strtok(line, "|");
    int ctr = 0;
    arg[strlen(arg) - 1] = '\0';
    lines[ctr++] = arg;
    while ((arg = strtok(NULL, "|")) != NULL){
        arg[strlen(arg) - 1] = '\0';
        lines[ctr++] = arg;
    }
    static int lines_num[MAX_ARGS];
    int i = 0;
    while(lines[i] != NULL) {
        lines_num[i] = get_line_num(lines[i]);
        i++;
    }
    lines_num[i] = -1;
    return lines_num;
}

void add_new_command(char* line, char** lines, int line_ctr) {
    char *line_copy = (char *) calloc(256, sizeof(char));
    strcpy(line_copy, line);
    lines[line_ctr] = line_copy;
    char *line_copy2 = (char *) calloc(256, sizeof(char));
    strcpy(line_copy2, line);
    char* word = strtok(line_copy2, " ");
    DICT[FREE++] = word;
}


void parse_execute(FILE* file){
    char** lines = (char**)calloc(MAX_LINES, sizeof(char*));
    char* line = (char*)calloc(256, sizeof(char));
    int* lines_num;
    int line_ctr = 0;

    while(fgets(line, 256 * sizeof(char), file)) {
        printf("\n--------------------------------------------");
        printf("\nLINE: %s", line);

        if (strstr(line, "=")) {
            add_new_command(line, lines, line_ctr);
            line_ctr++;
        }
        else {
            lines_num = get_lines_to_execute(line);

            int pipe_in[2];
            int pipe_out[2];

            pipe(pipe_out);

            int lines_number = 0;
            while(lines_num[lines_number] != -1) lines_number++;

            for (int i = 0; i < lines_number; i++) {
                char** commands = parse_commands(lines[lines_num[i]]);
                int m = 0;
                while (commands[m] != NULL) {
                    printf("command: %s\n", commands[m]);
                    m++;
                }

                printf("-----------OUTPUT-----------\n\n");
                for (int j = 0; j < m; j++) {
                    pid_t pid = fork();

                    if (pid == 0) {
                        close(pipe_out[READ]);
                        close(pipe_in[WRITE]);
                        if (j == 0 && i == 0) { // first line & first command
                            dup2(pipe_out[WRITE], STDOUT_FILENO);
                        }
                        else if (j == m - 1 && i == lines_number - 1) { // last line & last command
                            close(pipe_out[WRITE]);
                            dup2(pipe_in[READ], STDIN_FILENO);
                        }
                        else {
                            dup2(pipe_in[READ], STDIN_FILENO);
                            dup2(pipe_out[WRITE], STDOUT_FILENO);
                        }

                        char path[256];
                        char** args = parse_command_args(commands[j]);
                        strcpy(path, args[0]);
                        execvp(path, args);
                    }
                    else {  // non-child
                        close(pipe_in[WRITE]);
                        pipe_in[READ] = pipe_out[READ];
                        pipe_in[WRITE] = pipe_out[WRITE];
                        pipe(pipe_out);
                    }
                }
            }
        }
        while ((wait(0)) > 0);
    }

}

int main(int argc, char* argv[]){
    if (argc < 1){
        printf("Wrong number of arguments!\n");
        return -1;
    }
    char* path = argv[1];
    FILE* commands = fopen(path, "r");
    parse_execute(commands);
    fclose(commands);
}