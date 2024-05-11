#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>


double f(double x) {
    return x * sin(x);
}

double exact_integral(double x)
{
    return sin(x) - x*cos(x);
}

int main(int argc, char** argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    // Check if the correct number of arguments has been passed
    if (argc != 3) {
        fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s N XMAX\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Parse the arguments
    int N = atoi(argv[1]);
    double X_MAX = atof(argv[2]);
    double deltaX = X_MAX / N;

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Calculate the number of nodes per process
    int nodes_per_process = N / world_size;

    // Allocate memory for the integral array
    double* integral = (double*) calloc(nodes_per_process + 1, sizeof(double));

    // Each process calculates the integral from 0 to its last node
    clock_t start_time = clock();
    double sum = 0.0;
    for (int i = world_rank * nodes_per_process + 1; i <= (world_rank + 1) * nodes_per_process; i++) {
        double x = (i - 0.5) * deltaX;
        sum += f(x) * deltaX;
        integral[i - world_rank * nodes_per_process] = sum;
    }

    // Gather the integral of each process to process 0
    double total_sum;
    MPI_Reduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        double exact = exact_integral(X_MAX);
        double error = fabs(total_sum - exact);
        double run_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        printf("Result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, total_sum, exact, error, run_time);
    }
    
    // Define an MPI file object
    MPI_File file_dat;

    // Open the .dat file in write mode
    char filename_dat[100];
    sprintf(filename_dat, "indef_integral_par.dat");
    MPI_File_open(MPI_COMM_WORLD, filename_dat, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file_dat);

    // Calculate the offset for this process
    MPI_Offset offset = world_rank * nodes_per_process * sizeof(double);

    // Write the integral data to the .dat file at the specified offset
    MPI_Status status;
    MPI_File_write_at(file_dat, offset, integral, nodes_per_process, MPI_DOUBLE, &status);

    // Close the .dat file
    MPI_File_close(&file_dat);
    
    if (world_rank == 0) {
        // Print integral info
        char filename_info[100];
        sprintf(filename_info, "indef_integral_par.info");
        FILE* file_info = fopen(filename_info, "w");
        if (file_info != NULL) {
            fprintf(file_info, "# %s\n", "Indefinite integral version parallel");
            fprintf(file_info, "data %s\n", filename_dat);
            fprintf(file_info, "N %d\n", N);
            fprintf(file_info, "XMAX %lf\n", X_MAX);
            fprintf(file_info, "size %d\n", world_size);
            fclose(file_info);
        }
    }
    
    // free the allocated memory at the end
    free(integral);

    // Finalize MPI
    MPI_Finalize();

    return 0;
}