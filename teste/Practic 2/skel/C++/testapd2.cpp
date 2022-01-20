// Maria Moșneag 333CA

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    /*
    MPI_Send(void* data, int count, MPI_Datatype datatype,
                int destination, int tag, MPI_Comm communicator);

    MPI_Recv(void* data, int count, MPI_Datatype datatype, int source,
                int tag, MPI_Comm communicator, MPI_Status* status);
    */

    MPI_Finalize();

}