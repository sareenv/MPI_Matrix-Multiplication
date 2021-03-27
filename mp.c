#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<string.h>

// constants - global variables.
const int nrows = 2;
const int ncols = 2;


// utlity functions to generate the zero matrix
void generateZeroMatrix(int matrix[nrows][ncols]) {
    for(int i = 0; i< nrows; i++) {
        for(int j = 0; j < ncols; j++) {
            matrix[i][j] = 0; // all the elements are zero
        }
    }
    return;
}


// utlity functions to generate the rand matrix
void generateMatrix(int matrix[nrows][ncols]) {
    for(int i = 0; i< nrows; i++) {
        for(int j = 0; j < ncols; j++) {
            matrix[i][j] = rand() % 20; // fixed range
        }
    }
    return;
}

// utlity function to print matrix.
void printMatrix(int matrix[nrows][ncols]) {
    for(int i = 0; i< nrows; i++) {
        for(int j = 0; j < ncols; j++) {
            printf("%d \t", matrix[i][j]);
        }
        printf("\n");
    }
}

// Utility function to multiply the matrix.
/*
    References: 
    1. "https://en.wikipedia.org/wiki/Matrix_multiplication_algorithm"
    2. https://www.mathsisfun.com/algebra/matrix-multiplying.html
*/
void multiply(int m1[nrows][ncols], int m2[nrows][ncols], int result[nrows][ncols]) { 
    // no of rows of m1.
    for(int i = 0; i< nrows; i++) {
        // cols of m2
        for(int j = 0; j< ncols; j++) {
            // rows of m2
            for(int k = 0; k< nrows; k++) {
                /* submission part.
                    c_{{ij}}=\sum _{{k=1}}^{m}a_{{ik}}b_{{kj}}.
                */
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }  
}

// utility function to send the matrix rows.
void sendMatrixRows(int matrix[nrows][ncols], MPI_Comm comm, MPI_Status status, int fsrc) {
  int isStopped = 0;
  int i = 0;
  int matrixrow[nrows];
  int j = 0;
  while(isStopped != 1 && j < nrows) {
    
      MPI_Recv( &i , 1 , MPI_INT , fsrc, 0 , comm , &status);
      if(isStopped == 1) {
          printf("Break sending any data");
          break; 
        }
      MPI_Send(matrix[j] , nrows , MPI_INT, fsrc , 0 , comm);
      j+=1;
  }
}

//p2.
// utility function to recieve and reconstruct the matrix.
void recvMatrixRow(MPI_Comm comm, MPI_Status status,
     int resultMatrix[nrows][ncols], 
     int fdes) {
    int i = 0;
    int matrixrows[nrows];
    int running = 0;
    int stopped = 1;
    while (i < nrows)
    {
        MPI_Send(&running , 1 , MPI_INT , fdes , 0 , comm);
        MPI_Recv( matrixrows, nrows, MPI_INT , fdes , 0 , comm , &status);    
        for(int j = 0; j< nrows; j++) { resultMatrix[i][j] = matrixrows[j]; }
        i += 1;
    }
    MPI_Send( &stopped , 1 , MPI_INT, fdes, 0 , comm);
}


int main() {

    // initialise the mpi
    MPI_Init(NULL , NULL);
    int size; // size of the processes inside the communicator.
    int rank; // rank of processes;    
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Status status;
    double time1, time2;

   int matrixa[nrows][ncols];
   int matrixb[nrows][ncols];
   int result[nrows][ncols];
   MPI_Comm_rank( comm , &rank); // mpi routine call for ranking the process in the communicator. 


    // master process only generates the matrix
    if(rank == 0) {
        time1 = MPI_Wtime();
        generateMatrix(matrixa);
        generateMatrix(matrixb);
        generateZeroMatrix(result);
        sendMatrixRows(matrixa, comm, status, 1);
        sendMatrixRows(matrixb, comm, status, 1);
        sendMatrixRows(result, comm, status, 1);
    }

    // prints the matrix.
    else if(rank == 1) {
        int matrixa[nrows][ncols];
        int matrixb[nrows][ncols];
        int resultMatrix[nrows][ncols];
        printf("Printing the matrix a \n");
        recvMatrixRow(comm, status, matrixa, 0);
        printMatrix(matrixa);

        printf("Printing the matrix b\n");
        recvMatrixRow(comm, status, matrixb, 0);
        printMatrix(matrixb);

        printf("Printing the initial result matrix\n");
        recvMatrixRow(comm, status, resultMatrix, 0);
        printMatrix(resultMatrix);

        // compute the matrix. 
        multiply(matrixa, matrixb, resultMatrix);
        // send the result for printing to another process using blocking call;
        sendMatrixRows(resultMatrix, comm, status, 2);
    } else {
        // all the other process with rank other than 0 or 1 will print the results.
       int resultMatrix[nrows][ncols];
       recvMatrixRow(comm, status, resultMatrix, 1);
       printf("Printing the result \n");
       printMatrix(resultMatrix);
       time2 = MPI_Wtime();
       printf("Final time was %f", time2 - time1);
    }

    MPI_Finalize(); // final call for mpi.
    return 0;
}