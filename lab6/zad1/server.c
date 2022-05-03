#include "posix.h"

FILE* server_logs;
client clients[MAX_CLIENTS];
int server_queue;

void add_log(message* msg){
    char buffer[MAX_LEN];
    strftime(buffer, MAX_LEN, "time: %Y-%m-%d %H:%M:%S", &msg->time);

    char log_msg[2 * MAX_LEN];
    sprintf(log_msg, "%s; sender_id: %d; msg_type: %ld\n", buffer, msg->sender_id, msg->type);

    fputs(log_msg, server_logs);
}

void list_clients(message* msg){
    add_log(msg);
    printf("INFO: LIST received\n");
    printf("CLIENTS:\n");
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].is_running)
            printf("\tid: %d   queue_id: %d\n", clients[i].id, clients[i].queue_id);
    }
}

void send_to_one(message* msg){
    add_log(msg);
    int queue_id = msg->queue_id;
    message msg_to_send = {.type = TO_ONE, .time=msg->time};
    strcpy(msg_to_send.text, msg->text);
    send_msg(queue_id, &msg_to_send);
}

void send_to_all(message* msg){
    add_log(msg);
    for(int i=0; i<MAX_CLIENTS; i++){
        message msg_to_send = {.type = TO_ALL, .time=msg->time};
        strcpy(msg_to_send.text, msg->text);
        send_msg(clients[i].queue_id, &msg_to_send);
    }
}

void init_client(message* msg){
    add_log(msg);
    printf("INFO: INIT received\n");

    int id = -1;
    for(int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].is_running == false){
            printf("INFO: client ID: %d\n", i);
            id = i;
            break;
        }
    }

    if (id == -1){
        printf("ERROR: No free client queues!");
        exit(1);
    }

    int queue_id = msg->queue_id;
    client new_client = {id, queue_id, true};
    clients[id] = new_client;

    message msg_back = {.type = INIT, .sender_id = id};
    send_msg(queue_id, &msg_back);
}

void stop_client(message* msg){
    add_log(msg);
    int client_id = msg->sender_id;
    clients[client_id].is_running = false;
    printf("INFO: STOP received from: %d\n", client_id);
}

void handle_msg(message* msg){
    switch (msg->type) {
        case INIT:
            init_client(msg);
        case STOP:
            stop_client(msg);
        case LIST:
            list_clients(msg);
        case TO_ONE:
            send_to_one(msg);
        case TO_ALL:
            send_to_all(msg);
    }
}

void stop_server(){
    printf("INFO: STOPPING SERVER\n");
    message msg;
    int client_queue;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].is_running) {
            printf("INFO: Sending STOP to client with id: %d\n", i);

            client_queue = clients[i].queue_id;
            msg.type = STOP;
            send_msg(client_queue, &msg);

            receive_msg(server_queue, &msg, STOP);
            printf("INFO: STOP received by client\n");
        }
    }

    delete_queue(server_queue);
    fclose(server_logs);
    printf("INFO: SERVER STOPPED\n");
}

void handle_signal(){
    printf("INFO: SIGINT received\n");
    exit(0);
}

int main(){
    printf("---- SERVER ----\n");

    atexit(stop_server);

    key_t key = ftok(get_home_path(), SERVER_ID);

    server_queue = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (server_queue == -1){
        printf("ERROR: queue problem!\n");
        return -1;
    }
    printf("INFO: Server queue with id %d\n", server_queue);

    signal(SIGINT, handle_signal);

    for(int i = 0; i < MAX_CLIENTS; i++)
        clients[i].is_running = false;

    server_logs = fopen("log.txt", "w");

    message msg;
    while(true){
        if (msgrcv(server_queue, &msg, sizeof(message) - sizeof(msg.type), -TO_ALL-1, 0) != -1){
            handle_msg(&msg);
        }

    }
}