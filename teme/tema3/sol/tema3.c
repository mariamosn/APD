// Maria Moșneag 333CA

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
    int *v, size;

    int my_coordinator;
    int *offset;

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
        // TODO: de facut verificare la malloc
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
            // TODO: de facut verificare la malloc
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
                // TODO: de facut verificare la malloc
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
        // int my_coordinator;

        MPI_Recv(&my_coordinator, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);
        
        for (int i = 0; i < COORD_NUM; i++) {
            MPI_Recv(&num_of_workers[i], 1, MPI_INT, my_coordinator, 0,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // TODO: de facut verificare la malloc
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

    MPI_Barrier(MPI_COMM_WORLD);

    ////////////////////////////////////////////////////////////////////////

    // realizarea calculelor

    if (process_type == COORDINATOR) {
        // int *offset;
        if (rank == 0) {
            // generarea vectorului
            // TODO: de facut verificare la malloc
            v = malloc(n * sizeof(int));
            for (int i = 0; i < n; i++) {
                v[i] = i;
            }

            // TODO: de facut verificare la malloc
            offset = malloc(COORD_NUM * sizeof(int));

            // împărțirea vectorului între coordonatori
            int total_num_of_workers = numtasks - COORD_NUM;
            size = (double)n / total_num_of_workers * num_of_workers[0];
            offset[0] = 0;
            offset[1] = offset[0] + size;

            for (int i = 1; i < COORD_NUM; i++) {
                int dest = i;
                if (comm_status == ERROR && i == 1) {
                    dest = 2;
                }

                int crt_size = (double)n / total_num_of_workers *
                                num_of_workers[i];
                if (i == 2 && offset[i] + crt_size != n) {
                    crt_size = n - offset[i];
                }

                MPI_Send(&crt_size, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, dest);

                MPI_Send(v + offset[i], crt_size, MPI_INT, dest, 0,
                            MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, dest);

                if (i < COORD_NUM - 1) {
                    offset[i + 1] = offset[i] + crt_size;
                }
            }
        } else if (comm_status == CONNECTED ||
                    comm_status == ERROR && rank == 1) {
            // preia datele de la sursă
            MPI_Recv(&size, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
            // TODO: de facut verificare la malloc
            v = malloc(size * sizeof(int));
            MPI_Recv(v, size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
        } else if (comm_status == ERROR && rank == 2) {
            // intermediază comunicarea dintre 0 și 1

            // primește de la 0 datele lui 1
            int *aux_v, aux_size;
            MPI_Recv(&aux_size, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
            // TODO: de facut verificare la malloc
            aux_v = malloc(aux_size * sizeof(int));
            MPI_Recv(aux_v, aux_size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);

            // trimite către 1
            MPI_Send(&aux_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);

            MPI_Send(aux_v, aux_size, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);
            free(aux_v);

            // preia datele de la procesul 0
            MPI_Recv(&size, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
            // TODO: de facut verificare la malloc
            v = malloc(size * sizeof(int));
            MPI_Recv(v, size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
        }

        // trimite către workeri partea corespunzătoare
        int default_send_size = ceil((double)size / num_of_workers[rank]);
        for (int i = 0; i < num_of_workers[rank]; i++) {
            int off = i * default_send_size;
            int sz = default_send_size;
            if (off + sz > size || i == num_of_workers[rank] - 1) {
                sz = size - off;
            }

            MPI_Send(&sz, 1, MPI_INT, workers[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, workers[rank][i]);

            MPI_Send(v + off, sz, MPI_INT, workers[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, workers[rank][i]);
        }

        //*****************************************************************//

        // primește de la workeri părțile modificate
        for (int i = 0; i < num_of_workers[rank]; i++) {
            int off = i * default_send_size;
            int sz = default_send_size;
            if (off + sz > size || i == num_of_workers[rank] - 1) {
                sz = size - off;
            }

            MPI_Recv(v + off, sz, MPI_INT, workers[rank][i], 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
        }

        // 0 preia vectorii de la ceilalți coordonatori
        if (comm_status == CONNECTED) {
            if (rank == 0) {
                for (int i = 1; i < COORD_NUM; i++) {
                    int sz;
                    if (i == 1) {
                        sz = offset[i + 1] - offset[i];
                    } else {
                        sz = n - offset[i];
                    }
                    MPI_Recv(v + offset[i], sz, MPI_INT, i, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
                }
            } else {
                MPI_Send(v, size, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, 0);
            }
        } else {
            if (rank == 0) {
                MPI_Recv(v + offset[2], n - offset[2], MPI_INT, 2, 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(v + offset[1], offset[2] - offset[1], MPI_INT, 2, 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (rank == 1) {
                MPI_Send(&size, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, 2);

                MPI_Send(v, size, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, 2);
            } else {
                MPI_Send(v, size, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, 0);

                int sz;
                MPI_Recv(&sz, 1, MPI_INT, 1, 0, MPI_COMM_WORLD,
                            MPI_STATUS_IGNORE);
                int *aux_v = malloc(sz * sizeof(int));
                MPI_Recv(aux_v, sz, MPI_INT, 1, 0, MPI_COMM_WORLD,
                            MPI_STATUS_IGNORE);

                MPI_Send(aux_v, sz, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, 0);

                free(aux_v);
            }
        }

        // 0 afișează rezultatele
        if (rank == 0) {
            printf("Rezultat: ");
            for (int i = 0; i < n; i++) {
                printf("%d ", v[i]);
            }
            printf("\n");
        }

    } else if (process_type == WORKER) {
        // primește partea de vector
        MPI_Recv(&size, 1, MPI_INT, my_coordinator, 0, MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);
        v = malloc(size * sizeof(int));
        MPI_Recv(v, size, MPI_INT, my_coordinator, 0, MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);

        // modifică vectorul corespunzător (x2)
        for (int i = 0; i < size; i++) {
            v[i] = v[i] << 1;
        }

        // trimite vectorul modificat coordonatorului
        MPI_Send(v, size, MPI_INT, my_coordinator, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, my_coordinator);
    }

    ////////////////////////////////////////////////////////////////////////

    MPI_Barrier(MPI_COMM_WORLD);

    // eliberarea memoriei
    for (int i = 0; i < COORD_NUM; i++) {
        free(workers[i]);
    }
    free(v);
    
    if (rank == 0) {
        free(offset);
    }

    MPI_Finalize();
}
