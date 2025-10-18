#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "helpers.h"

// argv[1] = message type
int main(int argc, char **argv) {
    char buf[128];
    long queue_id = msgget((key_t) MESSAGE_KEY, IPC_CREAT | 0666);
    if (queue_id < 0) {
        write(2, "msgget failed\n", 14);
        return 1;
    }

    long message_type = 1;
    if (argc > 1) {
        message_type = str_to_i64(argv[1]);
    }

    struct msgbuf_local current_message, last_message;
    size_t counter = 0;
    long bytes_read;
    while (1) {
        bytes_read = msgrcv(queue_id, &current_message, sizeof(struct msgbuf_local) - sizeof(long), message_type, IPC_NOWAIT);
        
        if (bytes_read < 0 && counter) break;
        else if (bytes_read < 0) continue;

        last_message = current_message;
        counter++;
    }

    if (counter) {
        write(1, "\n\nServer: received message from queue:\n\n", 40);
        // Output all messages count
        write(1, "All messages count: ", 20);   
        bytes_read = i64_to_str(last_message.amount_of_messages, buf);
        write(1, buf, bytes_read);
        write(1, "\n", 1);

        // Output last message
        size_t sz = 0;
        while (sz < sizeof(last_message.authors_buffer) && last_message.authors_buffer[sz] != '\0') sz++;
        if (sz) write(1, last_message.authors_buffer, sz);
        write(1, "\n", 1);
    } else {
        write(1, "No messages\n", 12);
        return 0;
    }

    msgctl(queue_id, IPC_RMID, 0);
    return 0;
}


