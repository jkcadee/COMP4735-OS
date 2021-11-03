#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define Q_SIZE 10

int itemList[Q_SIZE];
int qHead;
int qTail;
int qCount;
int numToProduce;

enum {
    SYNC_SPIN,
    SYNC_SLEEP,
    SYNC_SEMA,
    SYNC_COND
} syncMethod;

bool producerSleeping, consumerSleeping;
sem_t producerWakeSema;
sem_t consumerWakeSema;

sem_t mutexSema;
sem_t hasRoomSema;
sem_t hasItemsSema;

pthread_cond_t producerWakeCond;
pthread_cond_t consumerWakeCond;
pthread_mutex_t mutex;

void * producerFunc(void *arg) {
    int n = 0;
    while (n < numToProduce) {
        switch (syncMethod) {
            case SYNC_SPIN:
                while(qCount == Q_SIZE) {
                    usleep(1);
                }
                break;
            case SYNC_SLEEP:
                if (qCount == Q_SIZE) {
                    for (int i = 0; i < 10000; i++) 
                    printf("producer sleep \n");
                    producerSleeping = true;
                    sem_wait(&producerWakeSema);
                    producerSleeping = false;
                    
                }
                pthread_mutex_lock(&mutex);
                break;
            case SYNC_SEMA:
                sem_post(&mutexSema);
                sem_post(&hasItemsSema);
                break;
            case SYNC_COND:
                // ???
                break;
        }
        // produce
        itemList[qTail++] = n;
        if (qTail == Q_SIZE) {
            qTail = 0;
        }

        ++qCount;
        
        printf("p %2d %d\n", qCount, n);
        ++n;
        usleep(1);
    }
    return 0;
}

void * consumerFunc(void *arg) {
    int n = 0;
    int prv_n = -1;
    while (true) {
        switch (syncMethod) {
        case SYNC_SPIN:
            while (qCount == 0) {
                usleep(1);
            }
            break;
        case SYNC_SLEEP:
            break;
        case SYNC_SEMA:
            break;
        case SYNC_COND:
            break;
        }

        n = itemList[qHead++];

        if ((n - prv_n) != 1) {
            printf("Consumer error at %d-%d\n", prv_n, n);
            exit(2);
        }

        prv_n = n;

        if (qHead == Q_SIZE) {
            qHead = 0;
        }

        --qCount;
        printf("c %2d %d \n", qCount, n);

        switch (syncMethod) {
        case SYNC_SPIN:
            break;
        case SYNC_SLEEP:
            break;
        case SYNC_SEMA:
            break;
        case SYNC_COND:
            break;
        }
        usleep(1);

        if (n == (numToProduce - 1)) {
            return 0;
        }
    }

    return 0;   
}

static void usage() {
    printf("Usage: producer sync(0=spin|1=sleep|2=sema|3=cond) numToProduce\n");
    exit(1);
}

int main(int argc, char **argv) {
    pthread_t producer, consumer;

    // process cmd line args
    if (argc != 3) {
        usage();
    }

    syncMethod = atoi(argv[1]);
    numToProduce = atoi(argv[2]);

    //initialize buffer to empty
    qHead = qTail = 0;
    qCount = 0;

    // start threads
    pthread_create(&producer, NULL, producerFunc, 0);
    pthread_create(&consumer, NULL, consumerFunc, 0);

    // wait for threads to end
    pthread_join(producer, 0);
    pthread_join(consumer, 0);

    return 0;
}  
