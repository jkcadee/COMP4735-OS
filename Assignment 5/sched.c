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

static void schedInit();
static bool readJobsFile(char *fn);

static void usage() {
    printf("usage: ");
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

void schedRun() {
    RdyQueue *queue;
    RdyEntry *readyEntry;
    ProcessTableEntry *processEntry;

    while (schedInfo.readyCount >= 0) {
        int prior;
        for (prior = NUM_PRIOR - 1; prior >= 0; prior--) {
            queue = &schedInfo.rdyQueueList[prior];
            if (queue->count > 0) {
                break;
            } 

        }
    
        if (prior < 0) {
            break;
        }

        readyEntry = &queue->entries[queue->head];
        processEntry = processFind(readyEntry->pid);
        if (--processEntry->timeRemain == 0) {
            printf("%s is done\n", processEntry->name);
            // qhead
            queue->head++;
            queue->count--;
        }
        // qhead
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
        printf("Canno open '%s' for reading \n", fn);
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
    schedRun();
}
