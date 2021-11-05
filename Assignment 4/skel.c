/*
 * This is a skeleton program that processes cmd line arguments
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

// provides usage for the function
static void usage() {
    printf("Usage: skel [-o1 v] [-o2] a1 a2\n");
    exit(1);
}

int main(int argc, char **argv) {
    // variable initialization 
    int i; // index
    int o1; 
    bool o2;
    int a1;
    float a2;

    // default vals
    o1 = 10;
    o2 = false;

    // for each argument passed through the command line
    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') { // no optional arg given
            break;
        }
        // if the current argument is '-o1'
        if (!strcmp(argv[i], "-o1")) {
            // if there is an error with num of arguments, print out usage
            if (++i == argc) {
                usage();
            }
            // get the argument
            o1 = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-o2")) {
            o2 = true; // get true if an "-o2" is provided
        } else {
            // usage printed on incorrect arguments
            usage();
        }
    }
    
    // compulsory args
    if ((argc - i) != 2) {
        usage();
    }

    // get the integer and float arguments
    a1 = atoi(argv[i++]);
    a2 = atof(argv[i++]);

    // print all the provided arguments
    printf("o1=%d, o2=%s, a1=%d, a2=%f\n", o1, o2 ? "T" : "F", a1, a2);
}