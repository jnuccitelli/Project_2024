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

    /* Define Caliper region names */
    const char* main_cali = "main";
    const char* data_init_runtime = "data_init_runtime";
    const char* correctness_check = "correctness_check";
    const char* comm = "comm";
    const char* comm_small = "comm_small";
    const char* comm_large = "comm_large";
    const char* comp = "comp";
    const char* comp_small = "comp_small";
    const char* comp_large = "comp_large";

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

    CALI_MARK_BEGIN(main_cali);
    /**************************** master task ************************************/
    if (taskid == MASTER)
    {
        printf("mpi_mm has started with %d tasks.\n", numtasks);
        printf("Initializing matrix...\n");

        CALI_MARK_BEGIN(data_init_runtime);
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
        CALI_MARK_BEGIN(data_init_runtime);

        CALI_MARK_BEGIN(comm);
        CALI_MARK_BEGIN(comm_small);
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

        CALI_MARK_END(comm_small);
        CALI_MARK_END(comm);
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

        CALI_MARK_BEGIN(comm);
        CALI_MARK_BEGIN(comm_large);
        // Receive matrix slice from master
        MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&matrix, rows * sizeOfMatrix, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);

       CALI_MARK_END(comm_large);
       CALI_MARK_END(comm);

        CALI_MARK_BEGIN(comp);
        CALI_MARK_BEGIN(comp_large);
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
        
        CALI_MARK_END(comp_large);
        CALI_MARK_BEGIN(comp_small);

        int transposed[sizeOfMatrix][rows];  // Transposed matrix for temporary storage

        // Step 2: Transpose the matrix
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < sizeOfMatrix; j++) {
                transposed[j][i] = matrix[i][j];
            }
        }
            
        CALI_MARK_END(comp_small);
        CALI_MARK_BEGIN(comp_large);


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

        CALI_MARK_END(comp_large);
        CALI_MARK_BEGIN(comp_small);

        // Step 4: Transpose back to the original form
        for (int i = 0; i < sizeOfMatrix; i++) {
            for (int j = 0; j < rows; j++) {
                matrix[j][i] = transposed[i][j];
            }
        }

        CALI_MARK_END(comp_small);
        CALI_MARK_BEGIN(comp_large);

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

        // Step 6: Transpose back to the original form
        for (int i = 0; i < sizeOfMatrix; i++) {
            for (int j = 0; j < rows; j++) {
                matrix[j][i] = transposed[i][j];
            }
        }
        
    CALI_MARK_END(comp_large);
    CALI_MARK_END(comp);
    CALI_MARK_BEGIN(correctness_check);

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
    CALI_MARK_END(correctness_check);
    CALI_MARK_BEGIN(comm);
    CALI_MARK_BEGIN(comm_large);

            // Send results back to the master
            mtype = FROM_WORKER;
            MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
            MPI_Send(&matrix, rows * sizeOfMatrix, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);

    CALI_MARK_END(comm_large);
    CALI_MARK_END(comm);
    // WHOLE PROGRAM COMPUTATION PART ENDS HERE
    CALI_MARK_END(main_cali);
    // Finalize Caliper and MPI

    adiak::init(NULL);
    adiak::launchdate();    // launch date of the job
    adiak::libraries();     // Libraries used
    adiak::cmdline();       // Command line used to launch the job
    adiak::clustername();   // Name of the cluster
    adiak::value("algorithm", "column"); // The name of the algorithm you are using (e.g., "merge", "bitonic")
    adiak::value("programming_model", "mpi"); // e.g. "mpi"
    adiak::value("data_type", "double"); // The datatype of input elements (e.g., double, int, float)
    adiak::value("size_of_data_type", sizeof(double)); // sizeof(datatype) of input elements in bytes (e.g., 1, 2, 4)
    adiak::value("input_size", sizeOfMatrix); // The number of elements in input dataset (1000)
    adiak::value("input_type", "ReverseSorted"); // For sorting, this would be choices: ("Sorted", "ReverseSorted", "Random", "1_perc_perturbed")
    adiak::value("num_procs", numtasks); // The number of processors (MPI ranks)
    adiak::value("scalability", "weak"); // The scalability of your algorithm. choices: ("strong", "weak")
    adiak::value("group_num", 1); // The number of your group (integer, e.g., 1, 10)
    adiak::value("implementation_source", "handwritten"); // Where you got the source code of your algorithm. choices: ("online", "ai", "handwritten").
    
    mgr.stop();
    mgr.flush();
    MPI_Finalize();

    return 0;
}