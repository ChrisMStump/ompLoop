#include <omp.h>
#include <chrono>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);
  
#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {
  float sum = 0;
  char sync[10];
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
  
  if (argc < 5) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  int * arr = new int [atoi(argv[1])];
  strcpy(sync, argv[3]);

  generateReduceData (arr, atoi(argv[1]));
  omp_set_num_threads(atoi(argv[2]));
  
  if(strcmp(sync, "static") == 0){
    omp_set_schedule(omp_sched_static, atoi(argv[4]));
  } else if(strcmp(sync, "dynamic") == 0){
    omp_set_schedule(omp_sched_dynamic, atoi(argv[4]));
  } else {
    omp_set_schedule(omp_sched_guided, atoi(argv[4]));
  }
  
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  #pragma omp parallel for reduction(+:sum)
      for(int i = 0; i < atoi(argv[1]); i++){
        sum += arr[i];        
      }
  
  delete[] arr;

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cout<<sum<<std::endl;
  
  std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}
