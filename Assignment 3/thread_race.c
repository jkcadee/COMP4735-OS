#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct ThreadFuncArgs {
    int numOfLoops;
    char* threadName;
};

int globalVal = 0;
pthread_mutex_t lock;

void* threadLooping(void* vargp) {
    struct ThreadFuncArgs *args = (struct ThreadFuncArgs *)vargp;

    for (int i = 0; i < args->numOfLoops; i++) {
        printf("%s: %d\n",args->threadName, globalVal);
        globalVal++;
        for (int i = 0; i < args->numOfLoops; i++);
    }
    return NULL;
}

void* mutexThreadLooping(void* vargp) {
    pthread_mutex_lock(&lock);

    struct ThreadFuncArgs *args = (struct ThreadFuncArgs *)vargp;

    for (int i = 0; i < args->numOfLoops; i++) {
        printf("%s: %d\n",args->threadName, globalVal);
        globalVal++;
        for (int i = 0; i < args->numOfLoops; i++);
    }

    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(int argc, char *argv[]) {
    int threadAmount = 0;

    if (strcmp(argv[1], "-mutex") == 0) {
        for (int i = 0; i < argc; i++)
        {
            if (i > 2 && argv[i] != NULL) {
                threadAmount++;
            }

            if (argv[i] == NULL) {
                break;
            }

        }
    } else {
        for (int i = 0; i < argc; i++)
        {
            if (i > 1 && argv[i] != NULL) {
                threadAmount++;
            }

            if (argv[i] == NULL) {
                break;
            }
        }
    }

    pthread_t threads[threadAmount];

    struct ThreadFuncArgs args[threadAmount];

    if (strcmp(argv[1], "-mutex") == 0) {
        for (int i = 0; i < threadAmount; i++)
        {
            args[i].numOfLoops = atoi(argv[2]);
            args[i].threadName = argv[i + 3];
        }

        for (int i = 0; i < threadAmount; i++)
        {
            pthread_mutex_init(&lock, NULL);
            pthread_create(&threads[i], NULL, mutexThreadLooping, (void*)&args[i]);
        }
    } else {
        for (int i = 0; i < threadAmount; i++)
        {
            args[i].numOfLoops = atoi(argv[1]);
            args[i].threadName = argv[i + 2];
        }

        for (int i = 0; i < threadAmount; i++)
        {
            pthread_create(&threads[i], NULL, threadLooping, (void*)&args[i]);
        }
    }

    for (int i = 0; i < threadAmount; i++)
    {
        pthread_join(threads[i], NULL);
        pthread_mutex_destroy(&lock);
    }
        printf("Final: %d\n", globalVal); 
        pthread_exit(NULL);   
    return 0;

}