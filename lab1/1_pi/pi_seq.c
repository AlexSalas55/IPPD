#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Función F(x) para el cálculo de Pi
double F(double x) {
    return 4.0 / (1.0 + x * x);
}

int main(int argc, char *argv[]) {
    // Obtenemos el valor de N y inicialzamos la suma a 0
    int N = atoi(argv[1]);
    double sum = 0.0;
    double delta_x = 1.0 / N;
    double start = omp_get_wtime();
    double x;

    // Calculamos el valor de Pi con la suma de rectángulos
    for (int i = 0; i < N; i++) {
        x = (i + 0.5) * delta_x;
        sum += F(x);
    }

    // Calculamos el valor de Pi y el tiempo de ejecución
    double pi = sum * delta_x;
    double end = omp_get_wtime();
    double runtime = end - start;

    printf("Pi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime);

    return 0;
}