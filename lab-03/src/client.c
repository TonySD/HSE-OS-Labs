#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdint.h>
#include <stdlib.h>
#include "helpers.h"

int main(int argc, char **argv) {
    key_t key = MSG_KEY;
    long qid = syscall(SYS_msgget, key, IPC_CREAT | 0666);
    if (qid < 0) {
        write(2, "msgget failed\n", 14);
        return 1;
    }

    long mtype = 1;
    if (argc > 1) {
        if (str_to_i64(argv[1], &mtype) != 0 || mtype <= 0) mtype = 1;
    }

    struct msgbuf_local m;
    m.mtype = mtype;

    int idx = 2;
    size_t pos = 0;
    while (idx < argc) {
        const char *s = argv[idx++];
        while (*s && pos + 1 < sizeof(m.mtext)) m.mtext[pos++] = *s++;
        if (idx < argc && pos + 1 < sizeof(m.mtext)) m.mtext[pos++] = '\t';
    }
    if (pos < sizeof(m.mtext)) m.mtext[pos] = '\0';
    else m.mtext[sizeof(m.mtext)-1] = '\0';

    long r = syscall(SYS_msgsnd, qid, &m, sizeof(m.mtext), 0);
    if (r < 0) {
        const char *e = "msgsnd failed\n";
        xwrite(2, e, 14);
        return 1;
    }

    const char *ok = "sent\n";
    xwrite(1, ok, 5);
    return 0;
}


