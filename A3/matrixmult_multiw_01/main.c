#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define MATRIX_SIZE 8

void log_child_start(int child_index, char* A_filename, char* W_filename) {
    pid_t pid = getpid();
    pid_t parent_pid = getppid();
    char log_filename[20];
    snprintf(log_filename, sizeof(log_filename), "%d.out", pid);

    int log_file = open(log_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    dup2(log_file, STDOUT_FILENO);
    dup2(log_file, STDERR_FILENO);
    close(log_file);

    printf("Starting command %d: child PID %d of parent PPID %d\n", child_index, pid, parent_pid);
    printf("[%s]\n", A_filename);
    printf("[%s]\n", W_filename);
}

void log_child_exit(int child_index, int exit_status, int signal_number) {
    pid_t pid = getpid();
    char log_filename[20];
    snprintf(log_filename, sizeof(log_filename), "%d.out", pid);

    int log_file = open(log_filename, O_WRONLY | O_APPEND);
    dup2(log_file, STDOUT_FILENO);
    dup2(log_file, STDERR_FILENO);
    close(log_file);

    if (WIFEXITED(exit_status)) {
        printf("Exited with exit code = %d\n", WEXITSTATUS(exit_status));
    } else if (WIFSIGNALED(exit_status)) {
        printf("Killed with signal %d\n", WTERMSIG(exit_status));
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s A W1 W2 W3\n", argv[0]);
        return -1;
    }

    for (int i = 2; i < argc; i++) {
        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("Fork failed");
            return -1;
        }

        if (child_pid == 0) {
            log_child_start(i - 2, argv[1], argv[i]);

            // Execute the matrixmult_parallel command
            execl("./matrixmult_parallel", "./matrixmult_parallel", argv[1], argv[i], (char *)0);

            // If execl() fails, it will reach here
            perror("Exec failed");
            exit(1);
        } else {
            int status;
            pid_t wpid;
            wpid = waitpid(child_pid, &status, 0);
            log_child_exit(i - 2, status, WIFSIGNALED(status) ? WTERMSIG(status) : 0);
        }
    }

    return 0;
}

