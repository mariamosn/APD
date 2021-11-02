// Maria Moșneag 333CA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "genetic_algorithm.h"
#include "tema1_helper.h"

// merge function for merging two parts
void merge(int low, int mid, int high, individual *a) {
	individual left[mid - low + 1];
	individual right[high - mid];
 
    // n1 is the size of the left part
    // n2 is the size of the right part
    int n1 = mid - low + 1, n2 = high - mid, i, j;
 
    // storing values in left part
    for (i = 0; i < n1; i++) {
        left[i] = a[i + low];
    }
 
    // storing values in right part
    for (i = 0; i < n2; i++) {
        right[i] = a[i + mid + 1];
    }
 
    int k = low;
    i = j = 0;
 
    // merge left and right in ascending order
    while (i < n1 && j < n2) {
		if (cmpfunc(&left[i], &right[j]) <= 0) {
            a[k++] = left[i++];
        } else {
            a[k++] = right[j++];
        }
    }
 
    // insert remaining values from left
    while (i < n1) {
        a[k++] = left[i++];
    }
 
    // insert remaining values from right
    while (j < n2) {
        a[k++] = right[j++];
    }
}
 
// merge sort function
void merge_sort_h(int low, int high, individual *v) {
    // calculating mid point of array
    int mid = low + (high - low) / 2;
    if (low < high) {
 
        // calling first half
        merge_sort_h(low, mid, v);
 
        // calling second half
        merge_sort_h(mid + 1, high, v);
 
        // merging the two halves
        merge(low, mid, high, v);
    }
}
 
// thread function for multi-threading
void merge_sort(int id, int N, int P, individual *v) { 
    // calculating low and high
    int low = id * ((double)N / P);
    int high = (id + 1) * ((double)N / P) - 1;
	if (id == P - 1) {
		high = N - 1;
	}
 
    // evaluating mid point
    int mid = low + (high - low) / 2;
    if (low < high) {
        merge_sort_h(low, mid, v);
        merge_sort_h(mid + 1, high, v);
        merge(low, mid, high, v);
    }
}

void sort(int id, int object_count, int P, individual *current_generation,
			pthread_barrier_t *barrier) {
	merge_sort(id, object_count, P, current_generation);
	
	// merging the final 4 parts
	pthread_barrier_wait(barrier);
	if (P == 2 && id == 0) {
		merge(0, (object_count - 1) / 2, object_count - 1, current_generation);
	} else if (P == 3 && id == 0) {
		int low0 = 0, high0 = ((double)object_count / P) - 1,
			high1 = 2 * ((double)object_count / P) - 1,
			high2 = object_count - 1;
		merge(low0, high0, high1, current_generation);
		merge(low0, high1, high2, current_generation);
	} else if (P == 4) {
		int low0 = 0, high0 = ((double)object_count / P) - 1,
			high1 = 2 * ((double)object_count / P) - 1,
			low2 = 2 * ((double)object_count / P),
			high2 = 3 * ((double)object_count / P) - 1,
			high3 = object_count - 1;
		if (id == 0) {
			merge(low0, high0, high1, current_generation);
		}
		if (id == 1) {
			merge(low2, high2, high3, current_generation);
		}
		pthread_barrier_wait(barrier);
		if (id == 0) {
			merge(low0, high1, high3, current_generation);
		}
	}
	pthread_barrier_wait(barrier);
}

// set initial generation
// (composed of object_count individuals with a single item in the sack)
void set_initial_generation(int start, int end, int object_count,
    						individual *current_generation,
							individual *next_generation) {

	for (int i = start; i < end; ++i) {
		current_generation[i].fitness = 0;
		current_generation[i].chromosomes = (int*) calloc(object_count,
															sizeof(int));
		current_generation[i].chromosomes[i] = 1;
		current_generation[i].index = i;
		current_generation[i].chromosome_length = object_count;

		next_generation[i].fitness = 0;
		next_generation[i].chromosomes = (int*) calloc(object_count,
														sizeof(int));
		next_generation[i].index = i;
		next_generation[i].chromosome_length = object_count;
	}
}

void compute_fitness(int start, int end, individual *generation,
						sack_object *objects, int sack_capacity) {
    int weight, profit;
    for (int i = start; i < end; ++i) {
        weight = 0;
        profit = 0;

        for (int j = 0; j < generation[i].chromosome_length; ++j) {
            if (generation[i].chromosomes[j]) {
                weight += objects[j].weight;
                profit += objects[j].profit;
            }
        }

        generation[i].fitness = (weight <= sack_capacity) ? profit : 0;
    }
}

// keep first 30% children (elite children selection)
void elite_children_selection(int object_count, int id, int P, int *cursor,
								individual *current_generation,
								individual *next_generation) {
	int count = object_count * 3 / 10;
	int start_local = id * (double)count / P;
	int end_local = (id + 1) * (double)count / P;
	if (end_local > count) {
		end_local = count;
	}
	for (int i = start_local; i < end_local; ++i) {
		copy_individual(current_generation + i, next_generation + i);
	}
	*cursor = count;
}

