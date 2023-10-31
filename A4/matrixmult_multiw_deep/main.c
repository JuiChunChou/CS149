#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define ROW 8
#define COL 8

int read_matrix(FILE* fp, int matrix[ROW][COL]) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (fscanf(fp, "%d", &matrix[i][j]) != 1) {
                perror("Error reading matrix");
                return -1;
            }
        }
    }
    return 0;
}

void compute_row(int row, int a[ROW][COL], int w[COL][COL], int* result_pipe) {
    int sum = 0;
    for (int j = 0; j < COL; j++) {
        sum = 0;
        for (int k = 0; k < COL; k++) {
            sum += a[row][k] * w[k][j];
        }
        write(result_pipe[1], &sum, sizeof(int)); // Write the result to the pipe
    }
    close(result_pipe[1]); // Close the write end of the pipe
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 10) {
        printf("Usage: %s A W1 W2 W3 W4 W5 W6 W7 W8\n", argv[0]);
        return -1;
    }

    FILE* fpA = fopen(argv[1], "r");
    FILE* fpW[8];

    for (int i = 0; i < 8; i++) {
        fpW[i] = fopen(argv[i + 2], "r");
    }

    if (fpA == NULL || fpW[0] == NULL) {
        perror("Error opening files");
        return -1;
    }

    int a[ROW][COL];
    int w[8][ROW][COL];
    int r[8][ROW][COL];

    if (read_matrix(fpA, a) == -1) {
        return -1;
    }

    for (int i = 0; i < 8; i++) {
        if (read_matrix(fpW[i], w[i]) == -1) {
            return -1;
        }
    }

    // Create pipes for communication
    int result_pipe[8][2];

    for (int i = 0; i < 8; i++) {
        if (pipe(result_pipe[i]) == -1) {
            perror("Pipe creation failed");
            return -1;
        }
    }

    // Fork a child process for each set of matrices
    for (int i = 0; i < 8; i++) {
        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("Fork failed");
            return -1;
        }
        if (child_pid == 0) {
            // Child process computes a row
            close(result_pipe[i][0]); // Close the read end of the pipe in the child process
            compute_row(i, a, w[i], result_pipe[i]);
        }
    }

    // Parent process reads the results from child processes
    for (int i = 0; i < 8; i++) {
        close(result_pipe[i][1]); // Close the write end of the pipe in the parent process
    }

    printf("Results of A * W1 to W8:\n");

    for (int i = 0; i < 8; i++) {
        printf("Result %d:\n", i + 1);
        for (int j = 0; j < ROW; j++) {
            for (int k = 0; k < COL; k++) {
                int sum;
                read(result_pipe[i][0], &sum, sizeof(int)); // Read the result from the pipe
                r[i][j][k] = sum;
                printf("%d ", r[i][j][k]);
            }
            printf("\n");
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < 8; i++) {
        wait(NULL);
    }

    return 0;
}