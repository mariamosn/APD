#include <stdlib.h>
#include <stdio.h>
#include <math.h>
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

//*******************************************************************

// merge function for merging two parts
void merge(int low, int mid, int high, individual *a)
{
    // individual* left = malloc((mid - low + 1) * sizeof(int));
    // individual* right = malloc((high - mid) * sizeof(int));
	individual left[mid - low + 1];
	individual right[high - mid];
 
    // n1 is size of left part and n2 is size
    // of right part
    int n1 = mid - low + 1, n2 = high - mid, i, j;
 
    // storing values in left part
    for (i = 0; i < n1; i++)
        left[i] = a[i + low];
 
    // storing values in right part
    for (i = 0; i < n2; i++)
        right[i] = a[i + mid + 1];
 
    int k = low;
    i = j = 0;
 
    // merge left and right in ascending order
    while (i < n1 && j < n2) {
		if (cmpfunc(&left[i], &right[j]) <= 0)
        	// if (left[i] <= right[j])
            a[k++] = left[i++];
        else
            a[k++] = right[j++];
    }
 
    // insert remaining values from left
    while (i < n1) {
        a[k++] = left[i++];
    }
 
    // insert remaining values from right
    while (j < n2) {
        a[k++] = right[j++];
    }

	// free(left);
	// free(right);
}
 
