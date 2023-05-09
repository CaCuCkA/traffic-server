#ifdef DAEMON
#include "daemon.h"
#else
#include "client.h"
#endif
#define SOCKET_DIR "/tmp/my_socket.sock"

int
main(void) {
    run(SOCKET_DIR);
    return 0;
}