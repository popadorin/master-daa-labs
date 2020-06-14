#include "stdio.h"
#include "omp.h"

#define THREADS_NUMBER 6

long getSum(long start, long end) {
  long sum = 0;
  for (long i = start; i < end; i++)
  {
    sum += i;
  }

  return sum;
}

int main() {
  printf("\nStart of program!\n");
  long number = 10000000000;
  long r = number / THREADS_NUMBER;
  long A[THREADS_NUMBER];
  omp_set_num_threads(THREADS_NUMBER);

  #pragma omp parallel 
  {
    int id = omp_get_thread_num();
    long ratio = r * id;
    A[id] = getSum(ratio, ratio + r);
    printf("\nthreadID: %d\n", id);
  }

  long sum = 0;

  for (int i = 0; i < THREADS_NUMBER; i++) {
    sum += A[i];
  }

  // printf("\nProgram exit!\n");
  // double initialTime = omp_get_wtime();
  // double pi = getPi();
  // double finalTime = omp_get_wtime();
  // printf("Time: %f", (finalTime - initialTime));
  // printf("\nPI = %f\n\n", pi);
  
  // long sum = getSum(0, number);
  printf("sum = %ld", sum);
  printf("\nEnd of program!\n");

  return 0;
}
