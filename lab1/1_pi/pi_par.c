#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Función F(x) para el cálculo de Pi
double F(double x) {
    return 4.0 / (1.0 + x * x);
}

int main(int argc, char *argv[]) {
    // Obtenemos el valor de N 
    int N = atoi(argv[1]);
    int N_threads = omp_get_max_threads();
    // Creamos un array para almacenar las sumas parciales de cada hilo
    double sum[N_threads];
    // Calculamos el incremento en x para cada iteración
    double delta_x = 1.0 / N;
    double start = omp_get_wtime();
    
    // Calculamos el número de iteraciones por hilo y iniciamos la región paralela
    int iter_per_thread = N / N_threads;
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        // Calculamos el índice de inicio del hilo
        int start = thread_id * iter_per_thread;
        // Calculamos el índice de fin del hilo
        int end = (thread_id == N_threads - 1) ? N : start + iter_per_thread;
        double x;        
        // Realizamos las iteraciones para cada hilo y añadimos el valor a la suma parcial del hilo
        for (int i = start; i < end; i++) {
            x = (i + 0.5) * delta_x;
            sum[thread_id] += F(x);
        }
        
    }
    // Sumamos todas las sumas parciales de cada hilo
    double sum_total = 0.0;
    for (int i = 0; i < N_threads; i++) {
        sum_total += sum[i];
    }

    // Calculamos el valor de Pi
    double pi = sum_total * delta_x;
    double end = omp_get_wtime();
    double runtime = end - start;

    printf("Pi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime);

    return 0;
}