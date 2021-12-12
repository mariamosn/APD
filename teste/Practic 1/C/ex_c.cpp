#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

// #define P 2

struct my_arg {
    int id;
    int P;
    int N;
    pthread_barrier_t *barrier;
    int *v;
};


void get_args(int argc, char **argv, int *N, int *P)
{
	if(argc < 3) {
		printf("Numar insuficient de parametri: ./testapd N P\n");
		exit(1);
	}

	*N = atoi(argv[1]);
	*P = atoi(argv[2]);
}


void *thread_function(void *arg) {
    struct my_arg *data = (struct my_arg*) arg;
    int id = data -> id;
    int P = data -> P;
    int N = data -> N;
    pthread_barrier_t *barrier = data -> barrier;
    int *v = data -> v;

    int start = id * (double)N / P;
    int end = (id + 1) * (double)N / P;
    if (end > N) {
        end = N;
    }

    int pos[end - start] = {0};
    int initial[end - start] = {0};
    for (int i = start; i < end; i++) {
        initial[i - start] = v[i];
        for (int j = 0; j < N; j++) {
            if (i != j && v[j] < v[i]) {
                pos[i - start]++;
            }
        }
    }

    pthread_barrier_wait(barrier);

    for (int i = start; i < end; i++) {
        v[pos[i - start]] = initial[i - start];
    }

    pthread_barrier_wait(barrier);

    if (id == 0) {
        for (int i = 0; i < N; i++) {
            cout << v[i] << ' ';
        }
        cout << endl;
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int N, P;
    get_args(argc, argv, &N, &P);

    int r;
    pthread_t *threads;
    struct my_arg *arguments;
    void *status;
    pthread_barrier_t barrier;

    threads = (pthread_t*) malloc(P * sizeof(pthread_t));
    arguments = (struct my_arg*) malloc(P * sizeof(struct my_arg));

    int v[N];
    for (int i = 0; i < N; i++) {
        v[i] = N - i + 5;
    }

    pthread_barrier_init(&barrier, NULL, P);

    for (int i = 0; i < P; i++) {
        arguments[i].id = i;
        arguments[i].P = P;
        arguments[i].N = N;
        arguments[i].barrier = &barrier;
        arguments[i].v = v;

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

    free(threads);
    free(arguments);
}
