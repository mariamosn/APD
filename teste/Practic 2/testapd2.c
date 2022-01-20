// Maria Moșneag 333CA

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int number = 2 * rank;

    // sortare - oets
    
    for (int i = 0; i < numtasks; i++) {
        if (i % 2 == 0) {
            if (rank % 2 == 0) {
                if (rank != numtasks - 1) {
                    int neigh_num;
                    MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(&neigh_num, 1, MPI_INT, rank + 1, 0,
                                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if (neigh_num > number) {
                        number = neigh_num;
                    }
                }
            } else {
                int neigh_num;
                MPI_Send(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                MPI_Recv(&neigh_num, 1, MPI_INT, rank - 1, 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (neigh_num < number) {
                    number = neigh_num;
                }
            }

        } else {
            if (rank % 2 == 1) {
                if (rank != numtasks - 1) {
                    int neigh_num;
                    MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(&neigh_num, 1, MPI_INT, rank + 1, 0,
                                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if (neigh_num > number) {
                        number = neigh_num;
                    }
                }
            } else {
                if (rank != 0) {
                    int neigh_num;
                    MPI_Send(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(&neigh_num, 1, MPI_INT, rank - 1, 0,
                                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if (neigh_num < number) {
                        number = neigh_num;
                    }
                }
            }
        }
    }

    printf("%d - %d\n", rank, number);

    // vreau ca toate task-urile să aibă valoarea finală înainte să încep
    // colectarea valorilor
    MPI_Barrier(MPI_COMM_WORLD);

    // colectarea valorilor

    if (rank < numtasks - 1) {
        for (int i = 0; i <= rank; i++) {
            // transmit la dreapta valorile tuturor vecinilor de la stânga
            if (i != rank) {
                int recv_num;
                MPI_Recv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
                            MPI_STATUS_IGNORE);
                MPI_Send(&recv_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);

            // transmit la final și valoarea mea
            } else {
                MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            }
        }
    } else {
        // primesc valorile vecinilor din stânga și le afișez în ordine
        for (int i = 0; i < rank; i++) {
            int recv_num;
            MPI_Recv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
            printf("%d ", recv_num);
        }

        // la final afișez și valoarea mea
        printf("%d\n", number);
    }

    MPI_Finalize();

}