#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define FILAS 100000
#define COLUMNAS 100000

int suma_diagonal_principal(int matriz[FILAS][COLUMNAS], int filas) {
    int i, suma = 0;
    for (i = 0; i < filas; i++) {
        suma += matriz[i][i];
    }
    return suma;
}

int suma_diagonal_secundaria(int matriz[FILAS][COLUMNAS], int filas) {
    int i, suma = 0;
    for (i = 0; i < filas; i++) {
        suma += matriz[i][COLUMNAS - 1 - i];
    }
    return suma;
}

void comparar_diagonales(int dp, int ds, int rank) {
    printf("El valor de la diagonal principal en el procesador %d es: %d\n", rank, dp);
    printf("El valor de la diagonal secundaria en el procesador %d es: %d\n", rank, ds);
    if (dp > ds) {
        printf("En el procesador %d, la diagonal principal es la mayor.\n", rank);
    } else if (ds > dp) {
        printf("En el procesador %d, la diagonal secundaria es la mayor.\n", rank);
    } else {
        printf("En el procesador %d, ambas diagonales tienen la misma suma.\n", rank);
    }
}

int main(int argc, char *argv[]) {
    int rank, size, i, j, suma_dp, suma_ds;
    int matriz[FILAS][COLUMNAS];
    int filas_por_proceso, resto_filas, inicio, fin;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        srand(time(NULL));

        for (i = 0; i < FILAS; i++) {
            for (j = 0; j < COLUMNAS; j++) {
                matriz[i][j] = rand() % 100;
            }
        }
        filas_por_proceso = FILAS / (size - 1);
        resto_filas = FILAS % (size - 1);
        inicio = 0;
        for (i = 1; i < size; i++) {
            int filas_proceso = filas_por_proceso;
            if (resto_filas > 0) {
                filas_proceso++;
                resto_filas--;
            }
            MPI_Send(&filas_proceso, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&matriz[inicio][0], filas_proceso*COLUMNAS, MPI_INT, i, 0, MPI_COMM_WORLD);
            inicio += filas_proceso;
        }
    } else {
        MPI_Recv(&filas_por_proceso, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int matriz_local[filas_por_proceso][COLUMNAS];
        MPI_Recv(&matriz_local[0][0], filas_por_proceso*COLUMNAS, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        suma_dp = suma_diagonal_principal(matriz_local, filas_por_proceso);
        suma_ds = suma_diagonal_secundaria(matriz_local, filas_por_proceso);
        comparar_diagonales(suma_dp, suma_ds, rank);
    }

    MPI_Finalize();
    return 0;
}