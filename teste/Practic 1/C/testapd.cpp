#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// #define P 2

struct my_arg {
    int id;
    int P;
    int N;
    // pthread_barrier_t *barrier;
}

/*
void get_args(int argc, char **argv)
{
	if(argc < 3) {
		printf("Numar insuficient de parametri: ./testapd N P\n");
		exit(1);
	}

	N = atoi(argv[1]);
	P = atoi(argv[2]);
}
*/

void *thread_function(void *arg) {
    struct my_arg *data = (struct my_arg*) arg;
    int id = data -> id;
    int P = data -> P;
    int N = data -> N;
    // pthread_barrier_t *barrier = data -> barrier;

    int start = id * (double)N / P;
    int end = (id + 1) * (double)N / P;
    if (end > N) {
        end = N;
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // get_args(argc, argv);

    int r;
    pthread_t *threads;
    struct my_arg *arguments;
    void *status;
    // pthread_barrier_t barrier;

    int N = 10, P = 2;

    threads = (pthread_t*) malloc(P * sizeof(pthread_t));
    arguments = (struct my_arg*) malloc(P * sizeof(struct my_arg));

    // pthread_barrier_init(&barrier, NULL, P);

    for (int i = 0; i < P; i++) {
        arguments[i].id = i;
        arguments[i].P = P;
        arguments[i].N = N;
        // arguments[i].barrier = &barrier;

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

    // pthread_barrier_destroy(&barrier);

    free(threads);
    free(arguments);
}
