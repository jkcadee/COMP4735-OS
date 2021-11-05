#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define Q_SIZE 10

// variable initialization 
// these are global variables to be used throughout the program
int itemList[Q_SIZE];
int qHead;
int qTail;
int qCount;
int numToProduce;

// enumeration for each case to run the producer under
enum {
    SYNC_SPIN,
    SYNC_SLEEP,
    SYNC_SEMA,
    SYNC_COND
} syncMethod;

// booleans for sleeping for sync_sleep
bool producerSleeping, consumerSleeping;

// semaphore initialization for sync_sleep
sem_t producerWakeSema;
sem_t consumerWakeSema;

// semaphore initialization for sync_sema
sem_t mutexSema;
sem_t hasRoomSema;
sem_t hasItemsSema;

// mutex and cond initialization
pthread_cond_t producerWakeCond;
pthread_cond_t consumerWakeCond;
pthread_mutex_t mutex;

// producer function
void * producerFunc(void *arg) {
    int n = 0;
    // loop until num to produce is met
    while (n < numToProduce) {
        // pick a sync based on what number was used in the argument
        switch (syncMethod) { 
        case SYNC_SPIN: // 0
            while(qCount == Q_SIZE) {
                usleep(1); // waste time
            }
            break;
        case SYNC_SLEEP: // 1
            if (qCount == Q_SIZE) {
                for (int i = 0; i < 10000; i++) // waste time 
                printf("producer sleep \n");
                producerSleeping = true; // flag producer as sleeping
                sem_wait(&producerWakeSema); // locks producer wake sema
                producerSleeping = false; // flag producer as not sleeping
            }
            pthread_mutex_lock(&mutex); // lock the mutex
            break;
        case SYNC_SEMA: // 2
            sem_wait(&hasRoomSema); // decrements (locks) the hasRooms sema
            sem_wait(&mutexSema); // decrements (locks) the mutex sema
            break;
        case SYNC_COND: // 3
            pthread_mutex_lock(&mutex); // locks the mutex
            while (qCount == Q_SIZE) { // while the queue is full...
                pthread_cond_wait(&producerWakeCond, &mutex); // have the cond var be blocked
            }
            break;
        }
        // produce
        // add an item into the queue
        itemList[qTail++] = n;

        // if the queue is full, make the tail = 0
        // so we can start from the beginning of the queue again next loop
        if (qTail == Q_SIZE) {
            qTail = 0;
        }

        // increment queue count
        ++qCount;
        
        printf("p %2d %d\n", qCount, n);
        
        // increment as a number has been produced
        ++n;

        // pick a sync based on what number was used in the argument
        switch (syncMethod) {
        case SYNC_SPIN:
            for (int i = 0; i < 1000000; i++) // waste time
            break;
        case SYNC_SLEEP:
            pthread_mutex_unlock(&mutex); // unlock prior locked mutex
            // if there is 1 item in the queue and the consumer is also sleeping
            // wake up the consumer by unlocking the consumerWake sema
            if ((qCount == 1) && consumerSleeping) {
                printf("Wake consumer\n");
                sem_post(&consumerWakeSema);
            }
            break;
        case SYNC_SEMA:
            sem_post(&mutexSema); // increments (unlocks) the mutex sema
            sem_post(&hasItemsSema); // increments (unlocks) the hasItems sema
            break;
        case SYNC_COND:
            pthread_cond_signal(&consumerWakeCond); // signals the condition variable to be unlocked for the consumer
            pthread_mutex_unlock(&mutex); // unlocks mutex
            break;
        }

        usleep(1); // ms of sleep for wasting time
    }
    return 0;
}

