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

static char* string = "===========================================================\n";

static bool
is_valid_ipv4_address(char* ip_str) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip_str, &(sa.sin_addr));
    return result != 0;
}

static void
handle_help_command() {
    char* commands = "Available commands:\n"
                     " - start - start sniffing on the specified interface\n"
                     " - stop - stop sniffing\n"
                     " - stats [iface] - show statistics for the specified interface\n"
                     " - show [ip] - show the number of packets received from the specified IP\n"
                     " - select [iface] - select specific iface for sniffing\n"
                     " - help - show this help message\n"
                     " - exit - exit the program\n";
    logging(stdout, "%s", commands);
}

static void
handle_stat_command(char* command, int* client_socket) {
    char buffer[BUFFER_SIZE];
    send_data_to_socket(client_socket, command, strlen(command), NULL);
    read_data_from_socket(client_socket, buffer, BUFFER_SIZE, NULL);

    logging(stdout, "%s", buffer);
}

static void
handle_show_command(char* command, int* client_socket) {
    send_data_to_socket(client_socket, command, strlen(command), NULL);

    int ip_number = 0;
    read_data_from_socket(client_socket, &ip_number, sizeof(ip_number), NULL);

    strtok(command, " ");
    char* ip = strtok(NULL, " ");
    logging(stdout, "This IP [%s], has been encountered %d times\n", ip, ip_number);
}

static void
command_dispatcher(char* command, int* client_socket) {
    char* token = strtok(strdup(command), " ");
    size_t command_len = strlen(command);

    if (!strcmp(token, "start") || !strcmp(token, "stop")
        || !strcmp(token, "select") || !strcmp(token, "exit")) {
        send_data_to_socket(client_socket, command, command_len, NULL);
        if (token[0] == 'e') {
            close(*client_socket);
            exit(0);
        }
    } else if (!strcmp(token, "stat")) {
        handle_stat_command(command, client_socket);
    } else if (!strcmp(token, "show")) {
        if (is_valid_ipv4_address(strtok(NULL, " "))) {
            handle_show_command(command, client_socket);
        } else {
            logging(stdout, "Failed to accept ip. You input wrong format for ip. Example: 127.0.0.1\n");
        }
    } else {
        handle_help_command();
    }

    logging(stdout, "%s", string);
}


static void
main_loop(int* client_socket) {
    char command[COMMAND_SIZE];

    logging(stdout, "%sWelcome to the packet sniffer\n%s\n", string, string);

    while (true) {
        memset(command, 0, COMMAND_SIZE);
        logging(stdout, "Enter a command (start, stop, stat, select, show, help, exit): \n");
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
