#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include "utils.h"
#include "server.h"
#include "exception.h"

void
logging(FILE* o_stream, char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(o_stream, format, args);
    va_end(args);
    fflush(o_stream);
}

void
set_connection_options(int* t_socket, char* socket_path, FILE* out_stream) {
    create_socket(t_socket, AF_UNIX, out_stream);

    struct sockaddr_un server_addr = {0};
    make_address(&server_addr, AF_UNIX, socket_path);
#ifdef DAEMON
    int option_value = 1;
    set_socket_option(t_socket, SOL_SOCKET, SO_REUSEADDR, option_value, sizeof(int), out_stream);
    bind_socket(t_socket, &server_addr, out_stream);
    listen_socket(t_socket, SOMAXCONN, out_stream);
#else
    connect_socket(t_socket, &server_addr, NULL);
#endif
}

#ifdef DAEMON

static void
sigterm_handler() {
    exit(0);
}

FILE*
open_file() {
    FILE* fp = fopen("Logs.txt", "w+");
    if (!fp) {
        perror("Failed to open file\n");
        exit(-1);
    }
    return fp;
}

void
set_daemon_mode() {
    pid_t process_id;
    pid_t sid;

    process_id = fork();
    if (process_id < 0) {
        logging(stderr, "Failed to fork processes");
        exit(EC_CANT_FORK_PROCESS);
    }

    if (process_id > 0) {
        logging(stdout, "process_id of child process %d \n", process_id);
        exit(EC_OK);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        logging(stderr, "Failed to set sid");
        exit(EC_CANT_SET_SID);
    }
    chdir(".");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGTERM, sigterm_handler);
}

void
end_processes(FILE* logs, rb_node_t** root, const int* server_socket, const int* client_socket) {
    if (root != NULL) {
        save_rbtree(*root);
        free_tree(*root);
    }
    close(*client_socket);
    close(*server_socket);
    free(root);
    fclose(logs);
}

rb_node_t**
create_rbtree() {
    return malloc(sizeof(rb_node_t*));
}
#endif
