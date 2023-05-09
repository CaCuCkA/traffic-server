#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "client.h"
#include "server.h"


#define BUFFER_SIZE 2048
#define COMMAND_SIZE 40

static void
handle_help_command() {
    char* commands = "Available commands:\n"
                     " - start - start sniffing on the specified interface\n"
                     " - stop - stop sniffing\n"
                     " - stats [iface] - show statistics for the specified interface\n"
                     " - show [ip] count - show the number of packets received from the specified IP\n"
                     " - select [iface] - select specific iface for sniffing\n"
                     " - help - show this help message\n"
                     " - exit - exit the program\n";
    fprintf(stdout, "%s", commands);
}

static void
handle_stat_command(char* command, int* client_socket) {
    char buffer[BUFFER_SIZE];
    send_data_to_socket(client_socket, command, strlen(command), NULL);
    read_data_from_socket(client_socket, buffer, BUFFER_SIZE, NULL);
}

static void
handle_show_command(char* command, int* client_socket) {
    send_data_to_socket(client_socket, command, strlen(command), NULL);

    int ip_number = 0;
    read_data_from_socket(client_socket, &ip_number, sizeof(ip_number), NULL);

    strtok(command, " ");
    char* ip = strtok(NULL, " ");
    fprintf(stdout, "This IP [%s], has been encountered %d times\n", ip, ip_number);
}

static void
command_dispatcher(char* command, int* client_socket) {
    char* token = strtok(strdup(command), " ");
    size_t command_len = strlen(command);

    if (!strcmp(token, "start") || !strcmp(token, "stop")
        || !strcmp(token, "select") || !strcmp(token, "exit")) {
        fprintf(stdout, "%s\n", command);
        send_data_to_socket(client_socket, command, command_len, NULL);
        if (token[0] == 'e') {
            close(*client_socket);
            exit(0);
        }
    } else if (!strcmp(token, "stat")) {
        handle_stat_command(command, client_socket);
    } else if (!strcmp(token, "show")) {
        handle_show_command(command, client_socket);
    } else {
        handle_help_command();
    }

}


static void
main_loop(int* client_socket) {
    char command[COMMAND_SIZE];

    while (true) {
        memset(command, 0, COMMAND_SIZE);
        fprintf(stdout, "Enter a command (start, stop, stat, select, show, help, exit): \n");
        fgets(command, COMMAND_SIZE, stdin);
        command[strcspn(command, "\n")] = '\0';
        command_dispatcher(command, client_socket);
    }
}

void
run(char* socket_path) {
    int client_socket;

    set_connection_options(&client_socket, socket_path, NULL);

    main_loop(&client_socket);
}
