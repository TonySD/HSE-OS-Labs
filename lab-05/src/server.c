#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include <arpa/inet.h>
#include <sys/resource.h>
#include <errno.h>
#include "helpers.h"

int main() {
    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        write(2, "socket failed\n", 15);
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    for (size_t i = 0; i < my_strlen(SERVER_SOCKET_PATH); ++i) addr.sun_path[i] = SERVER_SOCKET_PATH[i];

    unlink(SERVER_SOCKET_PATH); // Delete previous socket file

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        write(2, "bind failed\n", 12);
        return 1;
    }

    char inbuf[8192];
    struct sockaddr_un client_addr;
    socklen_t client_len = (socklen_t)sizeof(client_addr);
    long bytes_received = recvfrom(sockfd, inbuf, sizeof(inbuf), 0, (struct sockaddr *)&client_addr, &client_len);
    if (bytes_received <= 0) {
        write(2, "recvfrom failed\n", 16);
        return 1;
    }

    char outbuf[8192];
    size_t outpos = 0;

    size_t idx = 0;
    while (idx < (size_t)bytes_received) {
        size_t start = idx;
        while (idx < (size_t)bytes_received && inbuf[idx] != '\n') idx++;

        long pid = str_to_i64(inbuf + start);

        long priority = getpriority(PRIO_PROCESS, (pid_t)pid);

        char numbuf[64];
        size_t len_pid = i64_to_str(pid, numbuf);
        for (size_t i = 0; i < len_pid; i++) outbuf[outpos++] = numbuf[i];
        outbuf[outpos++] = ' ';

        size_t len_pr = i64_to_str((long)priority, numbuf);
        for (size_t i = 0; i < len_pr; i++) outbuf[outpos++] = numbuf[i];
        outbuf[outpos++] = '\n';

        idx++; // skip '\n'
    }

    sendto(sockfd, outbuf, outpos, 0, (struct sockaddr *)&client_addr, client_len);
    return 0;
}


