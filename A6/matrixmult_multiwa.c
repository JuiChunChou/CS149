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
    //pid_t parent_pid = getppid();
    char log_filename[20];
    snprintf(log_filename, sizeof(log_filename), "%d.out", pid);

    int log_file = open(log_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    dup2(log_file, STDOUT_FILENO);
    
    //dup2(log_file, STDERR_FILENO);
    close(log_file);

    snprintf(log_filename, sizeof(log_filename), "%d.err", pid);

    int err_file = open(log_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    dup2(err_file, STDERR_FILENO);

    close(err_file);

    // printf("Starting command %d: child PID %d of parent PPID %d\n", child_index, pid, parent_pid);
    // printf("[%s]\n", A_filename);
    // printf("[%s]\n", W_filename);
}

void log_child_exit(int child_index, int exit_status, int signal_number, int child_pid) {
    // pid_t pid = getpid();
    char log_filename[20];
    snprintf(log_filename, sizeof(log_filename), "%d.out", child_pid);

    FILE* fp_out = fopen(log_filename, "a");

     snprintf(log_filename, sizeof(log_filename), "%d.err", child_pid);
    FILE* fp_err = fopen(log_filename, "a");
    fprintf(fp_out, "Finished child %d pid of parent %d\n", child_pid, getpid());
    if (WIFEXITED(exit_status)) {
        fprintf(fp_out, "Exited with exit code = %d\n", WEXITSTATUS(exit_status));
    } else if (WIFSIGNALED(exit_status)) {
        fprintf(fp_err, "Killed with signal %d\n", WTERMSIG(exit_status));
    }
    fclose(fp_err);
    fclose(fp_out);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s A W1 W2 W3\n", argv[0]);
        return -1;
    }
    int n = argc - 2;
    int ** result_pipe = (int **) malloc(n * sizeof(int*)); 
    pid_t * child_pids = (pid_t*) malloc(n * sizeof(pid_t));
    for (int i = 0; i < n; i++) {
        result_pipe[i] = (int *) malloc(sizeof(int) *2);
        if (pipe(result_pipe[i]) == -1) {
            perror("Pipe creation failed");
            return -1;
        }
        child_pids[i] = -1;
    }

    for (int i = 2; i < argc; i++) {
        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("Fork failed");
            return -1;
        }

        if (child_pid == 0) {
            // log_child_start(i - 2, argv[1], argv[i]);
            
            // here is a weired thing, why we should should close all other pipe to 
            // send EOF to the pipe 
            for(int j = 0; j < n; j ++) {
                if (j != i-2) {
                    close(result_pipe[j][0]);
                    close(result_pipe[j][1]);
                }
            }
            close(result_pipe[i-2][1]);
            dup2(result_pipe[i-2][0], STDIN_FILENO);
            pid_t pid = getpid();
            //pid_t parent_pid = getppid();
            char log_filename[20];
            snprintf(log_filename, sizeof(log_filename), "%d.out", pid);

            int log_file = open(log_filename, O_RDWR|O_CREAT|O_TRUNC, 0666);
            dup2(log_file, STDOUT_FILENO);
            //dup2(log_file, STDERR_FILENO);
            close(log_file);
            // Execute the matrixmult_parallel command
            execl("./matrixmult_threaded", "./matrixmult_threaded", argv[1], argv[i], (char *)0);

            // If execl() fails, it will reach here
            perror("Exec failed");
            exit(1);
        } else {
            // close read end of the pipe
            close(result_pipe[i-2][0]);
            child_pids[i-2] = child_pid;
            
        }
    }

    char line[100];
    
    while (fgets(line, 100, stdin))
    {
        for (int i = 0; i < n; i++) {
        
            write(result_pipe[i][1], line, strlen(line));
        }
    }
    

    for (int i = 0; i < n; i++) {
        
        close(result_pipe[i][1]);
    }

    // int ppid = getpid();
    for (int i = 0; i < n; i++) {
        
        
        free(result_pipe[i]);       
        int status;
        pid_t wpid;
        wpid = waitpid(child_pids[i], &status, 0);
        
        log_child_exit(i - 2, status, WIFSIGNALED(status) ? WTERMSIG(status) : 0, wpid);
    }
    free(result_pipe);
    free(child_pids);
    return 0;
}

