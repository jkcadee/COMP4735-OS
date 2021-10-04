#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 50

struct ThreadFuncArgs {
    int numOfLoops;
    char* threadName;
};

int globalVal = 0;

void* threadLooping(void* vargp) {
    struct ThreadFuncArgs *args = (struct ThreadFuncArgs *)vargp;

    for (int i = 0; i < args->numOfLoops; i++) {
        printf("%s: %d\n",args->threadName, globalVal);
        globalVal++;
        for (int i = 0; i < args->numOfLoops; i++);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int threadAmount = 0;

    for (int i = 0; i < MAX_THREADS; i++)
    {
        if (i > 1 && argv[i] != NULL) {
            threadAmount++;
        }

        if (argv[i] == NULL) {
            break;
        }
    }

    pthread_t threads[threadAmount];

    struct ThreadFuncArgs args[threadAmount];
    
    for (int i = 0; i < threadAmount; i++)
    {
        args[i].numOfLoops = atoi(argv[1]);
        args[i].threadName = argv[i + 2];
    }
    

    for (int i = 0; i < threadAmount; i++)
    {
        pthread_create(&threads[i], NULL, threadLooping, (void*)&args[i]);
    }

    for (int i = 0; i < threadAmount; i++)
    {
        pthread_join(threads[i], NULL);
    }
        pthread_exit(NULL);    
    return 0;
}