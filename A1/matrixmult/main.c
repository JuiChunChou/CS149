#include <stdio.h>
#include <stdlib.h>

//read the number of rows in the matrix form the txt files
int read_row(FILE* fp){
    int c, m=0;
    while((c = fgetc(fp)) != EOF)
        if(c =='\n'){
            m++;
        }
    m++;
    rewind(fp);
    return m;
}
//read the number of columns in the matrix form the txt file
int read_rank(FILE* fp) {
    int n = 0, c;
    while ((c = fgetc(fp)) != '\n')
        if (c == ' ')
            n++;
    n++;
    rewind(fp);
    return n;
}

int main(int argc, char* argv[]) {
    //check for the correct number od command line arguments
    if (argc != 4) {
        printf("Usage: %s A W B\n", argv[0]);
        return -1;
    }
    FILE* fp1, * fp2, * fp3;
    //Open the txt files which user inputs
    fp1 = fopen(argv[1], "r");
    fp2 = fopen(argv[2], "r");
    fp3 = fopen(argv[3], "r");
    //check the files are null or not
    if (fp1 == NULL || fp2 == NULL || fp3 == NULL) {
        printf("Error opening files.\n");
        return -1;
    }
    //setting the size of the matrices A, W, B and R
    int row1 = 1, colum1 = 3, row2 = 3, colum2 = 5, row3 = 1, colum3 = 5;

    //print the sizes of the matrices
    printf("A is %d row and %d column\n", row1, colum1);
    printf("W is %d row and %d column\n", row2, colum2);
    printf("B is %d row and %d column\n", row3, colum3);
    
    //Allocate memory for matrices A, W and B
    int a[row1][colum1];
    int w[row2][colum2];
    int b[row3][colum3];
    
    //Allocate memory for matrices R
    int r[row1][colum2];
    
    //read data from the txt input files into matrice A
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < colum1; j++) {
            fscanf(fp1, "%d", &a[i][j]);
        }
    }
    fclose(fp1);

    //read data from the txt input files into matrice W
    for (int i = 0; i < row2; i++) {
        for (int j = 0; j < colum2; j++) {
            fscanf(fp2, "%d", &w[i][j]);
        }
    }
    fclose(fp2);

    //read data from the txt input files into matrice B
    for (int i = 0; i < row3; i++) {
        for (int j = 0; j < colum3; j++) {
            fscanf(fp3, "%d", &b[i][j]);
        }
    }
    fclose(fp3);

    //calculate matric mutiplication and addition(A*W+B=R)
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < colum2; j++) {
            int sum = 0;
            for (int k = 0; k < colum1; k++) {
                sum += a[i][k] * w[k][j];
            }
            r[i][j] = sum + b[i][j];
        }
    }

    //Print the result matrix R
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < colum2; j++) {
            printf("%d ", r[i][j]);
        }
        printf("\n");
    }
    return 0;
}
