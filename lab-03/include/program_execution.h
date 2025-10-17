#ifndef PROGRAM_EXECUTION_H
#define PROGRAM_EXECUTION_H

#define BLOCK_SIZE 128

#include "helpers.h"
#include <sys/wait.h>

char* read_from_fd(int fd) {
    size_t bytes_read, now_bytes = 0, capacity = BLOCK_SIZE;
    char *buffer = malloc(BLOCK_SIZE);

    while ((bytes_read = read(fd, buffer + now_bytes, capacity - now_bytes - 1)) != -1) {
        // If we read less than maximum, we reached the end
        if (bytes_read < capacity - now_bytes - 1) {
            buffer[bytes_read + now_bytes] = 0;
            close(fd);

            return buffer;
        }

        now_bytes += bytes_read;
        buffer = realloc(buffer, capacity * 2 + 1);
        capacity = capacity * 2 + 1;
    }   

    write(2, "Error occured while reading from pipe", 37);
    exit(1);
}


int execute_in_fork(const char* filepath,  char* const* argv) {
    int pid, pipefd[2];

    if (pipe(pipefd) == -1) {
        write(2, "execute_in_fork pipe failed\n", 28);
        exit(1);
    }

    if ((pid = fork()) == -1) { 
        write(2, "execute_in_fork fork failed\n", 28);
        exit(1);
    }

    // child branch
    if (pid == 0) {
        // stdout = 1
        dup2(pipefd[1], 1);
        execv(filepath, argv);

        write(2, "fork cmd failed\n", 16);
        exit(1);
    } 
    // parent branch
    else {
        wait(&pid);
        close(pipefd[1]);
        return pipefd[0];
    }
}

// Just returns stdout, allocated on heap
char* program_exec(const char* program) {
    int pid, pipefd[2];

    if (pipe(pipefd) == -1) {
        write(2, "execute_in_fork pipe failed\n", 28);
        exit(1);
    }

    if ((pid = fork()) == -1) { 
        write(2, "execute_in_fork fork failed\n", 28);
        exit(1);
    }

    // child branch
    if (pid == 0) {
        // stdout = 1
        dup2(pipefd[1], 1);
        execl("/bin/bash", "/bin/bash", "-c", program, NULL);

        write(2, "fork cmd failed\n", 16);
        exit(1);
    } 
    // parent branch
    else {
        wait(&pid);
        close(pipefd[1]);
        return read_from_fd(pipefd[0]);
    }
}

#endif