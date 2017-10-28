#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <string.h>	

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

float sequential_integrate (int argc, char* argv[]);

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
  
  if (argc < 9) {
    std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }
    
  float integrate = sequential_integrate(argc, argv);

  int intensity = atoi(argv[5]);
  int choice = atoi(argv[1]);
  float a = atof(argv[2]);
  float b = atof(argv[3]);
  float n = atof(argv[4]);
  float x = (b-a)/n;
  float result = 0;
  char sync[10];

  strcpy(sync, argv[7]);
  omp_set_num_threads(atoi(argv[6]));
  if(strcmp(sync, "static") == 0){
    omp_set_schedule(omp_sched_static, atoi(argv[8]));
  } else if(strcmp(sync, "dynamic") == 0){
    omp_set_schedule(omp_sched_dynamic, atoi(argv[8]));
  } else {
    omp_set_schedule(omp_sched_guided, atoi(argv[8]));
  }

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  switch (choice) {
    case 1:
      #pragma omp parallel for reduction(+:result)
      for(int i = 0; i < atoi(argv[4]); i++){
        result += f1(a+((i+.5)*x), intensity) * x;
      }
      break;
    case 2:
      #pragma omp parallel for reduction(+:result)
      for(int i = 0; i < atoi(argv[4]); i++){
        result += f2(a+((i+.5)*x), intensity) * x;
      }
      break;
    case 3:
      #pragma omp parallel for reduction(+:result)
      for(int i = 0; i < atoi(argv[4]); i++){
        result += f3(a+((i+.5)*x), intensity) * x;
      }
      break;
    case 4:
      #pragma omp parallel for reduction(+:result)
      for(int i = 0; i < atoi(argv[4]); i++){
        result += f4(a+((i+.5)*x), intensity) * x;
      }
      break;
  }
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cout<<integrate<<std::endl;
  
  std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}
