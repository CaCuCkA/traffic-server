#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "client.h"
#include "server.h"

#define TRUE 1
#define BUFFER_SIZE 2048


static void
set_connection_options(int* client_socket, char* socket_path) {
    create_socket(client_socket, AF_UNIX, NULL);

    struct sockaddr_un server_addr;
    make_address(&server_addr, AF_UNIX, socket_path);

    connect_socket(client_socket, &server_addr, NULL);
}

static void
main_loop(int* client_socket) {
    char buffer[BUFFER_SIZE];

    while (TRUE) {
        fprintf(stdout, "Enter a command (start, stop, status, info, exit): \n");
        fgets(buffer, sizeof(buffer), stdin);

        send_data_to_socket(client_socket, buffer, strlen(buffer), NULL);

        memset(buffer, 0, BUFFER_SIZE);
        read_data_from_socket(client_socket, buffer, BUFFER_SIZE, NULL);

        if (!strcmp(buffer, "exit")) {
            break;
        }
    }
}

void
run(char* socket_path) {
    int client_socket;

    set_connection_options(&client_socket, socket_path);

    main_loop(&client_socket);

    close(client_socket);
}
