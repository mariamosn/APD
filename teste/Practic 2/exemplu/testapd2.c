// Maria Moșneag 333CA

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MAPPERS 3
#define LINE_LEN 15

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if (numtasks != 6 || argc != 2) {
        if (rank == 0) {
            fprintf(stderr, "Usage: mpirun -np 6 ./testapd2 file.in\n");
        }
        MPI_Finalize();
        return 0;
    }


    if (rank == 0) {
        FILE *in = fopen(argv[1], "r");

        int lines;
        fscanf(in, "%d", &lines);

        for (int mapper = 1; mapper <= 3; mapper++) {
            int start = (mapper - 1) * (double)lines / MAPPERS;
            int end = mapper * (double)lines / MAPPERS;
            if (end > lines || mapper == MAPPERS) {
                end = lines;
            }
            int cnt = end - start;
            MPI_Send(&cnt, 1, MPI_INT, mapper, 0, MPI_COMM_WORLD);

            char line[LINE_LEN];
            for (int i = start; i < end; i++) {
                fscanf(in, "%s", line);

                MPI_Send(line, LINE_LEN, MPI_CHAR, mapper, 0, MPI_COMM_WORLD);
            }
        }       
    } else if (rank >= 1 && rank <= MAPPERS) {
        int cnt;
        MPI_Recv(&cnt, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        char line[LINE_LEN];
        int voc = 0, cons = 0;
        for (int i = 0; i < cnt; i++) {
            MPI_Recv(line, LINE_LEN, MPI_CHAR, 0, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
            printf("[%d] %s\n", rank, line);
            for (int i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
                if (line[i] == 'a' || line[i] == 'A' ||
                    line[i] == 'e' || line[i] == 'E' ||
                    line[i] == 'i' || line[i] == 'I' ||
                    line[i] == 'o' || line[i] == 'O' ||
                    line[i] == 'u' || line[i] == 'U') {
                    voc++;
                } else if (line[i] >= 'a' && line[i] <= 'z' ||
                            line[i] >= 'A' && line[i] <= 'Z') {
                    cons++;
                }
            }
        }

        printf("[%d] v=%d c=%d\n", rank, voc, cons);
        MPI_Send(&voc, 1, MPI_INT, 4, 0, MPI_COMM_WORLD);
        MPI_Send(&cons, 1, MPI_INT, 5, 0, MPI_COMM_WORLD);
    } else {
        int total = 0;
        int recv_val;
        for (int i = 1; i <= MAPPERS; i++) {
            MPI_Recv(&recv_val, 1, MPI_INT, i, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
            total += recv_val;
        }

        if (rank == 4) {
            printf("[%d] Vowels: %d\n", rank, total);
        } else if (rank == 5) {
            printf("[%d] Consonants: %d\n", rank, total);
        }
    }

    MPI_Finalize();

}