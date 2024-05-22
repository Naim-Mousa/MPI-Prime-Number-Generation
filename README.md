# MPI Prime Number Generation

## Overview
This project involves creating an MPI program to generate prime numbers between 2 and N (inclusive) and to test its scalability and performance. The program is implemented in C and uses the MPI (Message Passing Interface) library.

## Prerequisites

  - MPI library
  - C compiler (e.g., gcc)

## Files

primes.c: The source code file.

## How to Compile

To compile the program, use the following command:

`mpicc -o primes primes.c`

## How to Run

To run the program, use the mpiexec command with the desired number of processes and the upper limit N:

`mpiexec -n <number_of_processes> ./primes <N>`

Example:

`mpiexec -n 2 ./primes 10`

This command will generate a file named 10.txt containing the prime numbers up to 10.

## Output

The program outputs a text file named N.txt (where N is the input number) that contains the prime numbers separated by a single space and in ascending order. For example, the output file 10.txt will contain: **2 3 5 7**

## Algorithm

The program follows these steps to generate prime numbers:
  - Generate all numbers from 2 to N.
  - Remove all multiples of each prime number starting from 2.
  - Continue this process until reaching the floor value of (N+1)/2.
  - The remaining numbers are the prime numbers.

## Implementation Details

  - The program distributes the range of numbers equally among the available processes.
  - Each process eliminates the non-prime numbers in its assigned range using the Sieve of Eratosthenes method.
  - The root process collects the prime numbers from all processes and writes them to the output file.

## Performance and Scalability

  - To measure the performance and scalability of the code, use MPI_Wtime() for timing:
  - The timing includes all computations, communications, and dynamic memory allocations.
  - Only the root process writes the results to the file and prints the elapsed time.





