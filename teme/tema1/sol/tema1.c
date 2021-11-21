// Maria Moșneag 333CA

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "genetic_algorithm.h"
#include "tema1_helper.h"

void run_parallel_genetic_algorithm(sack_object *objects, int object_count,
									int generations_count, int sack_capacity,
									int P) {
	individual *current_generation = (individual*) calloc(object_count,
													sizeof(individual));
	individual *next_generation = (individual*) calloc(object_count,
													sizeof(individual));

	int r, sorted = 0;
	pthread_t *threads;
	struct my_arg *arguments;
	void *status;
	pthread_barrier_t barrier;

	threads = (pthread_t*) malloc(P * sizeof(pthread_t));
	arguments = (struct my_arg*) malloc(P * sizeof(struct my_arg));

	pthread_barrier_init(&barrier, NULL, P);

	for (int i = 0; i < P; i++) {
		arguments[i].id = i;
		arguments[i].P = P;
		arguments[i].object_count = object_count;
		arguments[i].generations_count = generations_count;
		arguments[i].sack_capacity = sack_capacity;
		arguments[i].objects = objects;
		arguments[i].current_generation = current_generation;
		arguments[i].next_generation = next_generation;
		arguments[i].barrier = &barrier;
		arguments[i].sorted = &sorted;

		r = pthread_create(&threads[i], NULL, thread_function, &arguments[i]);

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

	// free resources
	free(current_generation);
	free(next_generation);
	free(threads);
	free(arguments);
}

int main(int argc, char *argv[]) {
	// array with all the objects that can be placed in the sack
	sack_object *objects = NULL;

	// number of objects
	int object_count = 0;

	// maximum weight that can be carried in the sack
	int sack_capacity = 0;

	// number of generations
	int generations_count = 0;

	// number of threads
	int P = 0;

	if (!read_input(&objects, &object_count, &sack_capacity, &generations_count,
					&P, argc, argv)) {
		return 0;
	}

    run_parallel_genetic_algorithm(objects, object_count, generations_count,
									sack_capacity, P);

	free(objects);

	return 0;
}
