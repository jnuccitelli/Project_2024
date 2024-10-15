#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <caliper/cali.h>
#include <caliper/cali-manager.h>
#include <adiak.hpp>

#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */

int main (int argc, char *argv[])
{
    CALI_CXX_MARK_FUNCTION;
    int sizeOfMatrix;
    if (argc == 2)
    {
        sizeOfMatrix = atoi(argv[1]);
    }
    else
    {
        printf("\n Please provide the size of the matrix");
        return 0;
    }

    int numtasks, taskid, numworkers, source, dest, mtype, rows, averow, extra, offset, i, j, rc;
    double matrix[sizeOfMatrix][sizeOfMatrix], resultmatrix[sizeOfMatrix][sizeOfMatrix];           
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if (numtasks < 2) {
        printf("Need at least two MPI tasks. Quitting...\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        exit(1);
    }

    numworkers = numtasks - 1;

    // Create caliper ConfigManager object
    cali::ConfigManager mgr;
    mgr.start();


    /**************************** master task ************************************/
    if (taskid == MASTER)
    {
        printf("mpi_mm has started with %d tasks.\n", numtasks);
        printf("Initializing matrix...\n");

        // Initialize the matrix with reverse sorted values
        int array[sizeOfMatrix * sizeOfMatrix];
        for (i = 0; i < (sizeOfMatrix * sizeOfMatrix); i++) {
            array[i] = i;
        }

        int p = (sizeOfMatrix * sizeOfMatrix) - 1;
        for (i = 0; i < sizeOfMatrix; i++) {
            for (j = 0; j < sizeOfMatrix; j++) {
                matrix[i][j] = array[p];
                p--;
            }
        }

        // Matrix distribution to workers------ ROWS
        averow = sizeOfMatrix / numworkers;
        extra = sizeOfMatrix % numworkers;
        offset = 0;
        mtype = FROM_MASTER;

        for (dest = 1; dest <= numworkers; dest++) {
            rows = (dest <= extra) ? averow + 1 : averow;
            printf("Sending %d rows to task %d offset=%d\n", rows, dest, offset);
            MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&matrix[offset][0], rows * sizeOfMatrix, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
            offset += rows;
        }

        // Receive results from worker tasks
        mtype = FROM_WORKER;
        for (i = 1; i <= numworkers; i++) {
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&resultmatrix[offset][0], rows * sizeOfMatrix, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);
            printf("Received results from task %d\n", source);
        }


        // Print the result matrix
        printf("******************************************************\n");
        printf("Result Matrix:\n");
        for (i = 0; i < sizeOfMatrix; i++) {
            for (j = 0; j < sizeOfMatrix; j++) {
                printf("%6.2f   ", resultmatrix[i][j]);
            }
            printf("\n");
        }
        printf("******************************************************\n");


    }

    /**************************** worker task ************************************/
    if (taskid > MASTER)
    {
        int offset, rows;
        mtype = FROM_MASTER;

        // Receive matrix slice from master
        MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&matrix, rows * sizeOfMatrix, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);

        // Step 1: Sort each column of the original matrix
        for (int col = 0; col < sizeOfMatrix; col++) {
            for (int i = 0; i < rows - 1; i++) {
                for (int j = i + 1; j < rows; j++) {
                    if (matrix[i][col] > matrix[j][col]) {
                        int temp = matrix[i][col];
                        matrix[i][col] = matrix[j][col];
                        matrix[j][col] = temp;
                    }
                }
            }
        }
        
        int transposed[sizeOfMatrix][rows];  // Transposed matrix for temporary storage

        // Step 2: Transpose the matrix
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < sizeOfMatrix; j++) {
                transposed[j][i] = matrix[i][j];
            }
        }


        // Step 3: Sort each column of the transposed matrix
        for (int col = 0; col < rows; col++) {
            for (int i = 0; i < sizeOfMatrix - 1; i++) {
                for (int j = i + 1; j < sizeOfMatrix; j++) {
                    if (transposed[i][col] > transposed[j][col]) {
                        int temp = transposed[i][col];
                        transposed[i][col] = transposed[j][col];
                        transposed[j][col] = temp;
                    }
                }
            }
        }

        // Step 4: Transpose back to the original form
        for (int i = 0; i < sizeOfMatrix; i++) {
            for (int j = 0; j < rows; j++) {
                matrix[j][i] = transposed[i][j];
            }
        }

        // Step 5: Sort each column again in the original matrix
        for (int col = 0; col < sizeOfMatrix; col++) {
            for (int i = 0; i < rows - 1; i++) {
                for (int j = i + 1; j < rows; j++) {
                    if (matrix[i][col] > matrix[j][col]) {
                        int temp = matrix[i][col];
                        matrix[i][col] = matrix[j][col];
                        matrix[j][col] = temp;
                    }
                }
            }
        }
        
        double max = -1;
        int flag = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < sizeOfMatrix; j++) {
                if (matrix[i][j] < max) {
                    printf("Error: the result matrix is not sorted\n");
                    flag = 1;
                }
                max = resultmatrix[i][j];
            }
        }
        if (flag == 0 && rows > 0) {
            printf("The result matrix is sorted\n");
        }

            // Send results back to the master
            mtype = FROM_WORKER;
            MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
            MPI_Send(&matrix, rows * sizeOfMatrix, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
        }

    // WHOLE PROGRAM COMPUTATION PART ENDS HERE

    // Finalize Caliper and MPI
    mgr.stop();
    mgr.flush();
    MPI_Finalize();

    return 0;
}
