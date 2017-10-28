#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <math.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

  
#ifdef __cplusplus
}
#endif

void merge(int array[], int low, int mid, int high){
  int temp[high+1]; //Create a temporary array to hold data.
  int i, j, k, m;
  j = low;
  m = mid + 1;

  //Merge the two halves
  for(i = low; j <= mid && m <= high; i++){
    if(array[j] <= array[m]){
      temp[i] = array[j];
      j++;
    } else {
      temp[i] = array[m];
      m++;
    }
  }
  if(j > mid){
    //Copy the elements of the right side.
    for(k = m; k <= high; k++){
      temp[i] = array[k];
      i++;
    }
  } else {
    //Copy the elements of the left side.
    for(k = j; k <= mid; k++){
      temp[i] = array[k];
      i++;
    }
  }
  //Copy the elements back into array.
  for(k = low; k <= high; k++){
    array[k] = temp[k];
  }
}

void mergesort(int array[], int low, int high){
  int mid;
  if(low < high){ //Is there more than one value in the array?
    mid = (low + high) / 2; //If so, then we need to find the midpoint of the array.

    //The program recursively calls itself and breaks the array into individual pieces.
    #pragma omp parallel sections
    { //We are calling sections to distribute the tasks between the threads.
      #pragma omp section
      {
        mergesort(array, low, mid); //Break down the first part of the array and or array segment.
      } //Eventually the program will merge together this half.
      #pragma omp section
      {
        mergesort(array, mid+1, high); //Break down the second part of the array and or array segment.
      } //Eventually the program will merge together this half.
    }
    merge(array, low, mid, high); //Merge the two halves together.
  }
}


int main (int argc, char* argv[]) {

  //forces openmp to create the threads beforehand
#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int * arr = new int [atoi(argv[1])];

  omp_set_num_threads(atoi(argv[2]));

  generateMergeSortData (arr, atoi(argv[1]));
  
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  mergesort(arr, 0, atoi(argv[1])); //This is the call to initiate mergesort.
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cerr<<elapsed_seconds.count()<<std::endl;

  checkMergeSortResult (arr, atoi(argv[1]));
  
  delete[] arr;

  return 0;
}
