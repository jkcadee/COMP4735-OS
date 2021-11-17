#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_PROCESS_NAME_LEN 20
#define MAX_PROCESSES 32
#define MAX_LINE_LEN 80
#define NUM_PRIOR 4
#define MAX_RDY_QUEUE_LEN 8

typedef struct RdyEntry {
    int pid;
} RdyEntry;

typedef struct {
    int count;
    RdyEntry entries[MAX_RDY_QUEUE_LEN];
    int head;
    int timeSliceAllot;
    int timeSliceRemain;
} RdyQueue;

typedef struct {
    int readyCount;
    RdyQueue rdyQueueList[NUM_PRIOR];
} SchedInfo;

typedef struct {
    char name[MAX_PROCESS_NAME_LEN];
    int pid;
    int timeRemain;
    int jobLen;
    int prior;
} ProcessTableEntry;

typedef struct {
    int count;
    int unusedPID;
    ProcessTableEntry entries[MAX_PROCESSES];
} ProcessTable;

static ProcessTable processTable;
static SchedInfo schedInfo;

static bool enableFairness = false;
static bool enableThread = false;
static bool sysActive;
static int sleepAmount = 10;

static void schedInit();
static bool readJobsFile(char *fn);

static void usage() {
    printf("usage: sched [-jobs filename]");
    exit(1);
}

static void makeJobsReady() {
    ProcessTableEntry *entry;
    RdyQueue *queue;
    RdyEntry *readyEntry;
    for (int i = 0; i < processTable.count; i++) {
        entry = &processTable.entries[i];
        queue = &schedInfo.rdyQueueList[entry->prior];
        readyEntry = &queue->entries[queue->count++];
        
        readyEntry->pid = entry->pid;
        entry->timeRemain = entry->jobLen;

        schedInfo.readyCount++;

        printf("Prior: %d, PID: %d, JobLen: %d\n", entry->prior, entry->pid, entry->jobLen);   
    }
}

void schedInit() {
    RdyQueue *queue;
    for (int i = 0; i < NUM_PRIOR; i++) {
        queue = &schedInfo.rdyQueueList[i];
        queue->count = 0;
        queue->head = 0;
        queue->timeSliceAllot = i + 1;
        queue->timeSliceRemain = queue->timeSliceAllot;
    }
    makeJobsReady();
}

static ProcessTableEntry * processFind(int pid) {
    for (int i = 0; i < processTable.count; i++) {
        if (processTable.entries[i].pid == pid) {
            return &processTable.entries[i];
        }
    }
    return NULL;
}

static void schedRun() {
    RdyQueue *queue;
    RdyEntry *readyEntry;
    ProcessTableEntry *processEntry;

    while (sysActive) {
        int prior;
        for (prior = NUM_PRIOR - 1; prior >= 0; prior--) {
            queue = &schedInfo.rdyQueueList[prior];
            if (queue->count > 0) {
                break;
            } 
            if (queue->timeSliceRemain > 0) {
                break;
            } 
        }

        if (prior < 0) {
            if (enableFairness) {
                for (prior = NUM_PRIOR-1; prior >= 0; prior--) {
                    queue = &schedInfo.rdyQueueList[prior];
                    queue->timeSliceRemain = queue->timeSliceAllot;
                }
            }
            queue = NULL;
        } 

        if (queue) {
            if (enableFairness) {
                queue->timeSliceRemain--;
            }
            for (prior = NUM_PRIOR-1; prior >= 0; prior--) {   
                readyEntry = &queue->entries[queue->head];
                processEntry = processFind(readyEntry->pid);
                if (--processEntry->timeRemain == 0) {
                    printf("%s is done\n", processEntry->name);
                    queue->count--;
                    for (int h = queue->head; h < queue->count; h++) {
                        queue->entries[h] = queue->entries[h++];
                    }
                    schedInfo.readyCount--;
                } else {
                    if (processEntry->timeRemain >= 0) {
                        printf("%s %d\n", processEntry->name, processEntry->timeRemain);
                    }
                }
                if (queue->head++ >= queue->count) {
                    queue->head = 0;
                }
                usleep(sleepAmount*1000);
            }
        }
    }
}

static bool readJobsFile(char* fn) {
    FILE *filepath;
    char *lp;
    char line[MAX_LINE_LEN];
    int numCharsRead;
    int jobLen;
    ProcessTableEntry *newEntry;
    char jobName[MAX_PROCESS_NAME_LEN];

    if (!(filepath = fopen(fn, "r"))) {
        printf("Cannot open '%s' for reading \n", fn);
        return false;
    }
    
    processTable.count = 0;

    for(int p = 0; p < NUM_PRIOR; ++p) {
        fgets(line, MAX_LINE_LEN, filepath);
        lp = line;
        while(sscanf(lp, "%s%d%n", jobName, &jobLen, &numCharsRead) == 2) {
            newEntry = &processTable.entries[processTable.count++];
            strncpy(newEntry->name, jobName, MAX_PROCESS_NAME_LEN);
            newEntry->jobLen = jobLen;
            newEntry->prior = p;
            newEntry->pid = processTable.unusedPID++;
            lp += numCharsRead;
        }
    }

    fclose(filepath);
    return true;
}

void * schedThread(void*arg) {
    schedRun();
    printf("Scheduler exited\n");
    return 0;
}

int main(int argc, char** argv) {
    int i;
    char *jobsFn;

    // default
    jobsFn = "schedJobs.txt";

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            break;
        }

        if (!strcmp(argv[i], "-jobs")) {
            if (++i == argc) {
                usage();
            }
            jobsFn = argv[i];
        } else if (!strcmp(argv[i], "-fair")) {
            enableFairness = true;
        } else if (!strcmp(argv[i], "-thread")) {
            enableThread = true;
        } else {
            usage();
        }
    }

    if (i != argc) {
        usage();
    }

    if (!readJobsFile(jobsFn)) {
        exit(1);
    }
    schedInit();

    sysActive = true;
    if (enableThread) {
        pthread_t tid;
        char cmd[256];
        pthread_create(&tid, NULL, schedThread, 0);

        while (sysActive) {
            fprintf(stderr, ">");
            fgets(cmd, 256, stdin);
            if (cmd[0] == 'q') {
                sysActive = false;
            } else if (cmd[0] == 'r') {
                makeJobsReady();
            } else if (cmd[0] == 's') {
                sscanf(&cmd[1], "%d", &sleepAmount);
            }
        }
        pthread_join(tid, NULL);
    } else {
        schedRun();
    }
}
