#include "prime.c"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_THREADS 2

// function run when threads are created
void* runningPrimeThread(void *vargp) {
    prime(atoi((char*) vargp), NULL, 0);
    return 0;
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_OF_THREADS];

    // depending on the number sent (0 or anything else)
    // the prime values with be sequentially determined
    // or threads will be created to do these calculations
    if (atoi(argv[1]) == 0) {
        for (int i = 0; i < 2; i++)
        {
            prime(atoi(argv[2]), NULL, 0);
        }
    } else {
        pthread_create(&threads[0], NULL, runningPrimeThread, argv[2]);
        pthread_create(&threads[1], NULL, runningPrimeThread, argv[2]);
        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
        
        pthread_exit(NULL);        
    }


    return 0;
}