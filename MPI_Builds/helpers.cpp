/******************************************************************************
* FILE: helpers.cpp
* DESCRIPTION:  
*   Helper functions for sorting algorithms
* AUTHOR: Group 1
* LAST REVISED: 10/04/2024
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctime>
#include "helpers.h"

#define RANDOM_INPUT 0
#define SORTED_INPUT 1
#define REVERSE_SORTED_INPUT 2
#define ONE_PERC_PERMUTED 3

double* generateArray(int size, int inputType) {
   srand(std::time(0)); //give rand a random seed so we get different answers each time
   double* arr = new double[size];
   if(inputType == RANDOM_INPUT) {
      for(int i = 0; i < size; i++) {
         arr[i] = (rand() % 1000) / 10.0; //generates doubles from 0 to 100
      }
      return arr;
   } else if(inputType == SORTED_INPUT) {
      double lastVal = 0;
      for(int i = 0; i < size; i++) {
         double newVal = ((rand() % 100) / 10.0) + lastVal;
         arr[i] = newVal; //generates doubles from the last value to 10+last value
         lastVal = newVal;
      }
      return arr;
   } else if(inputType == REVERSE_SORTED_INPUT) {
      double lastVal = 0;
      for(int i = size-1; i >= 0; i--) {
         double newVal = ((rand() % 100) / 10.0) + lastVal;
         arr[i] = newVal; //generates doubles from the last value to 10+last value
         lastVal = newVal;
      }
      return arr;
   } else { //one percent permuted
      double lastVal = 0;
      for(int i = 0; i < size; i++) {
         double newVal = ((rand() % 100) / 10.0) + lastVal;
         arr[i] = newVal; //generates doubles from the last value to 10+last value
         lastVal = newVal;
      } //generates sorted array

      int numToPermute = (size/100) + 1; //about 1% of the elements of our array, at least 0
      for(int p = 0; p < numToPermute; p++) {
         int indx1 = rand() % size;
         int indx2 = rand() % size;
         while(indx1 == indx2) { //make sure we have 2 different indicies
            indx2 = rand() % size;
         }
         double oldVal = arr[indx1];
         arr[indx1] = arr[indx2];
         arr[indx2] = oldVal;
      }
      return arr;
   }
}

void printArray(double* arr, int size) {
   for(int i = 0; i < size; i++) {
      printf("%.2f ", arr[i]);
   }
   printf("\n");
}
