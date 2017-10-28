#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <chrono>


#ifdef __cplusplus
extern "C" {
#endif

  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
  
  
#ifdef __cplusplus
}
#endif


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
  int * temp = new int [atoi(argv[1])];
  int * pr = new int [atoi(argv[1])+1];
  int logNum = ceil(log(atoi(argv[1]))); //Used to determine how many interations we need.
  
  omp_set_num_threads(atoi(argv[2])); //Set the number of threads

  generatePrefixSumData (arr, atoi(argv[1]));

  for(int i = 0; i < atoi(argv[1]); i++){ //Initial temp array so the threads do not interfere.
   temp[i] = arr[i];
  }
  
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  for(int j = 1; j < logNum + 1; j ++){ //Iterate through the array logN times. This ensures that we will account for every value.
    #pragma omp parallel for schedule(static)
    for(int i = 0; i < atoi(argv[1])+1; i++){ //For every value in the array
      if(i >= pow(2, j-1)){ //If it hasn't been fully accounted for
        arr[i] = ((temp[i -((int)pow(2, j-1))]) + temp[i]); //Add the value which it has not been accounted for.
      }
    }
    #pragma omp parallel for
    for(int i = 0; i < atoi(argv[1]); i++){ //We need a temp array so that the threads do not interfere with each other.
     temp[i] = arr[i];
    }
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cerr<<elapsed_seconds.count()<<std::endl;

  pr[0] =0;
  #pragma omp parallel for
  for(int i = 0; i < atoi(argv[1]); i++){
   pr[i+1] = arr[i];
  } //Store the values into pr

  checkPrefixSumResult(pr, atoi(argv[1]));
  
  delete[] arr;
  delete[] pr;

  return 0;
}
