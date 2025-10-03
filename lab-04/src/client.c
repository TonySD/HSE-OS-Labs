#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include "helpers.h"

int is_binary(const char *data, size_t size) {
    size_t i = 0;
    while (i < size) {
        unsigned char c = (unsigned char)data[i++];
        if (c == 0 || c == ' ' || c == 'd') return 0;
        if (c == 'x' || c == 'X') return 1;
    }
    return 0;
}

int main() {
    key_t key = SHM_KEY;
    long shm_id = shmget(key, SHM_SIZE, 0666);
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

    char *data = (char *)addr + sizeof(struct shm_header);
    size_t size = header->data_size;

    size_t start = 0;
    while (start < size) {
        size_t line_start = start;
        while (start < size && data[start] != '\n') start++;
        size_t line_len = start - line_start;
        if (line_len > 0) {
            if (is_binary(data + line_start, line_len)) {
                write(1, data + line_start, line_len);
                write(1, "\n", 1);
            }
        }
        start++;
    }

    struct shmid_ds shm_info;
    if (shmctl(shm_id, IPC_STAT, &shm_info) < 0) {
        write(2, "shmctl failed\n", 14);
    } else {
        char buf[64];
        size_t len = i64_to_str((long)shm_info.shm_lpid, buf);
        buf[len] = 0;
        write(1, "Last attached pid: ", 20);
        write(1, buf, len);
        write(1, "\n", 1);
    }

    header->read_done = 1;
    shmdt(addr);
    return 0;
}
