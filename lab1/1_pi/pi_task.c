#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double F(double x) {
    return 4.0 / (1.0 + x * x);
}

double compute_pi(int start, int end, int min_steps, double delta_x) {
    if (end - start <= min_steps) {
        double sum = 0.0;
        for (int i = start; i < end; i++) {
            double x = (i + 0.5) * delta_x;
            sum += F(x);
        }
        return sum;
    } else {
        int mid = (start + end) / 2;
        double sum1, sum2;
        #pragma omp task shared(sum1)
        sum1 = compute_pi(start, mid, min_steps, delta_x);
        #pragma omp task shared(sum2)
        sum2 = compute_pi(mid, end, min_steps, delta_x);
        #pragma omp taskwait
        return sum1 + sum2;
    }
}

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    double delta_x = 1.0 / N;
    double start_time = omp_get_wtime();

    double sum;
    #pragma omp parallel
    {
        #pragma omp single
        sum = compute_pi(0, N, M, delta_x);
    }

    double pi = sum * delta_x;
    double end_time = omp_get_wtime();
    double runtime = end_time - start_time;

    printf("Pi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime);

    return 0;
}