#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define MAX_ARGS 20
#define MAX_LINES 10
#define MAX_SIZE 256
int READ = 0;
int WRITE = 1;
int FREE = 0;
char* DICT[MAX_ARGS];

char** parse_commands(char* line){  // split sth like cmd1 = cat /etc | echo "xd" | ls
    char** commands = (char**)calloc(MAX_ARGS, sizeof(char*));
    strtok(line, "=");
    char* seq;
    int cnt = 0;
    while ((seq = strtok(NULL, "|")) != NULL) commands[cnt++] = seq;
    return commands;
}

char** parse_command_args(char* command){  // split sth like ls -l /etc/passwd
    char** args = (char**)calloc(MAX_SIZE, sizeof(char*));
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

int* get_lines_to_execute(char* line){  // decide which commands should be executed from cmd1 | cmd2 etc
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
    char *line_copy = (char *) calloc(MAX_SIZE, sizeof(char));
    strcpy(line_copy, line);
    lines[line_ctr] = line_copy;
    char *line_copy2 = (char *) calloc(MAX_SIZE, sizeof(char));
    strcpy(line_copy2, line);
    char* word = strtok(line_copy2, " ");
    DICT[FREE++] = word;
}


void manage_child_pipes(int pipe_in[2], int pipe_out[2], int i, int j, int cmd_cnt,
                  int lines_number, char** commands) {

    close(pipe_out[READ]);
    close(pipe_in[WRITE]);
    if (j == 0 && i == 0) { // first line & first command
        dup2(pipe_out[WRITE], STDOUT_FILENO);
    }
    else if (j == cmd_cnt - 1 && i == lines_number - 1) { // last line & last command
        close(pipe_out[WRITE]);
        dup2(pipe_in[READ], STDIN_FILENO);
    }
    else {
        dup2(pipe_in[READ], STDIN_FILENO);
        dup2(pipe_out[WRITE], STDOUT_FILENO);
    }

    char path[MAX_SIZE];
    char** args = parse_command_args(commands[j]);
    strcpy(path, args[0]);
    execvp(path, args);
}

void manage_parent_pipes(int pipe_in[2], int pipe_out[2]) {
    close(pipe_in[WRITE]);
    pipe_in[READ] = pipe_out[READ];
    pipe_in[WRITE] = pipe_out[WRITE];
    pipe(pipe_out);
}

void parse_execute_line(char** lines, char* line) {
    int* lines_num = get_lines_to_execute(line);
    int pipe_in[2];
    int pipe_out[2];
    pipe(pipe_out);

    int lines_number = 0;
    while(lines_num[lines_number] != -1) lines_number++;

    for (int i = 0; i < lines_number; i++) {
        char** commands = parse_commands(lines[lines_num[i]]);
        int cmd_cnt = 0;
        while (commands[cmd_cnt] != NULL) {
            printf("command: %s\n", commands[cmd_cnt]);
            cmd_cnt++;
        }

        printf("-----------OUTPUT-----------\n\n");
        for (int j = 0; j < cmd_cnt; j++) {
            pid_t pid = fork();
            if (pid == 0)
                manage_child_pipes(pipe_in, pipe_out, i, j, cmd_cnt, lines_number, commands);
            else
                manage_parent_pipes(pipe_in, pipe_out);
        }
    }
    while ((wait(0)) > 0);
}

void parse_execute_file(FILE* file){
    char** lines = (char**)calloc(MAX_LINES, sizeof(char*));
    char* line = (char*)calloc(256, sizeof(char));
    int line_ctr = 0;

    while(fgets(line, 256 * sizeof(char), file)) {
        printf("\n--------------------------------------------");
        printf("\nLINE: %s", line);

        if (strstr(line, "=")) {
            add_new_command(line, lines, line_ctr);
            line_ctr++;
        }
        else {
            parse_execute_line(lines, line);
        }
    }
}

int main(int argc, char* argv[]){
    if (argc < 1){
        printf("Wrong number of arguments!\n");
        return -1;
    }
    char* path = argv[1];
    FILE* commands = fopen(path, "r");
    parse_execute_file(commands);
    fclose(commands);
}