// merge sort function
void merge_sort_h(int low, int high, individual *v)
{
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
void merge_sort(int id, int N, int P, individual *v)
{ 
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

//**********************************************************************

void oets(individual *current_generation, int object_count, int id, int P, pthread_barrier_t *barrier, int *sorted) {
	int start_odd, start_even, end, sorted_aux;
	individual aux;

	// printf("HELLO! %d\n", id);

	start_odd = id * (double)object_count / P;
	end = (id + 1) * (double)object_count / P;
	if (end > object_count - 1) {
		end = object_count - 1;
	}

	if (start_odd % 2 == 0) {
		start_even = start_odd;
		start_odd++;
	} else {
		start_even = start_odd + 1;
	}
	// printf("id: %d, oc: %d, start odd: %d, start_even: %d, end: %d\n", id, object_count, start_odd, start_even, end);

	if (id == 0) {
		*sorted = 0;
	}
	pthread_barrier_wait(barrier);
	// implementati aici OETS paralel
	for (int k = 0; k < object_count && *sorted == 0; k++) {
		sorted_aux = 1;
		for (int i = start_even; i < end; i += 2) {
			if (cmpfunc(&current_generation[i], &current_generation[i + 1]) > 0) {
				aux = current_generation[i];
				current_generation[i] = current_generation[i + 1];
				current_generation[i + 1] = aux;
				sorted_aux = 0;
			}
		}
		
		pthread_barrier_wait(barrier);
		if (id == 0) {
			*sorted = 1;
		}

		for (int i = start_odd; i < end; i += 2) {
			if (cmpfunc(&current_generation[i], &current_generation[i + 1]) > 0) {
				aux = current_generation[i];
				current_generation[i] = current_generation[i + 1];
				current_generation[i + 1] = aux;
				sorted_aux = 0;
			}
		}

		if (sorted_aux == 0) {
			*sorted = 0;
		}
		
		pthread_barrier_wait(barrier);
	}
	// printf("%d DONE!!!\n", id);
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
	// int *sorted = data -> sorted;

	int cursor, count;
	individual *tmp = NULL;
	int weight;
	int profit;

	// set initial generation (composed of object_count individuals with a single item in the sack)
	int start = id * (double)object_count / P;
	int end = (id + 1) * (double)object_count / P;
	if (end > object_count) {
		end = object_count;
	}

	for (int i = start; i < end; ++i) {
		current_generation[i].fitness = 0;
		current_generation[i].chromosomes = (int*) calloc(object_count, sizeof(int));
		current_generation[i].chromosomes[i] = 1;
		current_generation[i].index = i;
		current_generation[i].chromosome_length = object_count;

		next_generation[i].fitness = 0;
		next_generation[i].chromosomes = (int*) calloc(object_count, sizeof(int));
		next_generation[i].index = i;
		next_generation[i].chromosome_length = object_count;
	}

	pthread_barrier_wait(barrier);

	///////////////////////////////////////////////////////////////

	// iterate for each generation
	for (int k = 0; k < generations_count; ++k) {
		cursor = 0;

		// compute fitness and sort by it
		// compute_fitness_function(objects, current_generation, object_count, sack_capacity);
		for (int i = start; i < end; ++i) {
			weight = 0;
			profit = 0;

			for (int j = 0; j < current_generation[i].chromosome_length; ++j) {
				if (current_generation[i].chromosomes[j]) {
					weight += objects[j].weight;
					profit += objects[j].profit;
				}
			}

			current_generation[i].fitness = (weight <= sack_capacity) ? profit : 0;
		}
		pthread_barrier_wait(barrier);

		/////////////////////////////////////////////////////////////

		// TODO: DE PARALELIZAT SORTAREA!!!!!
		if (id == 0) qsort(current_generation, object_count, sizeof(individual), cmpfunc);
		// pthread_barrier_wait(barrier);
		// oets(current_generation, object_count, id, P, barrier, sorted);
		/*
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
			// merge(0, (2 * object_count / 3 - 1) / 2, 2 * object_count / 3 - 1, current_generation);
			// merge(0, 2 * object_count / 3 - 1, object_count - 1, current_generation);
		} else if (P == 4) {
			int low0 = 0, high0 = ((double)object_count / P) - 1,
				high1 = 2 * ((double)object_count / P) - 1,
				low2 = 2 * ((double)object_count / P), high2 = 3 * ((double)object_count / P) - 1,
				high3 = object_count - 1;
			if (id == 0) merge(low0, high0, high1, current_generation);
			if (id == 1) merge(low2, high2, high3, current_generation);
			pthread_barrier_wait(barrier);
			if (id == 0) merge(low0, high1, high3, current_generation);
			// merge(0, (object_count / 2 - 1) / 2, object_count / 2 - 1, current_generation);
			// merge(object_count / 2, object_count / 2 + (object_count - 1 - object_count / 2) / 2, object_count - 1, current_generation);
			// merge(0, (object_count - 1) / 2, object_count - 1, current_generation);
		}
		*/
		pthread_barrier_wait(barrier);

		/////////////////////////////////////////////////////////////

		// keep first 30% children (elite children selection)
		count = object_count * 3 / 10;
		int start_local = id * (double)count / P;
		int end_local = (id + 1) * (double)count / P;
		if (end_local > count) {
			end_local = count;
		}
		for (int i = start_local; i < end_local; ++i) {
			copy_individual(current_generation + i, next_generation + i);
		}
		cursor = count;

		// mutate first 20% children with the first version of bit string mutation
		count = object_count * 2 / 10;
		start_local = id * (double)count / P;
		end_local = (id + 1) * (double)count / P;
		if (end_local > count) {
			end_local = count;
		}
		for (int i = start_local; i < end_local; ++i) {
			copy_individual(current_generation + i, next_generation + cursor + i);
			// TODO: DE PARALELIZAT MUTATIA => nu cred ca e necesar
			mutate_bit_string_1(next_generation + cursor + i, k);
		}
		cursor += count;

		// mutate next 20% children with the second version of bit string mutation
		count = object_count * 2 / 10;
		start_local = id * (double)count / P;
		end_local = (id + 1) * (double)count / P;
		if (end_local > count) {
			end_local = count;
		}
		for (int i = start_local; i < end_local; ++i) {
			copy_individual(current_generation + i + count, next_generation + cursor + i);
			// TODO: DE PARALELIZAT MUTATIA => nu cred ca e necesar
			mutate_bit_string_2(next_generation + cursor + i, k);
		}
		cursor += count;

		//////////////////////////////////////////////////////

		// crossover first 30% parents with one-point crossover
		// (if there is an odd number of parents, the last one is kept as such)
		count = object_count * 3 / 10;

		if (count % 2 == 1 && id == 0) {
			copy_individual(current_generation + object_count - 1, next_generation + cursor + count - 1);
			count--;
		}

		start_local = id * (double)(count / 2) / P;
		end_local = (id + 1) * (double)(count / 2) / P;
		if (end_local > count) {
			end_local = count;
		}
		for (int i = start_local; i < end_local; i += 2) {
			crossover(current_generation + i, next_generation + cursor + i, k);
		}

		pthread_barrier_wait(barrier);

		// switch to new generation
		// TODO: e nevoie sa se faca chestia asta pe un singur thread?
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

	// compute fitness and sort by it
	// compute_fitness_function(objects, current_generation, object_count, sack_capacity);
	for (int i = start; i < end; ++i) {
		weight = 0;
		profit = 0;

		for (int j = 0; j < current_generation[i].chromosome_length; ++j) {
			if (current_generation[i].chromosomes[j]) {
				weight += objects[j].weight;
				profit += objects[j].profit;
			}
		}

		current_generation[i].fitness = (weight <= sack_capacity) ? profit : 0;
	}
	pthread_barrier_wait(barrier);

	/////////////////////////////////////////////////////////////

	// TODO: DE PARALELIZAT SORTAREA!!!!!
	if (id == 0) qsort(current_generation, object_count, sizeof(individual), cmpfunc);
	pthread_barrier_wait(barrier);
	// oets(current_generation, object_count, id, P, barrier, sorted);

	//*************************************
/*
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
		// merge(0, (2 * object_count / 3 - 1) / 2, 2 * object_count / 3 - 1, current_generation);
		// merge(0, 2 * object_count / 3 - 1, object_count - 1, current_generation);
	} else if (P == 4) {
		int low0 = 0, high0 = ((double)object_count / P) - 1,
			high1 = 2 * ((double)object_count / P) - 1,
			low2 = 2 * ((double)object_count / P), high2 = 3 * ((double)object_count / P) - 1,
			high3 = object_count - 1;
		if (id == 0) merge(low0, high0, high1, current_generation);
		if (id == 1) merge(low2, high2, high3, current_generation);
		pthread_barrier_wait(barrier);
		if (id == 0) merge(low0, high1, high3, current_generation);
		// merge(0, (object_count / 2 - 1) / 2, object_count / 2 - 1, current_generation);
		// merge(object_count / 2, object_count / 2 + (object_count - 1 - object_count / 2) / 2, object_count - 1, current_generation);
		// merge(0, (object_count - 1) / 2, object_count - 1, current_generation);
	}
	pthread_barrier_wait(barrier);
*/
	//*****************************************

	if (id == 0) {
		print_best_fitness(current_generation);
	}

	pthread_barrier_wait(barrier);

	start = id * (double)current_generation->chromosome_length / P;
	end = (id + 1) * (double)current_generation->chromosome_length / P;
	if (end > current_generation->chromosome_length) {
		end = current_generation->chromosome_length;
	}
	for (int i = start; i < end; ++i) {
		free(current_generation[i].chromosomes);
		current_generation[i].chromosomes = NULL;
		current_generation[i].fitness = 0;

		free(next_generation[i].chromosomes);
		next_generation[i].chromosomes = NULL;
		next_generation[i].fitness = 0;
	}

	pthread_exit(NULL);
}

void do_magic(sack_object *objects, int object_count, int generations_count, int sack_capacity, int P) {
	individual *current_generation = (individual*) calloc(object_count, sizeof(individual));
	individual *next_generation = (individual*) calloc(object_count, sizeof(individual));

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

////////////////////////////////////////////////////////////////////////////////////////////////////

	// free resources for old generation
	// TODO: DE PARALELIZAT, DACA E CAZUL
	// free_generation(current_generation);
	// free_generation(next_generation);

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

	if (!read_input(&objects, &object_count, &sack_capacity, &generations_count, &P, argc, argv)) {
		return 0;
	}

	// run_genetic_algorithm(objects, object_count, generations_count, sack_capacity);
    do_magic(objects, object_count, generations_count, sack_capacity, P);

	free(objects);

	return 0;
}