#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MASTER 0

int main (int argc, char *argv[])
{
    int procs, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int value = rank;

    int n = -1;
    for (int i = 31; i >= 0 && n == -1; i--) {
        if (procs >= 1 << i) {
            n = i;
        }
    }

    for (int i = 2; i <= n; i *= 2) {
        if (rank % i == 0 && rank + (i / 2) < procs) {
            int aux;
            MPI_Recv(&aux, 1, MPI_INT, rank + (i / 2), 0,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            value += aux;
        } else if (rank % (i / 2) == 0 && rank - (i / 2) >= 0) {
            MPI_Send(&value, 1, MPI_INT, rank - (i / 2), 0,
                        MPI_COMM_WORLD);
        }
    }

    if (rank == MASTER) {
        printf("Result = %d\n", value);
    }

    MPI_Finalize();

}

