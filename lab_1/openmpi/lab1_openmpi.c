// #include <stdio.h>
// #include "mpi.h"

// #define number 10000000000

// int getSum(int start, int end)
// {
//     int sum = 0;
//     for (int i = start; i < end; i++)
//     {
//         sum += i;
//     }
//     return sum;
// }

// int main(int argc, char *argv[])
// {
//     MPI_Status status;
//     int pid, numberOfProcesses;

//     MPI_Init(&argc, &argv);
//     MPI_Comm_rank(MPI_COMM_WORLD, &pid);
//     MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

//     if (pid == 0)
//     {
//         int r = number / numberOfProcesses;
//         int id, start, end;

//         if (numberOfProcesses > 1)
//         {
//             int rest = number % numberOfProcesses;
//             int missed = 0;
//             for (int id = 1; id < numberOfProcesses; id++)
//             {
//                 start = r * id;
//                 if ((rest != 0) && (id == numberOfProcesses - 1))
//                 {
//                     missed = rest;
//                 }
//                 end = start + r + missed;
//                 MPI_Send(&start, 1, MPI_INT, id, 0, MPI_COMM_WORLD);
//                 MPI_Send(&end, 1, MPI_INT, id, 0, MPI_COMM_WORLD);
//             }
//         }

//         int sum = getSum(0, r);
//         int tmp;
//         for (int i = 1; i < numberOfProcesses; i++)
//         {
//             MPI_Recv(&tmp, 1, MPI_INT,MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
//             sum += tmp;
//         }
//         printf("Sum = %d\n", sum);
//     }
//     else
//     {
//         int start, end;
//         MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
//         MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

//         int partial_sum = getSum(start, end);
//         MPI_Send(&partial_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
//     }

//     MPI_Finalize();

//     return 0;
// }
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mpi.h"

int MAX_INPUT_SIZE = 50;

int isNumeric(const char *str)
{
    while (*str != '\0')
    {
        if (*str < '0' || *str > '9')
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
    MPI_Finalize();
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
    MPI_Status status;
    int numberOfProcesses;
    int processId;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

    if (processId == 0)
    { // master process
        char *app_name = argv[0];
        if (argc < 2)
        {
            printf("Call application %s with arguments [sleepy|busy].\n", app_name);
            printf("Application will read input.txt file and will compute the Fibonacci number of the read position.\n");
            printf("Example:\n");
            printf("%s sleepy -- Computation will be slowed down by using sleep instructions.\n", app_name);
            printf("%s busy -- Computation will be slowed down by keeping CPU busy.\n", app_name);

            exit(0);
        }
        if (argc > 2)
        {
            print_incorrect_arguments_message_and_exit(app_name);
        }
        int sleepy_mode = 1;
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

        char *input_file_name = "input.txt";
        char *output_file_name = "output.txt";
        int input_numbers[MAX_INPUT_SIZE];

        int line_count = read_fibonacci_numbers(input_file_name, input_numbers);

        int linesPerProcess = line_count / numberOfProcesses;
        if (numberOfProcesses > 1)
        {
            int rest = line_count % numberOfProcesses;
            int missed = 0;
            for (int id = 1; id < numberOfProcesses; id++)
            {
                int start = linesPerProcess * id;
                if ((rest != 0) && (id == numberOfProcesses - 1))
                {
                    missed = rest;
                }
                int end = start + linesPerProcess + missed;
                MPI_Send(&start, 1, MPI_INT, id, 0, MPI_COMM_WORLD);
                MPI_Send(&end, 1, MPI_INT, id, 0, MPI_COMM_WORLD);
                MPI_Send(&input_numbers, MAX_INPUT_SIZE, MPI_INT, id, 0, MPI_COMM_WORLD);
                MPI_Send(&sleepy_mode, 1, MPI_INT, id, 0, MPI_COMM_WORLD);
            }
        }
        long output_numbers[MAX_INPUT_SIZE];
        setOutput(0, linesPerProcess, input_numbers, output_numbers, sleepy_mode);

        int partialStart, partialEnd;
        long* partialFilledOutput;
        for (int i = 1; i < numberOfProcesses; i++)
        {
            MPI_Recv(&partialFilledOutput, MAX_INPUT_SIZE, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&partialStart, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&partialEnd, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            for (int k = partialStart; k < partialEnd; k++)
            {
                output_numbers[k] = partialFilledOutput[k];
            }
        }

        write_fibonacci_numbers(output_file_name, output_numbers, line_count);
        printf("\n");
    }
    else
    { // slave process
        int start, end, *input_numbers, sleepy_mode;
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&input_numbers, MAX_INPUT_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&sleepy_mode, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        long output_numbers[MAX_INPUT_SIZE];

        setOutput(start, end, input_numbers, output_numbers, sleepy_mode);
        MPI_Send(&output_numbers, MAX_INPUT_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
