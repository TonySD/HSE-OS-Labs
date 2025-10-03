#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "helpers.h"

int main() {
    key_t key = SHM_KEY;
    long shm_id = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id < 0) {
        write(2, "shmget failed\n", 14);
        return 1;
    }

    void *addr = shmat(shm_id, 0, 0);
    if ((long)addr == -1) {
        write(2, "shmat failed\n", 13);
        return 1;
    }

    struct shm_header *header = (struct shm_header *)addr;
    header->data_size = 0;
    header->read_done = 0;

    char *data = (char *)addr + sizeof(struct shm_header);
    size_t capacity = SHM_SIZE - sizeof(struct shm_header);

    int pipefd[2];
    if (pipe(pipefd) < 0) {
        write(2, "pipe failed\n", 12);
        return 1;
    }

    int pid = fork();
    if (pid == 0) {
        dup2(pipefd[1], 1);
        close(pipefd[0]);
        close(pipefd[1]);
        char *argv_exec[] = {"/bin/ls", "-l", ".", 0};
        execv("/bin/ls", argv_exec);
        
        // if we are here, execv failed
        _exit(1);
    }

    close(pipefd[1]);

    size_t pos = 0;
    while (1) {
        long bytes_read = read(pipefd[0], data + pos, capacity - pos);
        if (bytes_read <= 0) break;
        pos += (size_t)bytes_read;
        if (pos >= capacity) break;
    }
    close(pipefd[0]);
    header->data_size = pos;

    while (header->read_done == 0) {
        usleep(10000);
    }

    shmdt(addr);
    shmctl(shm_id, IPC_RMID, 0);
    return 0;
}