#include "posix.h"

int client_id;
int server_queue;
int client_queue;

void handle_delete_queue(){
    delete_queue(client_queue);
    exit(0);
}

void parse_msg_text(message* msg){
    printf("Enter msg:\n");
    char* msg_text = calloc(MAX_LEN, sizeof(char));
    fgets(msg_text, MAX_LEN, stdin);
    strcpy(msg->text, msg_text);
    free(msg_text);
}

void sent_TO_ALL(){
    printf("INFO: sending message TO_ALL\n");

    time_t now = time(NULL);
    message msg = {.sender_id = client_id, .type = TO_ALL, .time=*localtime(&now)};

    parse_msg_text(&msg);

    send_msg(server_queue, &msg);
    printf("INFO: message sent TO_ALL\n");
}

void sent_TO_ONE(){
    printf("INFO: sending message TO_ONE\n");

    time_t now = time(NULL);
    message msg = {.sender_id = client_id, .type = TO_ONE, .time=*localtime(&now)};

    printf("Enter client queue_id:\n");
    char* read_q_id = calloc(MAX_LEN, sizeof(char));
    fgets(read_q_id, MAX_LEN, stdin);
    msg.queue_id = atoi(read_q_id);
    free(read_q_id);

    parse_msg_text(&msg);

    send_msg(server_queue, &msg);
    printf("INFO: message sent TO_ONE\n");
}

void send_LIST(){
    printf("INFO: sending message LIST\n");
    time_t curr_time = time(NULL);
    message msg = {.type = LIST, .time=*localtime(&curr_time)};
    send_msg(server_queue, &msg);
}

void send_STOP(){
    printf("INFO: sending message STOP\n");
    time_t curr_time = time(NULL);
    message msg = {.sender_id = client_id, .type = STOP, .time=*localtime(&curr_time)};
    send_msg(server_queue, &msg);
    handle_delete_queue();
}

void initialize(){
    time_t curr_time = time(NULL);
    message msg = {.queue_id = client_queue, .type = INIT, .time=*localtime(&curr_time)};
    send_msg(server_queue, &msg);

    message received_msg;
    receive_msg(client_queue, &received_msg, INIT);

    client_id = received_msg.sender_id;
    printf("INFO: Client id: %d\n", received_msg.sender_id);
}

bool is_queue_empty(int q){
    struct msqid_ds buf;
    msgctl(q, IPC_STAT, &buf);

    if (buf.msg_qnum != 0) return false;
    return true;
}

void catcher(){
    while (!is_queue_empty(client_queue)){
        message msg;
        receive_msg(client_queue, &msg, 0);

        char buff[MAX_LEN];
        switch (msg.type)
        {
            case STOP:
                printf("INFO: STOP received \n");
                time_t now = time(NULL);
                message msg_back = {.type = STOP, .time=*localtime(&now)};
                send_msg(server_queue, &msg_back);
                handle_delete_queue();
            case TO_ALL:
                printf("INFO: new message received\n");
                strftime(buff, MAX_LEN, "%Y-%m-%d %H:%M:%S", &msg.time);
                printf("FROM: %d\nTIME: %s\nTEXT: %s\n", msg.sender_id, buff, msg.text);
            case TO_ONE:
                printf("INFO: new message received\n");
                strftime(buff, MAX_LEN, "%Y-%m-%d %H:%M:%S", &msg.time);
                printf("FROM: %d\nTIME: %s\nTEXT: %s\n", msg.sender_id, buff, msg.text);
        }
    }
}

bool is_input_available(){
    fd_set set;
    struct timeval time;

    time.tv_sec = 1;
    time.tv_usec = 0;

    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    select(STDIN_FILENO + 1, &set, NULL, NULL, &time);

    return (FD_ISSET(0, &set));
}


int main(){
    printf("---- CLIENT ----\n");

    pid_t pid = getpid();
    signal(SIGINT, send_STOP);
    atexit(handle_delete_queue);

    key_t key_s = ftok(get_home_path(), SERVER_ID);

    server_queue = msgget(key_s, 0);

    printf("INFO: Server queue id: %d\n", server_queue);

    key_t key_c = ftok(get_home_path(), pid);

    client_queue = msgget(key_c, IPC_CREAT | IPC_EXCL | 0666);

    printf("INFO: Client queue id: %d\n", client_queue);


    initialize();
    char buffer[MAX_LEN];
    while(true){
        if (is_input_available()) {
            fgets(buffer, MAX_LEN, stdin);
            if (strcmp("LIST\n", buffer) == 0)
                send_LIST();
            else if (strcmp("STOP\n", buffer) == 0)
                send_STOP();
            else if (strcmp("TO_ONE\n", buffer) == 0)
                sent_TO_ONE();
            else if (strcmp("TO_ALL\n", buffer) == 0)
                sent_TO_ALL();
        }
        catcher();
    }

}