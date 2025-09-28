// Macro for exposing sigaction structure
#define _XOPEN_SOURCE
#define BLOCK_SIZE 128
#define newline() write(1, "\n", 1)

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

size_t find(const char *string, const char find_char) {
    const char* cur_pointer = string;
    while (*cur_pointer != 0) {
        if (*++cur_pointer == find_char) {
            return cur_pointer - string;
        }
    }
    return -1;
}

// Returns pipefd
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

int INTERRUPTED = 0;

void handle_sigint(int sig) {
    char* first_interrupt[] = {"/bin/ls", "-s", NULL};

    // First interrupted signal
    if (INTERRUPTED++ == 0) {
        int pipe_out_fd = execute_in_fork("/bin/ls", first_interrupt);
        char *cmd_output = read_from_fd(pipe_out_fd);
        write(1, cmd_output, find(cmd_output, '\0'));
        newline();
        free(cmd_output);
    } else {
        int pipe_out_fd = execute_in_fork("/bin/ls", first_interrupt);
        char *cmd_output = read_from_fd(pipe_out_fd);
        cmd_output[find(cmd_output, '\n')] = 0;

        newline();
        write(1, cmd_output, find(cmd_output, '\0'));
        newline();
        free(cmd_output);
    }
}

void initialize_signal_handler() {
    struct sigaction sigact;

    memset(&sigact, 0, sizeof(struct sigaction));       // Zeroing memory (just for safe)
    sigact.sa_handler = handle_sigint;                  // Setting handler function
    sigemptyset(&sigact.sa_mask);                       // Setting empty mask (Mask is needed to filter signals, that can interrupt our handler execution)
    sigact.sa_flags = 0;                                // Don't use any flags

    // SIGINT = 2
    sigaction(2, &sigact, (struct sigaction *)NULL);    // Bind sigaction to SIGINT signal
}


int main() {
    int read_result, cmd_output_fd, newline_index, file = 0;
    char *cmd_output, buffer[256];

    initialize_signal_handler();
    memset(buffer, 0, 256);

    while (1) {
        write(1, "Please, enter the file, which will contain result: ", 51);
        read_result = read(0, buffer, 255);
        if (read_result == -1) {
            // Signal interrupts reading. Commented for less verboseness
            // write(2, "Error while reading from stdin\n\n", 32);
            continue;
        }

        // Maybe user input won't end with newline, I decided to check it
        if ((newline_index = find(buffer, '\n')) != -1) {
            buffer[newline_index] = 0;
        }
        
        file = creat(buffer, 0644);

        if (file == -1) {
            // stderr = 2
            write(2, "Failed to open file! Maybe we don't access to it?\n\n", 51);
            continue;
        }

        // Current directory section
        write(file, "Current directory absolute path: \n", 34);
        cmd_output_fd = execute_in_fork("/usr/bin/pwd", NULL);
        cmd_output = read_from_fd(cmd_output_fd);
        write(file, cmd_output, find(cmd_output, '\0'));

        // Files in current directory section
        write(file, "\nCurrent directory files: \n", 27);
        cmd_output_fd = execute_in_fork("/usr/bin/ls", NULL);
        cmd_output = read_from_fd(cmd_output_fd);
        write(file, cmd_output, find(cmd_output, '\0'));

        close(file);
    }


    return 0;
}
