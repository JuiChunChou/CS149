#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MATRIX_SIZE 8

int read_matrix(FILE* fp, int matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (fscanf(fp, "%d", &matrix[i][j]) != 1) {
                return -1;
            }
        }
    }
    return 0;
}

void print_matrix(int matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void compute_row(int row[MATRIX_SIZE], int w[MATRIX_SIZE][MATRIX_SIZE], int result_pipe) {
    int sum = 0;
    for (int j = 0; j < MATRIX_SIZE; j++) {
        for (int k = 0; k < MATRIX_SIZE; k++) {
            sum += row[k] * w[k][j];
        }
        write(result_pipe, &sum, sizeof(int));
    }
    close(result_pipe);
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s A W1 W2 W3\n", argv[0]);
        return -1;
    }

    int a[MATRIX_SIZE][MATRIX_SIZE];
    FILE* a_file = fopen(argv[1], "r");
    if (a_file == NULL) {
        printf("Error opening file %s.\n", argv[1]);
        return -1;
    }

    if (read_matrix(a_file, a) == -1) {
        printf("Error reading matrix from file %s.\n", argv[1]);
        return -1;
    }

    printf("Matrix %s:\n", argv[1]);
    print_matrix(a);

    for (int i = 2; i < argc; i++) {
        int w[MATRIX_SIZE][MATRIX_SIZE];
        FILE* w_file = fopen(argv[i], "r");
        if (w_file == NULL) {
            printf("Error opening file %s.\n", argv[i]);
            return -1;
        }

        if (read_matrix(w_file, w) == -1) {
            printf("Error reading matrix from file %s.\n", argv[i]);
            return -1;
        }

        int result_pipe[2];
        if (pipe(result_pipe) == -1) {
            perror("Pipe creation failed");
            return -1;
        }

        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("Fork failed");
            return -1;
        }

        if (child_pid == 0) {
            close(result_pipe[0]);
            compute_row(a[0], w, result_pipe[1]);
        } else {
            close(result_pipe[1]);
            int result[MATRIX_SIZE][MATRIX_SIZE];
            read(result_pipe[0], result, sizeof(result));
            close(result_pipe[0]);

            printf("%s and %s:\n", argv[1], argv[i]);

            printf("Matrix %s:\n", argv[1]);
            print_matrix(a);

            printf("Matrix %s:\n", argv[i]);
            print_matrix(w);

            printf("R(%s * %s):\n", argv[1], argv[i]);
            print_matrix(result);
            wait(NULL);
        }
    }

    return 0;
}