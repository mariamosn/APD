// Maria Moșneag 333CA

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

// tipul procesului
#define COORDINATOR 0
#define WORKER 1

// starea legăturii dintre procesele 0 și 1
#define CONNECTED 0
#define ERROR 1

#define COORD_NUM 3

int main (int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage:\nmpirun -np <numar_procese>"
                        " ./tema3 <dimensiune_vector> <eroare_comunicatie>\n");
        return 0;
    }
    
    int numtasks, rank;
    int n, comm_status;
    int process_type;

    int num_of_workers[COORD_NUM], *workers[COORD_NUM];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    n = atoi(argv[1]);
    comm_status = atoi(argv[2]);

    if (rank < COORD_NUM) {
        process_type = COORDINATOR;
    } else {
        process_type = WORKER;
    }

    ////////////////////////////////////////////////////////////////////////

    // stabilirea topologiei

    if (process_type == COORDINATOR) {
        // citește workerii din subarborele său
        int my_num_of_workers, *my_workers;

        FILE *in;
        if (rank == 0) {
            in = fopen("cluster0.txt", "r");
        } else if (rank == 1) {
            in = fopen("cluster1.txt", "r");
        } else if (rank == 2) {
            in = fopen("cluster2.txt", "r");
        }

        fscanf(in, "%d\n", &my_num_of_workers);
        my_workers = malloc(my_num_of_workers * sizeof(int));
        for (int i = 0; i < my_num_of_workers; i++) {
            fscanf(in, "%d\n", &my_workers[i]);
        }

        num_of_workers[rank] = my_num_of_workers;
        workers[rank] = my_workers;

        fclose(in);

        // trimite subarborele coordonatorului curent celorlalți
        // coordonatori cu care este conectat direct
        for (int i = 0; i < COORD_NUM; i++) {
            if (rank != i &&
                (comm_status == CONNECTED ||
                comm_status == ERROR &&
                !(i == 0 && rank == 1 || i == 1 && rank == 0))) {
                MPI_Send(&my_num_of_workers, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, i);

                MPI_Send(my_workers, my_num_of_workers, MPI_INT, i, 0,
                            MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, i);
            }
        }

        // primește subarborii coordonatorilor cu care este conectat direct
        int expected_recv = COORD_NUM - 1 - (comm_status == ERROR && rank < 2);
        for (int i = 1; i <= expected_recv; i++) {
            int recv_num_of_workers;
            int *recv_workers;
            MPI_Status status;

            MPI_Recv(&recv_num_of_workers, 1, MPI_INT, MPI_ANY_SOURCE, 0,
                        MPI_COMM_WORLD, &status);
            recv_workers = malloc(recv_num_of_workers * sizeof(int));
            MPI_Recv(recv_workers, recv_num_of_workers, MPI_INT,
                        status.MPI_SOURCE, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
            
            num_of_workers[status.MPI_SOURCE] = recv_num_of_workers;
            workers[status.MPI_SOURCE] = recv_workers;
        }

        // în cazul în care procesele 0 și 1 nu pot comunica direct,
        // procesul 2 va fi intermediar
        if (comm_status == ERROR) {
            if (rank == 2) {
                for (int i = 0; i <= 1; i++) {
                    int dest = 1 - i;
                    MPI_Send(&num_of_workers[i], 1, MPI_INT, dest, 0,
                                MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", 2, dest);

                    MPI_Send(workers[i], num_of_workers[i], MPI_INT, dest, 0,
                                MPI_COMM_WORLD);
                    printf("M(%d,%d)\n", 2, dest);
                }
            } else if (rank < 2) {
                int src = 1 - rank;
                MPI_Recv(&num_of_workers[src], 1, MPI_INT, 2, 0, MPI_COMM_WORLD,
                            MPI_STATUS_IGNORE);
                workers[src] = malloc(num_of_workers[src] * sizeof(int));
                MPI_Recv(workers[src], num_of_workers[src], MPI_INT, 2, 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        // anunță workerii că le este coordonator
        for (int i = 0; i < my_num_of_workers; i++) {
            MPI_Send(&rank, 1, MPI_INT, my_workers[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, my_workers[i]);
        }

        // trimite workerilor topologia
        for (int i = 0; i < my_num_of_workers; i++) {
            int dest = my_workers[i];
            for (int j = 0; j < COORD_NUM; j++) {
                MPI_Send(&num_of_workers[j], 1, MPI_INT, dest, 0,
                            MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, dest);

                MPI_Send(workers[j], num_of_workers[j], MPI_INT, dest, 0,
                            MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, dest);
            }
        }

    } else if (process_type == WORKER) {
        // află coordonatorul
        int my_coordinator;

        MPI_Recv(&my_coordinator, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);
        
        for (int i = 0; i < COORD_NUM; i++) {
            MPI_Recv(&num_of_workers[i], 1, MPI_INT, my_coordinator, 0,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            workers[i] = malloc(num_of_workers[i] * sizeof(int));
            MPI_Recv(workers[i], num_of_workers[i], MPI_INT, my_coordinator, 0,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // afișarea topologiei
    printf("%d -> ", rank);
    for (int i = 0; i < COORD_NUM; i++) {
        printf("%d:", i);
        for (int j = 0; j < num_of_workers[i] - 1; j++) {
            printf("%d,", workers[i][j]);
        }
        printf("%d ", workers[i][num_of_workers[i] - 1]);
    }
    printf("\n");

    ////////////////////////////////////////////////////////////////////////

    // realizarea calculelor

    

    MPI_Finalize();

}

