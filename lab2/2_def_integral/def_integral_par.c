#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

double function(double x)
{
    return x*sin(x);
}

double exact_quadrature(double x)
{
    return sin(x) - x*cos(x);
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(argc != 3) {
        if (rank == 0) {
            fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s N XMAX\n", argv[0]);
        }
        MPI_Finalize();
        return -1;
    }

    int N = (int)strtol(argv[1], NULL, 10);
    double X_MAX = (double) strtold(argv[2], NULL);
    double deltaX = X_MAX/(double) N;

    int local_N = N / size;
    double local_result = 0;
    double result;

    clock_t start_time = clock();

    for (int i = rank * local_N + 1; i <= (rank + 1) * local_N; i++) {
        double x_middle = (i - 0.5) * deltaX;
        local_result = local_result + function(x_middle) * deltaX;
    }

    MPI_Reduce(&local_result, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double exact = exact_quadrature(X_MAX);
        double error = fabs(result - exact);
        double run_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        printf("Result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, result, exact, error, run_time);
    }

    MPI_Finalize();

    return 0;
}