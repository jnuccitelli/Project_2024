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

    for(size_t i = 0; i < totalProcNum - 1; ++i){
        localSplitters.push_back(array.at(i * (array.size() / (totalProcNum - 1))));
    }

    if(taskId != MASTER){
        //send our local splitters to master
        int vsize = localSplitters.size();
        MPI_Send(&localSplitters[0],vsize , MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    if(taskId == MASTER){

        for(size_t i = 1; i < totalProcNum; ++i){
            std::vector<double> recvBufSplitters; 
            recvBufSplitters.resize(totalProcNum - 1);
            MPI_Recv(&recvBufSplitters, totalProcNum - 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            for(auto splitter : recvBufSplitters){
                localSplitters.push_back(splitter);
            }
        }   


        std::sort(localSplitters.begin(), localSplitters.end());

        std::vector<double> globalSplitters; 

        for(size_t i = 0; i < totalProcNum - 1; ++i){
            globalSplitters.push_back(localSplitters.at(i * (localSplitters.size() / (totalProcNum - 1))));
        }

        printf("Global Splitters made total amount is %d\n",globalSplitters.size());

        for(auto globalSplit : globalSplitters){
            printf("%d,",globalSplit);
        }
        printf("\n");



    }

    
    //each process sorts each bucket using quick sort


    //array is sorted now
}

