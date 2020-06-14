#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "omp.h"

int MAX_INPUT_SIZE = 50;

int isNumeric(const char *str) 
{
    while(*str != '\0')
    {
        if(*str < '0' || *str > '9')
            return 0;
        str++;
    }
    return 1;
}

void keep_cpu_asleep()
{
  useconds_t sleep_milisec = 250 * 1000;
  usleep(sleep_milisec);
}

int keep_cpu_busy()
{
  int iterations_count = 13000;
  int k = 0;
  for (int i = 0; i < iterations_count; i++)
  {
    for (int j = 0; j < iterations_count; j++)
    {
      k++;
    }
  }
  return k;
}

long fibonacci(int number)
{
  if (number <= 0)
  {
    printf("Error! Passed number is negative %i. Expected only positive number as input.", number);
    exit(1);
  }

  if (number == 1)
    return 0;
  if (number == 2)
    return 1;

  long number1 = 0, number2 = 1;
  long fib;

  for (int i = 3; i <= number; i++)
  {
    fib = number1 + number2;
    number1 = number2;
    number2 = fib;
  }
  return fib;
}

long sleepy_fibonacci(int number)
{
  keep_cpu_asleep();
  return fibonacci(number);
}

long busy_fibonacci(int number)
{
  keep_cpu_busy();
  return fibonacci(number);
}

int read_fibonacci_numbers(char *file_name, int *c)
{
  FILE *fptr;
  if ((fptr = fopen(file_name, "r")) == NULL)
  {
    printf("Could not open file for reading %s.", file_name);
    exit(1);
  }
  char *line = NULL;
  size_t len = 0;

  int line_count = 0;
  while (getline(&line, &len, fptr) != -1 && line_count < MAX_INPUT_SIZE)
  {
    c[line_count] = atoi(line);
    line_count++;
  }
  fclose(fptr);
  return line_count;
}

void write_fibonacci_numbers(char *file_name, long *c, int num_records)
{
  FILE *fptr;
  if ((fptr = fopen(file_name, "w")) == NULL)
  {
    printf("Could not open file for writing %s.", file_name);
    exit(1);
  }
  for (int i = 0; i < num_records; i++)
  {
    fprintf(fptr, "%li\n", c[i]);
  }

  fclose(fptr);
}

void print_incorrect_arguments_message_and_exit(char *app_name)
{
  printf("Unexpected number of arguments \n");
  printf("Call application %s with arguments [sleepy|busy] and number of threads [1..32] as the powers of 2.\n", app_name);
  exit(0);
}

void setOutput(int start, int end, int *input, long *output, int sleepy_mode)
{
  for (int i = start; i < end; i++)
  {
    printf("%i\n", input[i]);
    long fib = sleepy_mode == 1
                   ? sleepy_fibonacci(input[i])
                   : busy_fibonacci(input[i]);

    printf("Fib: %li\n", fib);
    output[i] = fib;
    printf("\n");
  }
}

int main(int argc, char *argv[])
{
  char *app_name = argv[0];
  if (argc < 3)
  {
    printf("Call application %s with arguments [sleepy|busy].\n", app_name);
    printf("Application will read input.txt file and will compute the Fibonacci number of the read position.\n");
    printf("Example:\n");
    printf("%s sleepy -- Computation will be slowed down by using sleep instructions.\n", app_name);
    printf("%s busy -- Computation will be slowed down by keeping CPU busy.\n", app_name);

    exit(0);
  }
  if (argc > 3)
  {
    print_incorrect_arguments_message_and_exit(app_name);
  }
  int sleepy_mode = 1;
  int numberOfThreads = 1;
  if (strcmp(argv[1], "sleepy") == 0)
  {
    sleepy_mode = 1;
  }
  else if (strcmp(argv[1], "busy") == 0)
  {
    sleepy_mode = 0;
  }
  else
  {
    printf("----\n");
    print_incorrect_arguments_message_and_exit(app_name);
  }
  if (sleepy_mode == 1)
  {
    printf("Computing Fibonacci numbers using Sleepy Mode computation.\n");
  }
  else
  {
    printf("Computing Fibonacci numbers using Busy Mode computation.\n");
  }
  if (isNumeric(argv[2]) == 1) {
    numberOfThreads = atoi(argv[2]);
  } else {
    printf("----\n");
    print_incorrect_arguments_message_and_exit(app_name);
  }

  char *input_file_name = "input.txt";
  char *output_file_name = "output.txt";
  int input_numbers[MAX_INPUT_SIZE];
  long output_numbers[MAX_INPUT_SIZE];

  int line_count = read_fibonacci_numbers(input_file_name, input_numbers);

  int linesPerThread = line_count / numberOfThreads;
  int threadId;
#pragma omp parallel for
  for (threadId = 0; threadId < numberOfThreads; threadId++)
  {
    int start = linesPerThread * threadId;
    int end = start + linesPerThread;
    setOutput(start, end, input_numbers, output_numbers, sleepy_mode);
  }

  write_fibonacci_numbers(output_file_name, output_numbers, line_count);
  printf("\n");
}
