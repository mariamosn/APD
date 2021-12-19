#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define N 12
#define MASTER 0

void compareVectors(int *a, int *b) {
	// DO NOT MODIFY
	int i;
	for (i = 0; i < N; i++) {
		if (a[i] != b[i]) {
			printf("Sorted incorrectly\n");
			return;
		}
	}
	printf("Sorted correctly\n");
}

void displayVector(int *v) {
	// DO NOT MODIFY
	int i;
	// int displayWidth = 2 + log10(v[N - 1]);
	for (i = 0; i < N; i++) {
		// printf("%*i", displayWidth, v[i]);
		printf("%d ", v[i]);
	}
	printf("\n");
}

int cmp(const void *a, const void *b) {
	// DO NOT MODIFY
	int A = *(int *)a;
	int B = *(int *)b;
	return A - B;
}
 
int main(int argc, char * argv[]) {
	int rank, i, j;
	int nProcesses;
	MPI_Init(&argc, &argv);
	int pos[N];
	int sorted = 0;
	int *v = (int *)malloc(sizeof(int) * N);
	int *vQSort = (int *)malloc(sizeof(int) * N);

	for (i = 0; i < N; i++)
		pos[i] = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	printf("Hello from %i/%i\n", rank, nProcesses);

    if (rank == MASTER) {
        // generate random vector
		for (i = 0; i < N; i++) {
			v[i] = N - i;
		}
    }

    // send the vector to all processes
	MPI_Bcast(v, N, MPI_INT, MASTER, MPI_COMM_WORLD);
	

	if (rank == 0) {
		// DO NOT MODIFY
		displayVector(v);

		// make copy to check it against qsort
		// DO NOT MODIFY
		for (i = 0; i < N; i++)
			vQSort[i] = v[i];
		qsort(vQSort, N, sizeof(int), cmp);

		// sort the vector v
		for (int i = 0; i < N / nProcesses; i++) {
			for (int j = 0; j < N; j++) {
				if (v[i] > v[j] || (v[i] == v[j] && i > j)) {
					pos[i]++;
				}
			}
		}
		
        // recv the new pozitions
		for (int i = 1; i < nProcesses; i++) {
			MPI_Recv(&pos[i * N / nProcesses], N / nProcesses,
						MPI_INT, i, 0, MPI_COMM_WORLD,
						MPI_STATUS_IGNORE);
		}

		int sorted_v[N];

		for (int i = 0; i < N; i++) {
			sorted_v[pos[i]] = v[i];
		}

		displayVector(sorted_v);
		displayVector(vQSort);
		compareVectors(sorted_v, vQSort);
	} else {		
        // compute the positions
		for (int i = 0; i < N / nProcesses; i++) {
			for (int j = 0; j < N; j++) {
				if (v[i + rank * N / nProcesses] > v[j] ||
					(v[i + rank * N / nProcesses] == v[j] && i > j)) {
					pos[i + rank * N / nProcesses]++;
				}
			}
		}

        // send the new positions to process MASTER
		MPI_Send(&pos[rank * N / nProcesses], N / nProcesses,
					MPI_INT, MASTER, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
