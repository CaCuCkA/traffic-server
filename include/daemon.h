#ifndef TRAFFIC_SERVER_DAEMON_H
#define TRAFFIC_SERVER_DAEMON_H

#include <sys/socket.h>

#define BUFFER_SIZE 1024

void set_connection_options(int* server_socket, char* socket_path);
void run(char* socket_path);
#endif //TRAFFIC_SERVER_DAEMON_H
