#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define TAMANIO_MATRIZ 100000

int main(int argc, char** argv) {
    int num_procesos, id_proceso;
    int matriz[TAMANIO_MATRIZ][TAMANIO_MATRIZ];
    int matriz_local[TAMANIO_MATRIZ/TAMANIO_MATRIZ][TAMANIO_MATRIZ];
    int num_buscado;
    int repeticiones_local = 0, repeticiones_total = 0;
    int i, j;
    MPI_Status status;

    srand(time(NULL));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procesos);
    MPI_Comm_rank(MPI_COMM_WORLD, &id_proceso);

    if (id_proceso == 0) {
        printf("Introduce el número que quieres buscar: \n");
        scanf("%d", &num_buscado);
    }

    MPI_Bcast(&num_buscado, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int tamano_parte = TAMANIO_MATRIZ / num_procesos;
    int inicio = id_proceso * tamano_parte;
    int fin = inicio + tamano_parte;

    for (i = inicio; i < fin; i++) {
        for (j = 0; j < TAMANIO_MATRIZ; j++) {
            matriz_local[i-inicio][j] = rand() % 1000;
            if (matriz_local[i-inicio][j] == num_buscado) {
                repeticiones_local++;
            }
        }
    }

    MPI_Gather(&matriz_local, TAMANIO_MATRIZ/TAMANIO_MATRIZ*TAMANIO_MATRIZ, MPI_INT, &matriz, TAMANIO_MATRIZ/TAMANIO_MATRIZ*TAMANIO_MATRIZ, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Reduce(&repeticiones_local, &repeticiones_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (id_proceso == 0) {
        printf("El número %d aparece %d veces en la matriz.\n", num_buscado, repeticiones_total);
        //printf("Matriz:\n");
        for (i = 0; i < TAMANIO_MATRIZ; i++) {
            for (j = 0; j < TAMANIO_MATRIZ; j++) {
                //printf("%d ", matriz[i][j]);
            }
            //printf("\n");
        }
    } 

    MPI_Finalize();

    return 0;
}