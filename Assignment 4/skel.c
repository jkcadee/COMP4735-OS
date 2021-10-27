/*
 * This is a skeleton program that processes cmd line arguments
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

static void usage() {
    printf("Usage: skel [-o1 v] [-o2] a1 a2\n");
    exit(1);
}

int main(int argc, char **argv) {
    int i;
    int o1;
    bool o2;
    int a1;
    float a2;

    // default vals
    o1 = 10;
    o2 = false;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            break;
        }

        if (!strcmp(argv[i], "-o1")) {
            if (++i == argc) {
                usage();
            }
            o1 = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-o2")) {
            o2 = true;
        } else {
            usage();
        }
    }
    
    // compulsory args
    if ((argc - i) != 2) {
        usage();
    }

    a1 = atoi(argv[i++]);
    a2 = atof(argv[i++]);

    printf("o1=%d, o2=%s, a1=%d, a2=%f\n", o1, o2 ? "T" : "F", a1, a2);
}