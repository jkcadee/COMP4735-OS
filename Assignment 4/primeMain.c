#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "prime.h"

static void usage() {
    printf("Usage: prime [-list] numToFind\n");
    exit(1);
}

int main(int argc, char **argv) {
    int i;
    bool list;
    int numToFind;
    int n_th_prime;
    int *primeList;

    // default val
    list = false;

    for (i = 0; i < argc; i++)
    {
        if (argv[i][0] != '-') {
            break;
        }

        if (!strcmp(argv[i], "-list")) {
            list = true;
        } else {
            usage();
        }
    }

    if ((argc - i) != 1) {
        usage();
    }

    numToFind = atoi(argv[i++]);

    if (list) {
        primeList = (int*) malloc(numToFind*sizeof(int));
        n_th_prime = prime(numToFind, primeList);
        for (int i = 0; i < numToFind; i++)
        {
            printf("%d", primeList[i]);
        }
        printf("\n");
        free(primeList);
    } else {
        n_th_prime = prime(numToFind, NULL);
    }
    
    printf("%d\n", n_th_prime);
}