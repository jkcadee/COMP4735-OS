/*
 * Virtual memory MMU
 * Illustrates operation of a MMU translating virtual addresses
 *
 * Memory reference file format:
 * Address in hex per line
 *
 * e.g.
 * 12
 * a5
 * e5
 * 67
 *
 * Page frame assignment file format:
 * Virtual page-Page frame
 *
 * e.g.
 * 0-3
 * 1-2
 * 3-0
 * 7-1
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define PAGE_TABLE_SZ 8

// page table entry struct
typedef struct {
    int virPage; // virtual page num
    int pageFrame; // page frame
} PageTableEntry;

static PageTableEntry pageTable[PAGE_TABLE_SZ];

// printing an integer as a binary number
static void
printIntAsBin(int val)
{
    // each one of these calculations determine if x % 2 = 0
    // if x % 2 = 0 then print 1, if not then print 0
    printf("%c%c%c%c%c%c%c%c", 
	val&0x80 ? '1':'0',
	val&0x40 ? '1':'0',
	val&0x20 ? '1':'0',
	val&0x10 ? '1':'0',
	val&0x8 ? '1':'0',
	val&0x4 ? '1':'0',
	val&0x2 ? '1':'0',
	val&0x1 ? '1':'0');
}

static bool
pageSetup(char *pageMapFn)
{
    FILE *fp; // file pointers 
    int virPage, pageFrame; // virtual page and page frame
    int index = 0;
    PageTableEntry* entry; 

    if (!(fp = fopen(pageMapFn, "r"))) {
        printf("Cannot open '%s' for reading\n", pageMapFn);
        return false;
    }
    // initialize page table
    for (int i = 0; i < PAGE_TABLE_SZ; i++) {
        pageTable[i].virPage = -1;
        pageTable[i].pageFrame = -1;
    }
    
    while (fscanf(fp, "%d-%d", &virPage, &pageFrame) == 2) { 
        // fill page table entry
        entry = &pageTable[index];

        // entry is filled 
        entry->virPage = virPage;
        entry->pageFrame = pageFrame;
        index++;
    }
    // close the file
    fclose(fp);

    printf("Page table:\n");
    for (int i=0; i<PAGE_TABLE_SZ; ++i) {
        // print virtual page to page frame mapping
        // print '-' if page is not in memory

        // if the vir page has been filled
        if (pageTable[i].virPage != -1) {
            // print out with the virtual page and page frame
            printf("%d: %d\n", pageTable[i].virPage, pageTable[i].pageFrame);
        } else {
            // else print without the page frame
            printf("%d: -\n", i);
        }
    }
    printf("\n");

    return true;
}

// run the file which outlines memRef
static bool
run(char *memRefFn)
{
    FILE *fp; // file pointer
    unsigned char virAddr; // new virtual address
    int offset, pageNumBits; // offset and page number bits
    int pageFrame = -1; // denotes if the page frame exists

    // if you cannot open the file, return false
    if (!(fp = fopen(memRefFn, "r"))) {
        printf("Cannot open '%s' for reading\n", memRefFn);
        return false;
    }

    printf("Run:\n");
    while (fscanf(fp, "%hhx", &virAddr) == 1) {  // if a hex number is found
        // compute page frame and physical addr by bitshifting 
        pageNumBits = (virAddr >> 5) & 0x3f;
        offset = virAddr & 0x1f;

        // find the corresponding page frame based on the stored page frame in the table and the number of page bits
        for (int i = 0; i < PAGE_TABLE_SZ; i++) {
            if(pageTable[i].virPage == pageNumBits) {
                pageFrame = pageTable[i].pageFrame;
                break;
            }
        }

        // newly created address
        int newAddr = (pageFrame << 5) | offset;

        // print the values, hex first
        printf("%02hhx ", virAddr);
        printf("(");
        printIntAsBin(virAddr);

        // print virtual address in hex, binary, and virtual page
        if (pageFrame != -1) {
            printf(", %d) -> %02hhx (", pageNumBits, newAddr);
            printIntAsBin(newAddr);
            printf(", %d)", pageFrame);
        } else {
            printf(", %d) -> ", pageNumBits);
            printf("Page fault"); // if a page frame is not found then print out a page fault
        }

        printf("\n");
        pageFrame = -1; // page frame is set to not existing anymore
        // print physical address in hex, binary, and page frame
    }

    fclose(fp); // close the file
    return true;
}

// usage for the function
static void
usage()
{
    printf("Usage: vmem [-memRef filename] [-pageMap filename]\n");
    exit(1);
}

int
main(int argc, char **argv)
{
    int i;
    char *memRefFn;
    char *pageMapFn;

    // default values
    memRefFn = "memRef.txt";
    pageMapFn = "pageMap.txt";

    // optional args should start with '-', and precede compulsory args
    for (i=1; i<argc; ++i) {
        if (argv[i][0] != '-')
            break;
        if (!strcmp(argv[i], "-memRef")) {
            // add support for -memRef and -pageMap options
            if (++i == argc) // if there is an issue with the arguments, print the usage
                usage();
            memRefFn = argv[i]; // file name becomes the new memRef file name
        } else if (!strcmp(argv[i], "-pageMap")) {
            if (++i == argc) // if there is an issue with the arguments, print the usage
                usage();
            pageMapFn = argv[i]; // file name becomes the new pageMap file name
        } else
            usage();
    }

    // 0 compulsory args
    if (i != argc)
        usage();

    pageSetup(pageMapFn);
    run(memRefFn);
}
