#include "stdio.h"
#include "omp.h"

#define THREADS_NUMBER 6
#define NUMBER 1000000000

long sum = 0;

long getSum(long start, long end)
{
  long sum = 0;
  for (long i = start; i < end; i++)
  {
    sum += i;
  }

  return sum;
}

void parallelSetSum()
{
  long r = NUMBER / THREADS_NUMBER;
#pragma omp parallel
  {
    double initialTime = omp_get_wtime();
    int id = omp_get_thread_num();
    long ratio = r * id;
    long partSum = getSum(ratio, ratio + r);
#pragma omp critical
    {
      sum += partSum;
    }
    printf("\nthreadID: %d\n", id);
    double finalTime = omp_get_wtime();
    printf("Time: %f\n", (finalTime - initialTime));
  }
}

int main()
{
  printf("\nStart of program!\n");

  // omp_set_num_threads(THREADS_NUMBER);
  // parallelSetSum();

  // int i;
  // #pragma omp parallel for 
  //   for (i = 0; i < THREADS_NUMBER; i++) {
  //     printf("\ni = %d\n", i);
  //     int id = omp_get_thread_num();
  //     printf("threadID: %d\n", id);
  //   }
  int nrOfProcessors = omp_get_num_procs();
  printf("nrOfPRocessors: %d", nrOfProcessors);
  // printf("sum = %ld", sum);
  printf("\nEnd of program!\n");

  return 0;
}
