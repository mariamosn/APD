#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define ROOT 0

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    // Checks the number of processes allowed.
    if (numtasks != 2) {
        printf("Wrong number of processes. Only 2 allowed!\n");
        MPI_Finalize();
        return 0;
    }

    // How many numbers will be sent.
    int send_numbers = 10;

    if (rank == 0) {

        // Generate the random numbers.
        // Generate the random tags.
        // Sends the numbers with the tags to the second process.
        for (int i = 0; i < send_numbers; i++) {
            int value = rand() % 1000;
            MPI_Send(&value, 1, MPI_INT, 1, i, MPI_COMM_WORLD);
            printf("Sent %d with the tag %d.\n", value, i);
        }
    } else {

        // Receives the information from the first process.
        // Prints the numbers with their corresponding tags.
        for (int i = 0; i < send_numbers; i++) {
            MPI_Status status;
            int value;
            MPI_Recv(&value, 1, MPI_INT, ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("Received %d with the tag %d.\n", value, status.MPI_TAG);
        }

    }

    MPI_Finalize();

}

