/******************************************************************************
* FILE: helpers.h
* DESCRIPTION:
*   Helper functions for sorting algorithms
* AUTHOR: Group 1
* LAST REVISED: 10/04/2024
******************************************************************************/
#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define RANDOM_INPUT 0
#define SORTED_INPUT 1
#define REVERSE_SORTED_INPUT 2
#define ONE_PERC_PERMUTED 3

double* generateArray(int size, int inputType);

void printArray(double* arr, int size);

#endif
