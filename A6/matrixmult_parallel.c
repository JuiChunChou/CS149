#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>

#define ROWS 8
#define COLS 8


int read_matrix(const char* filename, int array[][COLS]) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return -1;
    }

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            array[i][j] = 0;
        }
        
    }
    fprintf(stdout, "%s\n", filename);
    fflush(stdout);
    // read line 
    char line[100];
    int rows = 0;
    while (rows < ROWS && fgets(line, 100, fp))
    {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';
        int cols = 0;
        char * temp = strtok(line, " ");
        while (temp && cols < COLS)
        {
            array[rows][cols] = atoi(temp);
            cols ++;
            temp = strtok(NULL, " ");
        }
        rows ++;
                
    }
    fclose(fp);
    //  for (int i = 0; i < ROWS; i++)
    // {
    //     for (int j = 0; j < COLS; j++)
    //     {
    //         printf("%d ", array[i][j]);
    //     }
    //     printf("\n");
    // }
    return 1;
}

void compute_row(int row, int a[][COLS], int w[][COLS], int* result_pipe) {
    int sum;
    for (int j = 0; j < COLS; j++) {
        sum = 0;
        for (int k = 0; k < COLS; k++) {
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
    
    int a[ROWS][COLS];
    int w[ROWS][COLS];
    

    
    if (!read_matrix(argv[1], a)) {
        return 1;
    }

    if (!read_matrix(argv[2], w)) {
        return 1;
    }

    // Create pipes for communication
    
    char filename[100];
    int *r = NULL;
    int numberA = 0;
    while (1)
    {
        r = (int *) realloc(r, sizeof(int) * ((numberA + 1) * ROWS * COLS));

        int result_pipe[ROWS][2];
        for (int i = 0; i < ROWS; i++) {
            if (pipe(result_pipe[i]) == -1) {
                perror("Pipe creation failed");
                return -1;
            }
        }
        // Fork a child process for each row of A
        for (int i = 0; i < ROWS; i++) {
            pid_t child_pid = fork();
            if (child_pid == -1) {
                perror("Fork failed");
                return -1;
            }
            if (child_pid == 0) {
                // Child process computes a row
                close(result_pipe[i][0]); // Close the read end of the pipe in the child process
                compute_row(i, a, w, result_pipe[i]);
                
            } else {
                close(result_pipe[i][1]);
            }
        }

       

        // printf("Result of A*W = \n");
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                int sum;
                read(result_pipe[i][0], &sum, sizeof(int)); // Read the result from the pipe
                r[ROWS * COLS * numberA + i*COLS + j] = sum;
                // printf("%d ", r[ROWS * COLS * numberA + i*COLS + j] );
            }
            //printf("\n");
        }

        // Wait for all child processes to finish
        for (int i = 0; i < ROWS; i++) {
            wait(NULL);
        }
        numberA ++;
        //fprintf(stderr, "compute %d A matrix in child process %d\n", numberA, getpid());
        if(!fgets(filename, 100, stdin)) {
            break;
        }
        if (filename[strlen(filename) - 1] == '\n') {
            filename[strlen(filename) - 1] = '\0';
        }
        // printf("%s\n", filename);
        if(read_matrix(filename, a) < 0) {
            break;
        }

        
    }
    //fprintf(stderr, "compute %d A matrices in total in child process %d\n", numberA, getpid());
    for (int i = 0; i < ROWS * numberA; i++) {
        for (int j = 0; j < COLS; j++) {
            
            printf("%d ", r[i*COLS + j] );
        }
        printf("\n");
    }
    free(r);

    return 0;
}