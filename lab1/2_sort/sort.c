#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include "sort.h"

int sort(int *array, int n) {
    int i, j, tmp;

    for (i=1;i<n;i++) {  
        tmp = array[i];  
        for(j=i-1;j >= 0 && array[j] > tmp;j--) {  
            array[j+1] = array[j];  
        }  
        array[j+1] = tmp;  
    }
}

int sort_openmp(int *array, int n) {
    
    // Obtiene el número de threads establecido por el usuario
    int NUM_THREADS = omp_get_max_threads();
    int INT_MAX = 2147483647;
    // Calcula el tamaño base de cada fragmento que un hilo ordenará
    int base_chunk_size = n / NUM_THREADS;
    // Calcula el número de fragmentos que tendrán un elemento extra
    int num_extra = n % NUM_THREADS;
    // Asigna memoria para un array temporal
    int *tmp_array = malloc(n * sizeof(int));
    // Inicializa head_i
    int *head_i = malloc(NUM_THREADS * sizeof(int));

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int i, j, tmp;
        int thread_id = omp_get_thread_num();
        // Calcula el tamaño del fragmento del hilo actual
        int chunk_size = base_chunk_size + (thread_id < num_extra ? 1 : 0);
        // Calcula los índices de inicio y fin del fragmento del hilo actual
        int start = thread_id * base_chunk_size + (thread_id < num_extra ? thread_id : num_extra);
        int end = start + chunk_size;

        // Cada hilo ordena su fragmento del array
        for (i = start; i < end; i++) {
            tmp = array[i];
            for (j = i - 1; j >= start && array[j] > tmp; j--) {
                array[j + 1] = array[j];
            }
            array[j + 1] = tmp;
        }

        // Copia el fragmento ordenado a tmp_array
        for (int k = start; k < end; k++) {
            tmp_array[k] = array[k];
        }

        // Guarda el índice de fin del fragmento del hilo actual en head_i
        head_i[thread_id] = start;
    }

// Combina los fragmentos ordenados en el array original
int min_index;;
for (int i = 0; i < n; i++) {
    min_index = 0;  // Asumimos inicialmente que array[tail_i[0]] es el más pequeño
        for (int j = 1; j < NUM_THREADS; j++) {
            if (tmp_array[head_i[j]] < tmp_array[head_i[min_index]]) {
                min_index = j;  // Si encontramos un valor más pequeño, actualizamos min_index
            }
        }

    // Agrega el valor más bajo al array temporal
    array[i] = tmp_array[head_i[min_index]];

    // Manejamos el caso en el que el fragmento de un hilo se haya acabado y no haya más elementos
    if (tmp_array[head_i[min_index]] > tmp_array[head_i[min_index]+1] || head_i[min_index]==n) {
        tmp_array[head_i[min_index]]=INT_MAX;
    }else{
        head_i[min_index]++;
    }
}
    // Liberar la memoria asignada para tmp_array
    free(tmp_array);
}

void fill_array(int *array, int n) {
    int i;
    
    srand(time(NULL));
    for(i=0; i < n; i++) {
        array[i] = rand()%n;
    }
}
