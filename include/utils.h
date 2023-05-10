#ifndef TRAFFIC_SERVER_UTILS_H
#define TRAFFIC_SERVER_UTILS_H

#include <bits/types/FILE.h>

#include "rbtree.h"

void        logging(FILE* o_stream, char* format, ...);
void        set_connection_options(int* t_socket, char* socket_path, FILE* out_stream);

#ifdef DAEMON
FILE*       open_file();
rb_node_t** create_rbtree();
void        set_daemon_mode();
void        end_processes(FILE* fp, rb_node_t** root, const int* server_socket, const int* client_socket);
#endif

#endif //TRAFFIC_SERVER_UTILS_H