// mutate first 20% children with the first version of bit string mutation
void first_mutation(int object_count, int id, int P, int k, int *cursor,
					individual *current_generation,
					individual *next_generation) {
	int count = object_count * 2 / 10;
	int start_local = id * (double)count / P;
	int end_local = (id + 1) * (double)count / P;
	if (end_local > count) {
		end_local = count;
	}
	for (int i = start_local; i < end_local; ++i) {
		copy_individual(current_generation + i, next_generation + *cursor + i);
		mutate_bit_string_1(next_generation + *cursor + i, k);
	}
	*cursor += count;
}

// mutate next 20% children with the second version of bit string mutation
void second_mutation(int object_count, int id, int P, int k, int *cursor,
						individual *current_generation,
						individual *next_generation) {
	int count = object_count * 2 / 10;
	int start_local = id * (double)count / P;
	int end_local = (id + 1) * (double)count / P;
	if (end_local > count) {
		end_local = count;
	}
	for (int i = start_local; i < end_local; ++i) {
		copy_individual(current_generation + i + count,
						next_generation + *cursor + i);
		mutate_bit_string_2(next_generation + *cursor + i, k);
	}
	*cursor += count;
}

// crossover first 30% parents with one-point crossover
// (if there is an odd number of parents, the last one is kept as such)
void crossover_stage(int object_count, int id, int P, int k, int *cursor,
						individual *current_generation,
						individual *next_generation,
						pthread_barrier_t *barrier) {
	int count = object_count * 3 / 10;
	if (count % 2 == 1) {
		if (id == 0) {
			copy_individual(current_generation + object_count - 1,
							next_generation + *cursor + count - 1);
		}
		count--;
	}
	pthread_barrier_wait(barrier);

	int start_local = id * (double)count / P;
	int end_local = (id + 1) * (double)count / P;
	if (end_local > count) {
		end_local = count;
	}
	for (int i = start_local; i < end_local - 1; i += 2) {
		crossover(current_generation + i, next_generation + *cursor + i, k);
	}
}

void free_resources(int id, int P, individual *current_generation,
					individual *next_generation) {
	int start = id * (double)current_generation -> chromosome_length / P;
	int end = (id + 1) * (double)current_generation -> chromosome_length / P;
	if (end > current_generation -> chromosome_length) {
		end = current_generation -> chromosome_length;
	}

	for (int i = start; i < end; ++i) {
		free(current_generation[i].chromosomes);
		current_generation[i].chromosomes = NULL;
		current_generation[i].fitness = 0;

		free(next_generation[i].chromosomes);
		next_generation[i].chromosomes = NULL;
		next_generation[i].fitness = 0;
	}
}

void *thread_function(void *arg) {
	struct my_arg* data = (struct my_arg*) arg;
	int id = data -> id;
	int P = data -> P;
	int object_count = data -> object_count;
	int generations_count = data -> generations_count;
	int sack_capacity = data -> sack_capacity;
	sack_object *objects = data -> objects;
	individual *current_generation = data -> current_generation;
	individual *next_generation = data -> next_generation;
	pthread_barrier_t *barrier = data -> barrier;

	int cursor;
	individual *tmp = NULL;

	// set initial generation
	// (composed of object_count individuals with a single item in the sack)
	int start = id * (double)object_count / P;
	int end = (id + 1) * (double)object_count / P;
	if (end > object_count) {
	    end = object_count;
	}

    set_initial_generation(start, end, object_count, current_generation,
							next_generation);

	pthread_barrier_wait(barrier);

	// iterate for each generation
	for (int k = 0; k < generations_count; ++k) {
		cursor = 0;
        compute_fitness(start, end, current_generation, objects, sack_capacity);
		pthread_barrier_wait(barrier);

		sort(id, object_count, P, current_generation, barrier);

		// keep first 30% children (elite children selection)
		elite_children_selection(object_count, id, P, &cursor,
									current_generation, next_generation);

		// mutate first 20% children with the 1st version of bit string mutation
		first_mutation(object_count, id, P, k, &cursor,
						current_generation, next_generation);

		// mutate next 20% children with the 2nd version of bit string mutation
		second_mutation(object_count, id, P, k, &cursor,
						current_generation, next_generation);

		// crossover first 30% parents with one-point crossover
		crossover_stage(object_count, id, P, k, &cursor,
						current_generation, next_generation, barrier);

		pthread_barrier_wait(barrier);

		tmp = current_generation;
		current_generation = next_generation;
		next_generation = tmp;

		for (int i = start; i < end; ++i) {
			current_generation[i].index = i;
		}

		if (k % 5 == 0 && id == 0) {
			print_best_fitness(current_generation);
		}

		pthread_barrier_wait(barrier);
	}

	compute_fitness(start, end, current_generation, objects, sack_capacity);
	pthread_barrier_wait(barrier);

	sort(id, object_count, P, current_generation, barrier);

	if (id == 0) {
		print_best_fitness(current_generation);
	}

	pthread_barrier_wait(barrier);

	free_resources(id, P, current_generation, next_generation);

	pthread_exit(NULL);
}
