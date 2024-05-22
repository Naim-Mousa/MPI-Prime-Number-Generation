#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/**
 * Main function to find prime numbers within a range using MPI.
 * 
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return Exit status of the program
 */
int main(int argc, char* argv[]) {
    int my_rank, comm_size;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    double start_time, end_time;

    // Start the timer in the root process
    if (my_rank == 0) {
        start_time = MPI_Wtime();
    }

    // Read the upper limit for prime number search from the command line argument
    int N = atoi(argv[1]);

    // Calculate the range of numbers to be checked by each process
    int range = N - 1;
    int N_slice = range / comm_size;
    int remainder = range % comm_size;

    // Determine the starting and ending indices for each process
    int floor = my_rank * N_slice + 2;
    int ceil = floor + N_slice - 1;

    // Distribute the remainder among the first few processes
    if (remainder > 0) {
        if (my_rank < remainder) {
            floor += my_rank;
            ceil += my_rank + 1;
        } else {
            floor += remainder;
            ceil += remainder;
        }
    }

    // Ensure the last process checks up to N
    if (my_rank == comm_size - 1) ceil = N;

    // Allocate memory for the array to store local primes
    int* local_primes = (int*) malloc((ceil - floor + 1) * sizeof(int));

    // Initialize the local_primes array with the range of numbers
    for (int i = 0; i < ceil - floor + 1; i++) {
        local_primes[i] = floor + i;
    }

    int prime_count = 0;

    // Iterate to find prime numbers
    for (int i = 2; i <= sqrt(N); i++) {
        int found_prime;

        // Root process checks if i is prime and broadcasts the result
        if (my_rank == 0) {
            found_prime = 1;
            for (int j = 2; j <= sqrt(i); j++) {
                if (i % j == 0) {
                    found_prime = 0;
                    break;
                }
            }
        }

        // Broadcast the found_prime value to all processes
        MPI_Bcast(&found_prime, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // If i is not prime, continue to the next iteration
        if (!found_prime) continue;

        // Eliminate multiples of the prime number i in the local_primes array
        for (int j = floor; j <= ceil; j++) {
            if (local_primes[j - floor] != 0 && local_primes[j - floor] % i == 0) {
                local_primes[j - floor] = 0;
            }
        }
    }

    // Allocate memory to store the prime numbers found locally
    int* primes = (int*) malloc((ceil - floor + 1) * sizeof(int));

    // Collect the prime numbers from local_primes
    for (int i = 0; i < ceil - floor + 1; i++) {
        if (local_primes[i] != 0) {
            primes[prime_count++] = local_primes[i];
        }
    }

    // Send the number of prime numbers found to the root process
    MPI_Send(&prime_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    // Send the prime numbers to the root process
    MPI_Send(primes, prime_count, MPI_INT, 0, 0, MPI_COMM_WORLD);

    free(primes);

    // Root process collects and prints the prime numbers
    if (my_rank == 0) {
        char filename[20];
        sprintf(filename, "%d.txt", N);
        FILE* file = fopen(filename, "w");

        // Print the prime numbers from the root process
        for (int i = 0; i <  ceil - floor + 1; i++) {
            if (local_primes[i] != 0) {
                fprintf(file, "%d ", local_primes[i]);
            }
        }

        // Receive the prime numbers from all other processes
        for (int i = 1; i < comm_size; i++) {
            int num_primes;
            MPI_Recv(&num_primes, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Create array of size num_primes for the sending process
            int* primes = (int*) malloc(num_primes * sizeof(int));

            // Receive the primes array
            MPI_Recv(primes, num_primes, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int j = 0; j < num_primes; j++) {
                fprintf(file, "%d ", primes[j]);
            }

            free(primes);
        }

        fclose(file);
    }

    // Free the local_primes array
    free(local_primes);

    // Finalize the MPI environment
    MPI_Finalize();

    return 0;
}
