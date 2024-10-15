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
//#include "sampleHelpers.h"
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

const char* main_cali = "main";
const char* comm = "comm";
const char* comp = "comp";



int main (int argc, char *argv[])
{
   int sizeOfArray; 
   int inputType;
   MPI_Status status;

   cali::ConfigManager mgr;

   mgr.start();

   CALI_MARK_BEGIN(main_cali);
   CALI_MARK_BEGIN(comp);
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
    int totalProcNum;
    int taskId;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskId);
    MPI_Comm_size(MPI_COMM_WORLD,&totalProcNum);

    //create an array of array size / proc count

    std::vector<double> array;
    //sampleHelpers::generateArray(inputType,array);
    srand(std::time(0));
    for(size_t i = 0; i  < sizeOfArray / totalProcNum; ++i){
        array.push_back((rand() % 1000) / 10.0);
    }

    //sort our local data, lets go!
    std::sort(array.begin(), array.end());
    //now we find some splitters to send to our main process we find process count - 1 splitters !!
    std::vector<double> localSplitters; 

    for(size_t i = 0; i < totalProcNum - 1; ++i){
        localSplitters.push_back(array.at(i * (array.size() / (totalProcNum - 1))));
    }
    printf("local splitters created successfully\n");

    std::vector<double> globalSplitters; 

    CALI_MARK_END(comp);
    CALI_MARK_BEGIN(comm);
    if(taskId != MASTER){
        //send our local splitters to master
        int vsize = localSplitters.size();
        MPI_Send(&localSplitters[0],vsize , MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

        //get back the global splitters
        globalSplitters.resize(totalProcNum - 1);
        MPI_Recv(&globalSplitters[0], totalProcNum - 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,&status);

    }
    if(taskId == MASTER){
        for(size_t i = 1; i < totalProcNum; ++i){
            std::vector<double> recvBufSplitters; 
            recvBufSplitters.resize(totalProcNum - 1);
            MPI_Recv(&recvBufSplitters[0], totalProcNum - 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD,&status);
            for(auto splitter : recvBufSplitters){
                localSplitters.push_back(splitter);
            }
        }   
        std::sort(localSplitters.begin(), localSplitters.end());
        for(size_t i = 0; i < totalProcNum - 1; ++i){
            globalSplitters.push_back(localSplitters.at(i * (localSplitters.size() / (totalProcNum - 1))));
        }
        for(size_t i = 1; i < totalProcNum; ++i){
            MPI_Send(&globalSplitters[0],totalProcNum - 1 , MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
    }
    CALI_MARK_END(comm);
    CALI_MARK_BEGIN(comp);

    //everyone has the global splitter now!!!!

    //put each element in correct local bucket
    std::vector<std::vector<double>> buckets;
    buckets.resize(totalProcNum);
    for(size_t i = 0; i < array.size(); ++i){
        for(size_t j = 0; j < globalSplitters.size(); ++j){
            if(array.at(i) < globalSplitters.at(j)){
                buckets.at(j).push_back(array.at(i));
            }
        }
    }
    
    CALI_MARK_END(comp);
    CALI_MARK_BEGIN(comm);

    std::vector<double> localBucket;
    for(size_t i = 0; i < totalProcNum; ++i){

        if(i == taskId){
            // if this is our own bucket copy the elements over
            for(auto ele : buckets.at(i)){
                localBucket.push_back(ele);
            }            
        }
        //else send the elements
        else{
            int localBucketSize = buckets.at(i).size();
            MPI_Send(&localBucketSize,1 , MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&buckets.at(i), localBucketSize,MPI_DOUBLE,i,0,MPI_COMM_WORLD);

            int remoteVectorSize = 0;
            MPI_Recv(&remoteVectorSize,1 , MPI_INT, i, 0, MPI_COMM_WORLD,&status);    
            std::vector<double> remoteVector;
            remoteVector.resize(remoteVectorSize);
            MPI_Recv(&remoteVector[0],remoteVectorSize , MPI_DOUBLE, i, 0, MPI_COMM_WORLD,&status);
            for(auto ele : remoteVector){
                localBucket.push_back(ele);
            }
        }
    }
    CALI_MARK_END(comm);
    CALI_MARK_BEGIN(comp);
    //each process sorts each bucket using quick sort
    std::sort(localBucket.begin(),localBucket.end());
    printf("Im task %d and my min is %f\n",taskId,localBucket.at(0));

    //sampleHelpers::checkSort(localBucket,taskId);
   CALI_MARK_END(comp);
   CALI_MARK_END(main_cali);
   adiak::init(NULL);
   adiak::launchdate();    // launch date of the job
   adiak::libraries();     // Libraries used
   adiak::cmdline();       // Command line used to launch the job
   adiak::clustername();   // Name of the cluster
   adiak::value("algorithm", "sample"); // The name of the algorithm you are using (e.g., "merge", "bitonic")
   adiak::value("programming_model", "mpi"); // e.g. "mpi"
   adiak::value("data_type", "double"); // The datatype of input elements (e.g., double, int, float)
   adiak::value("size_of_data_type", sizeof(double)); // sizeof(datatype) of input elements in bytes (e.g., 1, 2, 4)
   adiak::value("input_size", sizeOfArray); // The number of elements in input dataset (1000)
   adiak::value("input_type", inputString); // For sorting, this would be choices: ("Sorted", "ReverseSorted", "Random", "1_perc_perturbed")
   adiak::value("num_procs", numtasks); // The number of processors (MPI ranks)
   adiak::value("scalability", "strong"); // The scalability of your algorithm. choices: ("strong", "weak")
   adiak::value("group_num", 1); // The number of your group (integer, e.g., 1, 10)
   adiak::value("implementation_source", "handwritten"); // Where you got the source code of your algorithm. choices: ("online", "ai", "handwritten").

   
   mgr.stop();
   mgr.flush();

    MPI_Finalize();
}

