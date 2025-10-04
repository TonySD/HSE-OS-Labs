#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sched.h>
#include <errno.h>
#include "helpers.h"

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        write(2, "socket failed\n", 15);
        return 1;
    }

    int enable = 1;
    (void)setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
    addr.sin_port = htons(SOCKET_PORT);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        write(2, "bind failed\n", 12);
        return 1;
    }

    char inbuf[8192];
    struct sockaddr_in client_addr;
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

        struct sched_param param;
        int prio = -1;
        if (sched_getparam((pid_t)pid, &param) == 0) prio = param.sched_priority;

        char numbuf[64];
        size_t len_pid = i64_to_str(pid, numbuf);
        for (size_t i = 0; i < len_pid; i++) outbuf[outpos++] = numbuf[i];
        outbuf[outpos++] = ' ';

        size_t len_pr = i64_to_str((long)prio, numbuf);
        for (size_t i = 0; i < len_pr; i++) outbuf[outpos++] = numbuf[i];
        outbuf[outpos++] = '\n';

        idx++; // skip '\n'
    }

    sendto(sockfd, outbuf, outpos, 0, (struct sockaddr *)&client_addr, client_len);
    return 0;
}


