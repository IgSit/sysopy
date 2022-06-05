#include "common.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

client *clients[MAX_CLIENTS];
int clients_cnt = 0;

int get_opponent(int idx){
    return idx % 2 == 0 ? idx + 1 : idx - 1;
}

int first_client() {
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] == NULL)
            return i;
    }
    return -1;
}

int another_waiting() {
    for (int i = 0; i < MAX_CLIENTS; i += 2){
        if (clients[i] != NULL && clients[i + 1] == NULL)
            return i + 1;
    }
    return -1;
}

void init_client(char *name, int fd, int idx) {
    client *new_client = calloc(1, sizeof(client));
    new_client->name = calloc(MAX_MSG_LEN, sizeof(char));
    strcpy(new_client->name, name);
    new_client->fd = fd;
    new_client->available = true;

    clients[idx] = new_client;
    clients_cnt++;
}

int add_client(char *name, int fd){
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0) return -1;

    int idx = another_waiting();
    if (idx == -1)
        idx = first_client();
    if (idx != -1)
        init_client(name, fd, idx);
    return idx;
}

int find_client(char *name){
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0) return i;
    return -1;
}

void free_player(int index) {
    free(clients[index]->name);
    free(clients[index]);
    clients[index] = NULL;
    clients_cnt--;
}

void free_client(int index){
    free_player(index);
    int opponent = get_opponent(index);
    if (clients[get_opponent(index)] != NULL)
        free_player(opponent);
}

void remove_client(char *name){
    int idx = -1;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0) idx = i;
    free_client(idx);
}

void delete_not_available_clients(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] && ! clients[i]->available)
            remove_client(clients[i]->name);
    }
}

void send_pings(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i]){
            send(clients[i]->fd, "ping: ", MAX_MSG_LEN, 0);
            clients[i]->available = false;
        }
    }
}

void* ping(){
    while (true){
        printf("PING\n");
        pthread_mutex_lock(&mutex);
        delete_not_available_clients();
        send_pings();
        pthread_mutex_unlock(&mutex);
        sleep(10);
    }
}

void pong(char* name) {
    int player = find_client(name);
    if (player != -1) clients[player]->available = 1;
}

int set_local_socket(char* socket_path){
    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sock_fd  == -1)
        exit(1);

    struct sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(struct sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, socket_path);

    unlink(socket_path);
    bind(sock_fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr));
    listen(sock_fd, MAX_CLIENTS);
    return sock_fd;
}

int set_network_socket(char *port){
    int sock_fd = socket(AF_INET , SOCK_STREAM, 0);

    struct sockaddr_in sock_addr;
    sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(atoi(port));

    bind(sock_fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr));
    listen(sock_fd, MAX_CLIENTS);
    return sock_fd;

}

struct pollfd* set_fds(int local_socket, int network_socket) {
    struct pollfd *fds = calloc(2 + clients_cnt, sizeof(struct pollfd));
    fds[0].fd = local_socket;
    fds[0].events = POLLIN;
    fds[1].fd = network_socket;
    fds[1].events = POLLIN;

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < clients_cnt; i++){
        fds[i + 2].fd = clients[i]->fd;
        fds[i + 2].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);

    poll(fds, clients_cnt + 2, -1);
    return fds;
}

int check_messages(int local_socket, int network_socket){
    struct pollfd *fds = set_fds(local_socket, network_socket);
    int retval = -1;
    for (int i = 0; i < clients_cnt + 2; i++){
        if (fds[i].revents & POLLIN){
            retval = fds[i].fd;
            break;
        }
    }
    if (retval == local_socket || retval == network_socket){
        retval = accept(retval, NULL, NULL);
    }
    free(fds);
    return retval;
}

void add(char* name, int client_fd) {
    int index = add_client(name, client_fd);

    if (index == -1){
        send(client_fd, "add:name_taken", MAX_MSG_LEN, 0);
        close(client_fd);
    }
    else if (index % 2 == 0){
        send(client_fd, "add:no_enemy", MAX_MSG_LEN, 0);
    }
    else{
        int random_num = rand() % 100;
        int first, second;
        if (random_num % 2 == 0){
            first = index;
            second = get_opponent(index);
        }
        else{
            first = get_opponent(index);
            second = index;
        }

        send(clients[first]->fd, "add:O",
             MAX_MSG_LEN, 0);
        send(clients[second]->fd, "add:X",
             MAX_MSG_LEN, 0);
    }
}

void move(char* arg, char* name, char buffer[]) {
    int move = atoi(arg);
    int player = find_client(name);

    sprintf(buffer, "move:%d", move);
    send(clients[get_opponent(player)]->fd, buffer, MAX_MSG_LEN,
         0);
}

void client_routine(int local_socket, int network_socket) {
    while (true){
        int client_fd = check_messages(local_socket, network_socket);
        char buffer[MAX_MSG_LEN + 1];
        recv(client_fd, buffer, MAX_MSG_LEN, 0);

        printf("%s\n", buffer);
        char *command = strtok(buffer, ":");
        char *arg = strtok(NULL, ":");
        char *name = strtok(NULL, ":");

        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0)
            add(name, client_fd);
        else if (strcmp(command, "move") == 0)
            move(arg, name, buffer);
        else if (strcmp(command, "end") == 0)
            remove_client(name);
        else if (strcmp(command, "pong") == 0)
            pong(name);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char* argv[]){
    if (argc != 3){
        printf("Wrong number of arguments!\n");
        exit(1);
    }
    srand(time(NULL));

    char *port = argv[1];
    char *socket_path = argv[2];

    int local_socket = set_local_socket(socket_path);
    int network_socket = set_network_socket(port);

    pthread_t t;
    pthread_create(&t, NULL, &ping, NULL);

    client_routine(local_socket, network_socket);
}