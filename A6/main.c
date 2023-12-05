#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int read_row(FILE* fp) {
    int c, m = 0;
    while ((c = fgetc(fp)) != EOF)
        if (c == '\n') {
            m++;
        }
    m++;
    rewind(fp);
    return m;
}

int read_rank(FILE* fp) {
    int n = 0, c;
    while ((c = fgetc(fp)) != '\n')
        if (c == ' ')
            n++;
    n++;
    rewind(fp);
    return n;
}

void compute_row(int row, int colum1, int colum2, int a[][colum1], int w[][colum2], int* result_pipe) {
    int sum;
    for (int j = 0; j < colum2; j++) {
        sum = 0;
        for (int k = 0; k < colum1; k++) {
            sum += a[row][k] * w[k][j];
        }
        write(result_pipe[1], &sum, sizeof(int)); // Write the result to the pipe
    }
    close(result_pipe[1]); // Close the write end of the pipe
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s A W\n", argv[0]);
        return -1;
    }
    FILE* fp1, * fp2;
    fp1 = fopen(argv[1], "r");
    fp2 = fopen(argv[2], "r");

    if (fp1 == NULL || fp2 == NULL) {
        printf("Error opening files.\n");
        return -1;
    }

    int row1 = read_row(fp1);
    int colum1 = read_rank(fp1);
    int row2 = read_row(fp2);
    int colum2 = read_rank(fp2);

    printf("A is %d row and %d column\n", row1, colum1);
    printf("W is %d row and %d column\n", row2, colum2);

    int a[row1][colum1];
    int w[row2][colum2];
    int r[row1][colum2];

    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < colum1; j++) {
            fscanf(fp1, "%d", &a[i][j]);
        }
    }
    fclose(fp1);

    for (int i = 0; i < row2; i++) {
        for (int j = 0; j < colum2; j++) {
            fscanf(fp2, "%d", &w[i][j]);
        }
    }
    fclose(fp2);

    // Create pipes for communication
    int result_pipe[row1][2];
    for (int i = 0; i < row1; i++) {
        if (pipe(result_pipe[i]) == -1) {
            perror("Pipe creation failed");
            return -1;
        }
    }

    // Fork a child process for each row of A
    for (int i = 0; i < row1; i++) {
        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("Fork failed");
            return -1;
        }
        if (child_pid == 0) {
            // Child process computes a row
            close(result_pipe[i][0]); // Close the read end of the pipe in the child process
            compute_row(i, colum1, colum2, a, w, result_pipe[i]);
        }
    }

    // Parent process reads the results from child processes
    for (int i = 0; i < row1; i++) {
        close(result_pipe[i][1]); // Close the write end of the pipe in the parent process
    }

    printf("Result of A*W = \n");
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < colum2; j++) {
            int sum;
            read(result_pipe[i][0], &sum, sizeof(int)); // Read the result from the pipe
            r[i][j] = sum;
            printf("%d ",r[i][j]);
        }
        printf("\n");
    }

    // Wait for all child processes to finish
    for (int i = 0; i < row1; i++) {
        wait(NULL);
    }

    return 0;
}