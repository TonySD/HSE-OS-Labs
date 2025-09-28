#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdint.h>
#include "helpers.h"

// argv[1] = message type
int main(int argc, char **argv) {
    key_t key = MESSAGE_KEY;
    int flags = IPC_CREAT | 0666;
    long queue_id = msgget(key, flags);
    if (queue_id < 0) {
        write(2, "msgget failed\n", 14);
        return 1;
    }

    long message_type = 1;
    if (argc > 1) {
        message_type = str_to_i64(argv[1]);
    }

    struct msgbuf_local last_message;
    int have = 0;
    while (1) {
        struct msgbuf_local current_message;
        long r = msgrcv(queue_id, &current_message, sizeof(current_message.mtext), message_type, IPC_NOWAIT);
        if (r < 0) break;
        last_message = current_message;
        have = 1;
    }

    struct msqid_ds ds;
    long ctlr = msgctl(queue_id, IPC_STAT, &ds);
    if (ctlr < 0) {
        write(2, "msgctl stat failed\n", 19);
    }

    if (have) {
        my_puts(last_message.mtext, slen(last_message.mtext));
    } else {
        write(1, "no messages\n", 12);
    }

    if (ctlr == 0) {
        char buf[64];
        unsigned long n = (unsigned long)ds.msg_qnum;
        int i = 63;
        buf[i--] = '\n';
        if (n == 0) buf[i--] = '0';
        while (n > 0 && i >= 0) {
            buf[i--] = (char)('0' + (n % 10));
            n /= 10;
        }
        xwrite(1, "count ", 6);
        xwrite(1, buf + i + 1, (size_t)(63 - i));
    }

    syscall(SYS_msgctl, qid, IPC_RMID, 0);
    return 0;
}