void * consumerFunc(void *arg) {
    int n = 0;
    int prv_n = -1; // previous val from n
    while (true) { 
        // pick a sync based on what number was used in the argument
        switch (syncMethod) {
        case SYNC_SPIN:
            while (qCount == 0) {
                usleep(1); // waste time
            }
            break;
        case SYNC_SEMA:
            sem_wait(&hasItemsSema); // decrements (locks) the hasItems sema
            sem_wait(&mutexSema); // decrements (locks) the mutex sema
            break;
        case SYNC_COND:
            pthread_mutex_lock(&mutex); // mutex is locked
            // while the queue is empty, we have the consumer wait until the queue has an item in it
            while (qCount == 0) { 
                pthread_cond_wait(&consumerWakeCond, &mutex);
            }
            break;
        default:
            if (qCount == 0) {
                for (int i = 0; i < 10000; i++); // waste time
                printf("consumer sleep \n");
                consumerSleeping = true; // flag consumer as sleeping
                sem_wait(&consumerWakeSema); // locks the consumerWake sema
                consumerSleeping = false; // flag the consumer as not sleeping
            }
            break;
        }

        // consume
        // take an item from the head
        n = itemList[qHead++];

        // if the items are consumed to quickly that the n - prv_n is somehow not equal to 1
        // then flag as error, print and exit 
        if ((n - prv_n) != 1) {
            printf("Consumer error at %d-%d\n", prv_n, n);
            exit(2);
        }

        // current n becomes the previous n
        prv_n = n;

        // if the queue head is the size of the queue, make it back to 0 again
        // to start counting from the beginning
        if (qHead == Q_SIZE) {
            qHead = 0;
        }

        // decrement count
        --qCount;
        printf("c %2d %d \n", qCount, n);

        // pick a sync based on what number was used in the argument
        switch (syncMethod) {
        case SYNC_SPIN:
            for (int i = 0; i < 1000000; i++) // waste time
            break;
        case SYNC_SLEEP:
            pthread_mutex_unlock(&mutex); // unlock mutex
            // if the queue count is one less than the size of the queue
            // and the producer is sleeping
            // wake up the producer to start making more items
            if ((qCount == (Q_SIZE - 1)) && producerSleeping) {
                printf("Wake producer\n");
                sem_post(&producerWakeSema);
            }
            break;
        case SYNC_SEMA:
            sem_post(&mutexSema); // increments (unlocks) the mutex sema
            sem_post(&hasRoomSema); // increments (unlocks) the hasRoom sema
            break;
        case SYNC_COND:
            pthread_cond_signal(&producerWakeCond); // signals the condition variable to be unlocked for producer
            pthread_mutex_unlock(&mutex); // unlocks mutex
            break;
        }
        usleep(1); // sleep for wasting time

        // if the consumer has consumed all there is to be consumed, return and exit the function
        if (n == (numToProduce - 1)) {
            return 0;
        }
    }

    return 0;   
}

// provides usage for the function
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

    // get command line arguments
    syncMethod = atoi(argv[1]);
    numToProduce = atoi(argv[2]);

    //initialize buffer to empty
    qHead = qTail = 0;
    qCount = 0;

    // pick a sync based on what number was used in the argument
    switch (syncMethod) {
    case SYNC_SPIN:
        break;
    case SYNC_SLEEP: // initialize all the needed mutexes and semaphores
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&producerWakeCond, NULL);
        sem_init(&producerWakeSema, 0, 0);
        sem_init(&consumerWakeSema, 0, 0);
        producerSleeping = consumerSleeping = false; // both sleeping states are false at the beginning
        break;
    case SYNC_SEMA: // initialize all needed semaphores
        sem_init(&mutexSema, 0, 1);
        sem_init(&hasRoomSema, 0, Q_SIZE);
        sem_init(&hasItemsSema, 0, 0);
        break;
    case SYNC_COND: // initialize all the needed mutexes and conditional variables
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&producerWakeCond, NULL);
        pthread_cond_init(&consumerWakeCond, NULL);
        break;
    }

    // start threads
    pthread_create(&producer, NULL, producerFunc, 0);
    pthread_create(&consumer, NULL, consumerFunc, 0);

    // wait for threads to end
    pthread_join(producer, 0);
    pthread_join(consumer, 0);

    // pick a sync based on what number was used in the argument
    switch (syncMethod) {
    case SYNC_SPIN:
        break;
    case SYNC_SLEEP: // destroy mutexes and semaphores used in the function
        pthread_mutex_destroy(&mutex);
        sem_destroy(&producerWakeSema);
        sem_destroy(&consumerWakeSema);
        break;
    case SYNC_SEMA:
        sem_destroy(&mutexSema);
        sem_destroy(&hasRoomSema);
        sem_destroy(&hasItemsSema);
        break;
    case SYNC_COND:
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&producerWakeCond);
        pthread_cond_destroy(&consumerWakeCond);
        break;
    }

    return 0;
}  
