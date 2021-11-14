// Maria Moșneag 333CA

#ifndef TEMA1_HELPER_H
#define TEMA1_HELPER_H

#include <pthread.h>
#include "genetic_algorithm.h"

struct my_arg {
	int id;
	int P;
	int object_count;
	int generations_count;
	int sack_capacity;
	sack_object *objects;
	individual *current_generation;
	individual *next_generation;
	pthread_barrier_t *barrier;
	int *sorted;
};

// merge function for merging two parts
void merge(int low, int mid, int high, individual *a);
 
// merge sort function
void merge_sort_h(int low, int high, individual *v);

void merge_sort(int id, int N, int P, individual *v);

void sort(int id, int object_count, int P, individual *current_generation,
			pthread_barrier_t *barrier);

// set initial generation
// (composed of object_count individuals with a single item in the sack)
void set_initial_generation(int id, int P, int object_count,
							individual *current_generation,
							individual *next_generation);

void compute_fitness(int start, int end, individual *generation,
						sack_object *objects, int sack_capacity);

// keep first 30% children (elite children selection)
void elite_children_selection(int object_count, int id, int P, int *cursor,
								individual *current_generation,
								individual *next_generation);

// mutate first 20% children with the first version of bit string mutation
void first_mutation(int object_count, int id, int P, int k, int *cursor,
					individual *current_generation,
					individual *next_generation);

// mutate next 20% children with the second version of bit string mutation
void second_mutation(int object_count, int id, int P, int k, int *cursor,
						individual *current_generation,
						individual *next_generation);

// crossover first 30% parents with one-point crossover
// (if there is an odd number of parents, the last one is kept as such)
void crossover_stage(int object_count, int id, int P, int k, int *cursor,
						individual *current_generation,
						individual *next_generation,
						pthread_barrier_t *barrier);

void free_resources(int id, int P, individual *current_generation,
					individual *next_generation);

void *thread_function(void *arg);

#endif
