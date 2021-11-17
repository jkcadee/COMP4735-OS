/*
 * Priority scheduler
 * Illustrates operation of a 4 level preemptive priority scheduler
 *
 * Job file format:
 * 1 line per priority level, starting from lowest level
 * each line: job name, length in clock slices, repeat
 *
 * e.g.
 * a0 6 b0 2 c0 11 d0 22
 * a1 3 b1 21 c1 12 d1 9
 * a2 5 b2 2 c2 8
 * a3 9
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define NUM_PRIOR 4
#define MAX_PROCESSES 32
#define MAX_PROCESS_NAME_LEN 16
#define MAX_RDY_QUEUE_LEN 8

// struct for ready entry
typedef struct RdyEntry {
    int pid;
} RdyEntry;

// struct for process table entry
typedef struct {
    char name[MAX_PROCESS_NAME_LEN];
    int pid;
    int timeRemain;
    int jobLen;
    int prior; // prior entry
} ProcessTableEntry;

// struct for process table
typedef struct {
    int cnt; // count
    int unusedPid;
    ProcessTableEntry entries[MAX_PROCESSES];
} ProcessTable;

// struct for a ready queue
typedef struct {
    int cnt;
    RdyEntry entries[MAX_RDY_QUEUE_LEN];
    int head;
    int timeSliceAllot;
    int timeSliceRemain;
} RdyQueue;

// struct for sched info
typedef struct {
    int rdyCnt;
    RdyQueue rdyQList[NUM_PRIOR];
} SchedInfo;

static ProcessTable processTable;
static SchedInfo schedInfo;
static bool sysActive; // denotes whether the system is active or not
static bool enableFairness = false;
static bool enableThread = false;
static int sleepAmount = 
    10;
    //1000; //ms

static bool readJobsFile(char *fn);
static void schedInit();
static void schedRun();
static void makeJobsRdy();
void * schedThread(void *arg);

// usage function for this program, describes optional parameters and required ones
static void
usage()
{
    printf("Usage: sched [-jobs filename]\n");
    exit(1);
}

// main function
int
main(int argc, char **argv)
{
    int i;
    char *jobsFn; 

    // default values
    jobsFn = "schedJobs.txt";

    // optional args should start with '-', and precede compulsory args
    for (i=1; i<argc; ++i) {
        if (argv[i][0] != '-')
            break;
        if (!strcmp(argv[i], "-jobs")) { 
            if (++i == argc)
                usage(); // display usage if there an issue with the given arguments
            jobsFn = argv[i];
	} else if (!strcmp(argv[i], "-fair")) {
	    enableFairness = true; // enables fairness
	} else if (!strcmp(argv[i], "-thread")) {
	    enableThread = true; // enables program to use threads
        } else
            usage(); // display usage if there an issue with the given arguments
    }

    // 0 compulsory args
    if (i != argc)
        usage();

    if (!readJobsFile(jobsFn)) // read jobs file, exit if there is an issue
        exit(1);
    schedInit(); // initialize the schedular info

    sysActive = true; // system is active
    if (enableThread) // if thread is enabled
    {
        pthread_t tid; // thread id
        char cmd[256]; // command line arg

        pthread_create(&tid, NULL, schedThread, 0); // create the thread that runs the scheduler info

        // while active...
        while (sysActive) {
            fprintf(stderr, "> ");
            fgets(cmd, 256, stdin);
            if (cmd[0] == 'q') { // if cmd[0] is q
                sysActive = false; // make system not active
            } else if (cmd[0] == 'r') { // if cmd[0] is r
                makeJobsRdy(); // make all jobs ready
            } else if (cmd[0] == 's') { // if cmd[0] is s
                sscanf(&cmd[1], "%d", &sleepAmount); // scan
            }
        }

        pthread_join(tid, NULL); // join threads
    } else {
	schedRun(); // if not threaded, just run the scheduler
    }
}

// reads the jobs file
static bool
readJobsFile(char *fn)
{
    FILE *fp;

    if (!(fp = fopen(fn, "r"))) { // if there is an error with opening the file
        // print an error message and return false
        printf("Cannot open '%s' for reading\n", fn);
        return false; 
    }

    {
#define MAX_LINE_LEN 80 // max line length for file
        char line[MAX_LINE_LEN];
        char *lp; // line pointer
        int numCharsRead; // number of characters read
        int jobLen; // length of a job
        ProcessTableEntry *pEntry; // entry in the process table
        char jobName[MAX_PROCESS_NAME_LEN]; // name of the process

        processTable.cnt = 0; // start at 0
        for (int p=0; p<NUM_PRIOR; ++p) {
            fgets(line, MAX_LINE_LEN, fp); // get a line based on the fp
            lp = line; // lp goes the current line
            while (sscanf(lp, "%s%d%n", jobName, &jobLen, &numCharsRead) == 2) {
                // fill out the process entry
                pEntry = &processTable.entries[processTable.cnt++];
                strncpy(pEntry->name, jobName, MAX_PROCESS_NAME_LEN);
                pEntry->jobLen = jobLen;
                pEntry->prior = p;
                pEntry->pid = processTable.unusedPid++;
                lp += numCharsRead; // add the number of characters read onto the line pointer to go to next line
            }
        }
    }

    // close the file and return true
    fclose(fp);
    return true;
}

// make the processes ready to be run
static void
makeJobsRdy()
{
    // variable initialization
    RdyQueue *rdyQueue;
    ProcessTableEntry *pEntry;
    RdyEntry *rdyEntry;

    // for each process in the process table
    for (int i=0; i<processTable.cnt; ++i) {
        // get the entry
        pEntry = &processTable.entries[i];
        // make the time that remains the length of the job
        pEntry->timeRemain = pEntry->jobLen;

        // put this entry in queue
        rdyQueue = &schedInfo.rdyQList[pEntry->prior];
        // get this ready entry
        rdyEntry = &rdyQueue->entries[rdyQueue->cnt++];
        // make this entry's pid the process entry's pid
        rdyEntry->pid = pEntry->pid;

        // increment ready count
        ++schedInfo.rdyCnt;
    }
}

// initialize scheduler 
static void
schedInit()
{
    // create scheduler 
    RdyQueue *rdyQueue;

    // initialize the scheduler queue with the corresponding variables
    for (int i=0; i<NUM_PRIOR; ++i) {
        rdyQueue = &schedInfo.rdyQList[i];
        rdyQueue->cnt = 0;
        rdyQueue->head = 0;
        rdyQueue->timeSliceAllot = i + 1;
        rdyQueue->timeSliceRemain = rdyQueue->timeSliceAllot;
    }

    // add processes into the ready queue
    makeJobsRdy();
}

// find an entry in the process table
static ProcessTableEntry *
processFind(int pid)
{
    for (int i=0; i<processTable.cnt; ++i) {
        if (processTable.entries[i].pid == pid)
            return &processTable.entries[i]; // return if found
    }
    return NULL; // return if not found
}

// run the scheduled processes
static void
schedRun()
{
    // initialize the needed variables
    RdyQueue *rdyQueue;
    RdyEntry *rdyEntry;
    ProcessTableEntry *pEntry;
    int prior;

    // while the system is active
    while (sysActive) {
    // if there are no processes ready and the program isn't using threads, break
	if (!enableThread && (schedInfo.rdyCnt == 0))
	    break;

        // find highest priority non-empty queue
        for (prior=NUM_PRIOR-1; prior >= 0; --prior) {
            rdyQueue = &schedInfo.rdyQList[prior];
            if (rdyQueue->cnt == 0)
                continue;
        // if the time slice that remains is 0, break
	    if (rdyQueue->timeSliceRemain > 0)
		break;
        }

	if (prior < 0) {
	    // no non-empty queue found
	    if (enableFairness) {
            // reset all queues to allocated time slice
            for (prior=NUM_PRIOR-1; prior >= 0; --prior) {
                rdyQueue = &schedInfo.rdyQList[prior];
                rdyQueue->timeSliceRemain = rdyQueue->timeSliceAllot;
            }
	    }
	    rdyQueue = NULL;
	}

	if (rdyQueue) {
        // if enable fairness was specified
	    if (enableFairness) {
            // decrement the time slice that remains
            --rdyQueue->timeSliceRemain;
	    }

	    for (int i=NUM_PRIOR-1; i >= 0; --i) {
		rdyEntry = &rdyQueue->entries[rdyQueue->head]; // get the first entry in the queue
		pEntry = processFind(rdyEntry->pid); // find the process based on pid
		if (--pEntry->timeRemain == 0) {
		    // process is done
		    printf("%s is done\n", pEntry->name);
		    --rdyQueue->cnt; // decrement count
            // for the head in the ready queue
		    for (int h=rdyQueue->head; h<rdyQueue->cnt; ++h) {
                // move the head down to the next entry
                rdyQueue->entries[h] = rdyQueue->entries[h+1];
		    }
            // decrement ready count
		    --schedInfo.rdyCnt;
		} else {
		    if (pEntry->timeRemain >= 0) // if the time remaining for the process is more than 0
			printf("%s %d\n", pEntry->name, pEntry->timeRemain); // say what the current remaining time is 
		}
		if (++rdyQueue->head >= rdyQueue->cnt) // if the head is greater than the count
		    rdyQueue->head = 0; // make the head back to 0
		break;
	    }
	    usleep(sleepAmount*1000); // sleep
	}
    }
}

// runs schedRun for threads
void *
schedThread(void *arg)
{
    schedRun();
    printf("Scheduler exited\n");
    return 0;
}
