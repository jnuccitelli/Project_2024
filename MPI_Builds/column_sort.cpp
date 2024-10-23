#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <vector>
#include <algorithm>

#include <caliper/cali.h>
#include <caliper/cali-manager.h>
#include <adiak.hpp>


#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          

#define ll long long

using namespace std;

vector<vector<ll>> custom_transpose(const vector<vector<ll>> &matrix) {
    int r = matrix.size();  // number of rows
    int c = matrix[0].size();  // number of columns

    // Transpose matrix (standard row to column swap)
    vector<vector<ll>> transposed(c, vector<ll>(r, 0));
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            transposed[j][i] = matrix[i][j];
        }
    }

    // Step 2: Reorganize the transposed matrix into the new form
    // Now reorganizing into a new matrix (same as the transposed matrix structure).
    vector<vector<ll>> final_result(r, vector<ll>(c, 0));  // Output matrix same dimensions as input

    // Track number of elements we've inserted into the new matrix
    int idx = 0;
    
    // Flatten the transposed matrix into a temporary array for easier rearrangement
    vector<ll> flattened;
    for (int i = 0; i < c; i++) {
        for (int j = 0; j < r; j++) {
            flattened.push_back(transposed[i][j]);
        }
    }

    // Now fill the result matrix row-wise in the desired format
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            final_result[i][j] = flattened[idx++];
        }
    }

    return final_result;
}

vector<vector<ll>> reverse_transpose(const vector<vector<ll>> &matrix) {
    int r = matrix.size();  // number of rows
    int c = matrix[0].size();  // number of columns
    vector<vector<ll>> result(r, vector<ll>(c, 0));  // Create a matrix of the same size
    
    // Step 1: Flatten the matrix into a 1D array
    vector<ll> flattened;
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            flattened.push_back(matrix[i][j]);
        }
    }

    // Step 2: Rearrange the flattened elements back to the transposed matrix
    int idx = 0;
    for (int j = 0; j < c; j++) {  // Column-wise filling
        for (int i = 0; i < r; i++) {
            result[i][j] = flattened[idx++];
        }
    }

    return result;
}


vector<vector<ll>> shift(vector<vector<ll>> &matrix) {
    int r = matrix.size();  
    int c = matrix[0].size();  

    // Highest value ever in cpp int max?
    ll padding_value = (r*c)+3;

    int rows_half_down = r / 2;  // Number of rows to be padded with -99
    int rows_half_up = r - rows_half_down;  // The remaining rows will have the original values with padding

    vector<vector<ll>> result(r, vector<ll>(c + 1, 0));  // Create the result matrix with one extra column

    // First part: Add -99 in the first column for the first `rows_half_down` rows
    for (int i = 0; i < rows_half_down; i++) {
        result[i][0] = -99;  // First element of the row is -99
        // Copy the rest of the values from the original matrix's second half
        for (int j = 1; j < c + 1; j++) {
            result[i][j] = matrix[rows_half_up + i][j - 1];  // Shifted values from the second half of the original matrix
        }
    }

    // Second part: Add the original values in the second half of the matrix and add padding_value (30)
    for (int i = rows_half_down; i < r; i++) {
        for (int j = 0; j < c; j++) {
            result[i][j] = matrix[i - rows_half_down][j];  // Copy original values from the top half of the matrix
        }
        result[i][c] = padding_value;  // Add padding value 30 in the last column
    }

    return result;
}


vector<vector<ll>> unshift(const vector<vector<long long>>& transformed_matrix, int colOriginal) {
    vector<long long> filtered_elements;
    
    int r = transformed_matrix.size();       
    int c = transformed_matrix[0].size();    

    // printf("Column original: %d\n", colOriginal);
    // printf("C: %d\n", c);
    // Step 1: Traverse each column, then each row within that column (column-wise traversal)
    for (int col = 0; col < c; col++) {
        for (int row = 0; row < r; row++) {
            long long element = transformed_matrix[row][col];
            // Step 2: Push back elements that are not -99 or 30
            if (element != -99 && element != (r*colOriginal)+3) {
                filtered_elements.push_back(element);
            }
        }
    }

    // Step 3: Reshape the filtered elements into a rxc matrix
    vector<vector<long long>> reshaped_matrix(r, vector<long long>(colOriginal));
    int index = 0;

    for (int j = 0; j < colOriginal; ++j) {
        for (int i = 0; i < r; ++i) {
            reshaped_matrix[i][j] = filtered_elements[index++];
        }
    }

    return reshaped_matrix;
}

