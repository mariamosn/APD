// Maria Moșneag 333CA

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define INSIDE -1 // daca numarul cautat este in interiorul intervalului
#define OUTSIDE -2 // daca numarul cautat este in afara intervalului

struct my_arg {
	int id;
	int N;
	int P;
	int number; // numarul cautat
	int *left; // stanga intervalului mare
	int *right; // dreapta intervalului mare
	int *keep_running;
	int *v; // vectorul in care se face cautarea
	int *found;
	pthread_barrier_t *barrier;
};

/*
void binary_search() {
	while (keep_running) {
		int mid = left + (right - left) / 2;
		if (left > right) {
			printf("Number not found\n");
			break;
		}

		if (v[mid] == number) {
			keep_running = 0;
			printf("Number found at position %d\n", mid);
		} else if (v[mid] > number) {
			left = mid + 1;
		} else {
			right = mid - 1;
		}
	}
}
*/

void *f(void *arg)
{
	struct my_arg* data = (struct my_arg*) arg;

	while (*data->keep_running) {
		pthread_barrier_wait(data->barrier);
		int size = *data->right - *data->left;
		// TODO: implementati parallel binary search

		if (*data->left > *data->right) {
			if (data->id == 0) {
				printf("Number not found\n");
			}
			break;
		}

		int start = *data->left + data->id * (double)size / data->P;
		int end = fmin(*data->right, *data->left + (data->id + 1) * (double)size / data->P - 1);
		end = fmax(end, *data->left);
		if (data->id == data->P - 1) {
			end = fmin(*data->right, data->N - 1);
		}

		if (data->v[start] == data->number && *data->keep_running) {
			printf("Number found at position %d\n", start);
			*data->keep_running = 0;
		} else if (data->v[end] == data->number && *data->keep_running) {
			printf("Number found at position %d\n", end);
			*data->keep_running = 0;
		} else if (data->v[start] < data->number && data->number < data->v[end]) {
			data->found[data->id] = 1;
		} else {
			data->found[data->id] = 0;
		}

		pthread_barrier_wait(data->barrier);

		if (data->found[data->id]) {
			*data->left = start + 1;
			*data->right = end - 1;
		}

		if (data->id == 0 && *data->keep_running) {
			int not_ok = 1;
			for (int i = 0; i < data->P && not_ok; i++) {
				if (data->found[i]) {
					not_ok = 0;
				}
			}
			if (not_ok) {
				printf("Number not found\n");
				*data->keep_running = 0;
			}
		}
		pthread_barrier_wait(data->barrier);
	}

	pthread_exit(NULL);
}

void display_vector(int *v, int size) {
	int i;

	for (i = 0; i < size; i++) {
		printf("%d ", v[i]);
	}

	printf("\n");
}


int main(int argc, char *argv[])
{
	int r, N, P, number, keep_running, left, right;
	int *v;
	int *found;
	void *status;
	pthread_t *threads;
	struct my_arg *arguments;
	pthread_barrier_t barrier;

	if (argc < 4) {
		printf("Usage:\n\t./ex N P number\n");
		return 1;
	}

	N = atoi(argv[1]);
	P = atoi(argv[2]);
	number = atoi(argv[3]);

	keep_running = 1;
	left = 0;
	right = N;

	v = (int*) malloc(N * sizeof(int));
	threads = (pthread_t*) malloc(P * sizeof(pthread_t));
	arguments = (struct my_arg*) malloc(P * sizeof(struct my_arg));
	found = (int*) malloc(P * sizeof(int));

	pthread_barrier_init(&barrier, NULL, P);

	for (int i = 0; i < N; i++) {
		v[i] = i * 2;
	}

	display_vector(v, N);

	for (int i = 0; i < P; i++) {
		arguments[i].id = i;
		arguments[i].N = N;
		arguments[i].P = P;
		arguments[i].number = number;
		arguments[i].left = &left;
		arguments[i].right = &right;
		arguments[i].keep_running = &keep_running;
		arguments[i].v = v;
		arguments[i].found = found;
		arguments[i].barrier = &barrier;

		r = pthread_create(&threads[i], NULL, f, &arguments[i]);

		if (r) {
			printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}
	}

	for (int i = 0; i < P; i++) {
		r = pthread_join(threads[i], &status);

		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
	}
	pthread_barrier_destroy(&barrier);

	free(v);
	free(threads);
	free(arguments);

	return 0;
}
