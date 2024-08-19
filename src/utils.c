#include "utils.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>


int count_lines(char *str){
	int lines = 0;
	for (int i = 0; str[i]; i++){
		if (str[i] == '\n') lines++;
	}
	return lines;
}


void bubbleSort(double *arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap arr[j] and arr[j + 1]
                double temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}
// Function to find the median of the array
double findMedian(double *arr, int size) {
    // Create a copy of the array to sort
    double *sortedArr = (double *)malloc(size * sizeof(double));
    memcpy(sortedArr, arr, size * sizeof(double));
    
    // Sort the array using Bubble Sort
    bubbleSort(sortedArr, size);
    
    // Calculate the median
    double median;
    if (size % 2 == 0) {
        median = (sortedArr[size / 2 - 1] + sortedArr[size / 2]) / 2.0;
    } else {
        median = sortedArr[size / 2];
    }
    
    // Free the allocated memory
    free(sortedArr);
    //printf("this is the median: \n%f\n\n", median);
    return median;
}

