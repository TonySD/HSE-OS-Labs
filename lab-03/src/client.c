#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "helpers.h"
#include "program_execution.h"

// argv[1] = message type
int main(int argc, char **argv) {
    key_t key = MESSAGE_KEY;
    long queue_id = msgget(key, IPC_CREAT | 0666);

    long mtype = 1;
    if (argc > 1) {
        mtype = str_to_i64(argv[1]);
    }

    struct msgbuf_local message = {0};
    message.mtype = mtype;
    strcat(message.authors_buffer, "Authors: \n");

    char command[1024];
    const char *user_name = getenv("USER");
    if (!user_name) user_name = "root";
    snprintf(command, sizeof(command), "cat /var/mail/%s | grep -E '^(From|Date):' | cut -d ':' -f2- | cut -d '(' -f1", user_name);

    char *mail_file = program_exec(command);

    long now_seconds = (long)time(0);
    long week_seconds = 7L * 24L * 3600L;

    char *line = strtok(mail_file, "\n");
    while (line) {
        while (*line == ' ') line++;
        time_t ts = parse_date(line);
        
        // Second line - sender
        line = strtok(NULL, "\n");
        if (!line) break;
        while (*line == ' ') line++;
        
        // Check date < last week
        if (ts != -1 && (now_seconds - (long)ts) <= week_seconds) {
            message.amount_of_messages++;
            
            // Check if author is already in the list
            if (!strstr(message.authors_buffer, line)) {
                strcat(message.authors_buffer, "- ");
                strcat(message.authors_buffer, line);
                strcat(message.authors_buffer, "\n");
                message.amount_of_authors++;
            }
        }
        
        line = strtok(NULL, "\n");
    }

    free(mail_file);

    msgsnd(queue_id, &message, sizeof(struct msgbuf_local) - sizeof(long), 0);
    write(1, "Client: message sent successfully!\n", 36);
    return 0;
}


