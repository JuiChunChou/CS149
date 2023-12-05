#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

#define ROWS 8
#define COLS 8


int* r = NULL;
pthread_mutex_t r_lock;
int a[ROWS][COLS];
int w[ROWS][COLS];

// used for pass args to the child thread
struct arg{
    int row;
    int col; 
    
};

int read_matrix(const char* filename, int array[][COLS]) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Open file error");
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
    
    return 1;
}

// void compute_row(int row, int a[][COLS], int w[][COLS], int* result_pipe) {
//     int sum;
//     for (int j = 0; j < COLS; j++) {
//         sum = 0;
//         for (int k = 0; k < COLS; k++) {
//             sum += a[row][k] * w[k][j];
//         }
//         write(result_pipe[1], &sum, sizeof(int)); // Write the result to the pipe
//     }
//     close(result_pipe[1]); // Close the write end of the pipe
//     exit(0);
// }

void * compute_cell(void *args) {
    struct arg* arg = (struct arg*) args;
    int result = 0;
   
    for (int k = 0; k < COLS; k++) {
        result += a[arg->row % ROWS][k] * w[k][arg->col];
    }
    pthread_mutex_lock(&r_lock);
    r[arg->row * COLS + arg->col] = result;
    pthread_mutex_unlock(&r_lock);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s A W\n", argv[0]);
        return 1;
    }
    
    // int a[ROWS][COLS];
    // int w[ROWS][COLS];
    
    pthread_mutex_init(&r_lock, NULL);
    
    if (!read_matrix(argv[1], a)) {
        return 1;
    }

    if (!read_matrix(argv[2], w)) {
        return 1;
    }

    // Create pipes for communication
    
    char filename[100];    
    int numberA = 0;
    struct arg * arg_array = (struct arg *) malloc(sizeof(struct arg) * ROWS * COLS);
    pthread_t * pt_ids = (pthread_t *) malloc(sizeof(pthread_t) * ROWS * COLS);
    while (1)
    {
        
        pthread_mutex_lock(&r_lock);
        r = (int *) realloc(r, sizeof(int) * ((numberA + 1) * ROWS * COLS));
        pthread_mutex_unlock(&r_lock);

        
        // printf("Result of A*W = \n");
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {

                arg_array[i*COLS + j].row = i + ROWS * numberA;
                arg_array[i*COLS + j].col = j;
                pthread_create(&pt_ids[i*COLS + j], NULL, compute_cell, &arg_array[i*COLS + j]);

            }
            //printf("\n");
        }

        // Wait for all child processes to finish
        for (int i = 0; i < ROWS * COLS; i++) {
            pthread_join(pt_ids[i], NULL);
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
    free(pt_ids);
    free(arg_array);
    pthread_mutex_destroy(&r_lock);

    return 0;
}

//  gcc -D_REENTRANT -pthread -o matrixmult_threaded matrixmult_threaded.c -Wall -Werror
