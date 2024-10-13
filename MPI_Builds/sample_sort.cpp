/******************************************************************************
* FILE: sample_sort.cpp
* DESCRIPTION:  
*   Sample Sort Algorithm implementation
*   This code sorts arrays using sample sort!   
* AUTHOR: Joseph Nuccitelli
* LAST REVISED: 10/13/2024
******************************************************************************/
#include "mpi.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <algorithm>
#include <ctime>


#include <caliper/cali.h> //for performance analysis
#include <caliper/cali-manager.h>
#include <adiak.hpp>
#include <vector>

#define MASTER 0          

int main (int argc, char *argv[])
{
   int sizeOfArray; 
   int inputType;
   std::string inputString;
   if (argc == 3)
   {
       sizeOfArray = atoi(argv[1]);
       inputType = atoi(argv[2]);
       if(inputType < 0 || inputType > 3) {
         printf("\n Please provide a valid input type [0-3]");
       }
   }
   else
   {
       printf("\n Please provide arguments [sizeOfArray] [input type]");
       return 0;
   }
    int arraySize = user input for array size;
    int totalProcNum;
    int taskId;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskId);
    MPI_Comm_size(MPI_COMM_WORLD,&totalProcNum);

    //create an array of array size / proc count

    std::vector<double> array;
    srand(std::time(0));
    for(size_t i = 0; i  < arraySize / procNum; ++i){
        array.push_back((rand() % 1000) / 10.0);
    }

    //sort our local data, lets go!
    std::sort(array.begin(), array.end());
    //now we find some splitters to send to our main process we find process count - 1 splitters !!
    std::vector<double> localSplitters; 

    if(taskId != MASTER){
        //send our local splitters to master
        int vsize = localSplitters.size();
        MPI_Send(&localSplitters[0],vsize , MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }


    
    MPI_Scatter(array, sizeof(array)/nbuckets, MPI_DOUBLE,procArray, sizeof(array)/nbuckets, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //quick sort the samples
    std::sort(procArray);
    MPI_Gather(all quick sorted elements);
    //select the splitters using quicksort each process will do this to avoid extra communication

    //put each element in the bucket
    for(i = 0; i < size/nbuckets; ++i){
    correctBucket.insert(procArray[i]);
    }
    //each process sorts each bucket using quick sort
    std::sort(bucket);

    //array is sorted now
}