// Function to sort columns of the matrix
void sort_columns(vector<vector<ll>> &matrix) {
    int r = matrix.size();
    int c = matrix[0].size();

    // Traverse each column
    for (int col = 0; col < c; col++) {
        // Create a temporary vector to store the column values
        vector<ll> column(r);
        for (int row = 0; row < r; row++) {
            column[row] = matrix[row][col];  // Extract the column elements
        }

        // Sort the column
        sort(column.begin(), column.end());

        // Put the sorted column back into the matrix
        for (int row = 0; row < r; row++) {
            matrix[row][col] = column[row];  // Replace the original column with the sorted values
        }
    }
}

// Correctness check -- to see if the final matrix is sorted
bool is_sorted(const vector<vector<ll>> &matrix) {
    int r = matrix.size();
    int c = matrix[0].size();
    for (int col = 0; col < c; col++) {
        for (int row = 0; row < r - 1; row++) {
            if (matrix[row][col] > matrix[row + 1][col]) {
                return false;
            }
        }
    }

    return true;
}

// Generate Array function with all positive values
vector<vector<ll>> generate_matrix(int sizeOfMatrix, int input_type) {
    int r = sizeOfMatrix;
    int c = 256;
    vector<vector<ll>> matrix(sizeOfMatrix, vector<ll>(c));

    if (input_type == 0) {
        // Sorted
        int count = 1;
        for (int j = 0; j < c; j++) {
            for (int i = 0; i < r; i++) {
                matrix[i][j] = count++;
            }
        }
    } else if (input_type == 1) {
        // Random: fill the matrix with random values from 0 to (r*c)
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                matrix[i][j] = rand() % (r * c);
            }
        }
    } else if (input_type == 2) {
        // Reverse: fill the matrix with random values from 0 to (r*c) in reverse order
        int count = r * c;
        for (int j = 0; j < c; j++) {
            for (int i = 0; i < r; i++) {
                matrix[i][j] = count--;
            }
        }
    } else if (input_type == 3) {
        // 1% perturbed: fill the matrix with values from 1 to (r*c) and perturb 1% of the values
        int count = 1;
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                matrix[i][j] = count++;
            }
        }

        // Perturb 1% of the values
        int num_perturbed = (r * c) / 100;
        for (int i = 0; i < num_perturbed; i++) {
            int row = rand() % r;
            int col = rand() % c;
            matrix[row][col] = rand() % (r * c);
        }
    }

    return matrix;
}



