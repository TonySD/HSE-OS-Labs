#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "helpers.h"

int is_system_process(long pid) {
    char path[64];
    size_t pos = 0;
    
    // Building /proc/<pid>/status to path variable
    const char *prefix = "/proc/";
    while (prefix[pos]) { path[pos] = prefix[pos]; pos++; }
    char numbuf[32];
    size_t nlen = i64_to_str(pid, numbuf);
    for (size_t i = 0; i < nlen; i++) path[pos++] = numbuf[i];
    path[pos++] = '/';
    path[pos++] = 's'; path[pos++] = 't'; path[pos++] = 'a'; path[pos++] = 't'; path[pos++] = 'u'; path[pos++] = 's';
    path[pos] = 0;

    // Opening /proc/<pid>/status
    int fd = open(path, O_RDONLY);
    if (fd < 0) return 0;

    char buf[4096];
    long bytes_read = read(fd, buf, sizeof(buf));
    close(fd);
    if (bytes_read <= 0) return 0;

    // Searching for "Uid:" in the file
    const char *needle = "Uid:";
    size_t i = 0;
    while ((long)i <= bytes_read - 4) {
        if (buf[i] == 'U' && buf[i+1] == 'i' && buf[i+2] == 'd' && buf[i+3] == ':') break;
        i++;
    }
    if ((long)i > bytes_read - 4) return 0;
    i += 4;

    // Skipping spaces and tabs
    while ((long)i < bytes_read && (buf[i] == ' ' || buf[i] == '\t')) i++;

    long uid = 0;
    int has_digit = 0;
    while ((long)i < bytes_read && buf[i] >= '0' && buf[i] <= '9') {
        has_digit = 1;
        uid = uid * 10 + (buf[i] - '0');
        i++;
    }
    if (!has_digit) return 0;

    // System process has uid == 0
    return uid == 0;
}

int main() {
    char sendbuf[8192];
    size_t sendpos = 0;

    DIR *d = opendir("/proc");
    if (!d) {
        write(2, "opendir failed\n", 15);
        return 1;
    }
    struct dirent *de;
    while ((de = readdir(d))) {
        if (de->d_type != DT_DIR && de->d_type != DT_UNKNOWN) continue;
        if (!is_digits(de->d_name)) continue;

        long pid = str_to_i64(de->d_name);
        if (!is_system_process(pid)) continue;

        char numbuf[32];
        size_t number_length = i64_to_str(pid, numbuf);
        if (sendpos + number_length + 1 >= sizeof(sendbuf)) break;
        for (size_t i = 0; i < number_length; i++) sendbuf[sendpos++] = numbuf[i];
        sendbuf[sendpos++] = '\n';
    }
    closedir(d);

    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        write(2, "socket failed\n", 15);
        return 1;
    }

    struct sockaddr_un client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;

    for (size_t i = 0; i < my_strlen(CLIENT_SOCKET_PATH); ++i) client_addr.sun_path[i] = CLIENT_SOCKET_PATH[i];
    unlink(CLIENT_SOCKET_PATH);

    if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        write(2, "bind client", 11);
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    for (size_t i = 0; i < my_strlen(SERVER_SOCKET_PATH); ++i) addr.sun_path[i] = SERVER_SOCKET_PATH[i];

    if (sendpos == 0) {
        // if nothing found - send 0
        sendpos = 2;
        sendbuf[0] = '0';
        sendbuf[1] = '\n';
    }

    if (sendto(sockfd, sendbuf, sendpos, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        write(2, "sendto failed\n", 14);
        return 1;
    }

    char recvbuf[8192];
    long bytes_read = recv(sockfd, recvbuf, sizeof(recvbuf), 0);
    if (bytes_read <= 0) {
        write(2, "recv failed\n", 12);
        return 1;
    }
    write(1, recvbuf, (size_t)bytes_read);

    unlink(CLIENT_SOCKET_PATH);
    unlink(SERVER_SOCKET_PATH);
    return 0;
}