int main(int argc, char *argv[])
{

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
    
    // Create caliper ConfigManager object
    cali::ConfigManager mgr;
    mgr.start();

    int sizeOfMatrix;
    int input_type = 0;
    const char* input_type_str = "";
    int columnNum = 256;
    if (argc == 2) {
        sizeOfMatrix = atoi(argv[1]);
    } else {
        printf("\n Please provide the size of the matrix");
        return 0;
    }

    // Based on the input type assign the string
    if (input_type == 0) {
        input_type_str = "Sorted";
    } else if (input_type == 1) {
        input_type_str = "Random";
    } else if (input_type == 2) {
        input_type_str = "Reverse";
    } else if (input_type == 3) {
        input_type_str = "1% Perturbed";
    }

    int numtasks, taskid, numworkers, source, dest, mtype, cols, avecol, extra, offset, i, j, rc;
    MPI_Status status;
    vector<vector<ll>> matrix;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if (numtasks < 2) {
        printf("Need at least two MPI tasks. Quitting...\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        exit(1);
    }

    numworkers = numtasks - 1;

    // Initialize the matrix with the type of input
    // Sorted = 0
    // Random = 1
    // Reverse = 2
    // 1% perturbed = 3 

    CALI_MARK_BEGIN(main_cali);

    CALI_MARK_BEGIN(data_init_runtime);
    matrix = generate_matrix(sizeOfMatrix, input_type);
    CALI_MARK_END(data_init_runtime);

    if (taskid == MASTER) {
        printf("Program started with %d tasks.\n", numtasks);

        CALI_MARK_BEGIN(comm);
        CALI_MARK_BEGIN(comm_small);

        int ncols = matrix[0].size();
        avecol = ncols / numworkers;
        extra = ncols % numworkers;
        offset = 0;
        mtype = FROM_MASTER;

        for (dest = 1; dest <= numworkers; dest++) {
            cols = (dest <= extra) ? avecol + 1 : avecol;
            MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&cols, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            for (int row = 0; row < sizeOfMatrix; row++) {
                MPI_Send(&matrix[row][offset], cols, MPI_LONG_LONG, dest, mtype, MPI_COMM_WORLD);
            }
            offset += cols;
        }


        // Receive sorted columns from workers
        mtype = FROM_WORKER;
        offset = 0;
        for (int source = 1; source <= numworkers; source++) {
            MPI_Recv(&cols, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            for (int row = 0; row < sizeOfMatrix; row++) {
                MPI_Recv(&matrix[row][offset], cols, MPI_LONG_LONG, source, mtype, MPI_COMM_WORLD, &status);
            }
            offset += cols;
        }

        CALI_MARK_END(comm_small);
        CALI_MARK_END(comm);

        // Sequential computation
        CALI_MARK_BEGIN(comp);
        CALI_MARK_BEGIN(comp_large);

        
        vector<vector<ll>> transposed_matrix = custom_transpose(matrix);
        sort_columns(transposed_matrix);
        matrix = reverse_transpose(transposed_matrix);
        sort_columns(matrix);
        matrix = shift(matrix);
        sort_columns(matrix);
        matrix = unshift(matrix, ncols);

        CALI_MARK_END(comp_large);
        CALI_MARK_END(comp);

        // Correctness check
        CALI_MARK_BEGIN(correctness_check);
        
        if (is_sorted(matrix)) {
            printf("The result matrix is sorted\n");
        } else {
            printf("Error: the result matrix is not sorted\n");
        }
        
        CALI_MARK_END(correctness_check);

        // // Print the sorted matrix
        // printf("******************************************************\n");
        // printf("Sorted Matrix Final:\n");
        // for (i = 0; i < sizeOfMatrix; i++) {
        //     for (j = 0; j < matrix[i].size(); j++) {
        //         printf("%lld   ", matrix[i][j]);
        //     }
        //     printf("\n");
        // }
        // printf("******************************************************\n");

    } else { // Worker processes
        int offset, cols;
        mtype = FROM_MASTER;

        CALI_MARK_BEGIN(comm);
        CALI_MARK_BEGIN(comm_small);
        MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&cols, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);

        vector<vector<ll>> local_matrix(sizeOfMatrix, vector<ll>(cols));
        for (int row = 0; row < sizeOfMatrix; row++) {
            MPI_Recv(&local_matrix[row][0], cols, MPI_LONG_LONG, MASTER, mtype, MPI_COMM_WORLD, &status);
        }

        CALI_MARK_END(comm_small);
        CALI_MARK_END(comm);

        CALI_MARK_BEGIN(comp);
        CALI_MARK_BEGIN(comp_small);
        // Sort columns in the received slice
        sort_columns(local_matrix);

        CALI_MARK_END(comp_small);
        CALI_MARK_END(comp);
        
        CALI_MARK_BEGIN(comm);
        CALI_MARK_BEGIN(comm_small);
        // Send the sorted columns back to the master
        mtype = FROM_WORKER;
        MPI_Send(&cols, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        for (int row = 0; row < sizeOfMatrix; row++) {
            MPI_Send(&local_matrix[row][0], cols, MPI_LONG_LONG, MASTER, mtype, MPI_COMM_WORLD);
        }
        CALI_MARK_END(comm_small);
        CALI_MARK_END(comm);
    }

    CALI_MARK_END(main_cali);

    // WHOLE PROGRAM COMPUTATION PART ENDS HERE

    adiak::init(NULL);
    adiak::launchdate();    // launch date of the job
    adiak::libraries();     // Libraries used
    adiak::cmdline();       // Command line used to launch the job
    adiak::clustername();   // Name of the cluster
    adiak::value("algorithm", "column"); // The name of the algorithm you are using (e.g., "merge", "bitonic")
    adiak::value("programming_model", "mpi"); // e.g. "mpi"
    adiak::value("data_type", "int"); // The datatype of input elements (e.g., double, int, float)
    adiak::value("size_of_data_type", sizeof(int)); // sizeof(datatype) of input elements in bytes (e.g., 1, 2, 4)
    adiak::value("input_size", (sizeOfMatrix*columnNum)); // The number of elements in input dataset (1000)
    adiak::value("input_type", input_type_str); // For sorting, this would be choices: ("Sorted", "ReverseSorted", "Random", "1_perc_perturbed")
    adiak::value("num_procs", numtasks); // The number of processors (MPI ranks)
    adiak::value("scalability", "weak"); // The scalability of your algorithm. choices: ("strong", "weak")
    adiak::value("group_num", 1); // The number of your group (integer, e.g., 1, 10)
    adiak::value("implementation_source", "handwritten"); // Where you got the source code of your algorithm. choices: ("online", "ai", "handwritten").

   
   // Flush Caliper output before finalizing MPI
   mgr.stop();
   mgr.flush();
   
   MPI_Finalize();
   return 0;
}