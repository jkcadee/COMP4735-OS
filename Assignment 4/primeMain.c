/* 
This is a program that prints the Nth prime number
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "prime.h"

static void usage() {
        printf("Usage: primeMain [-list] a1\n");
        exit(1);
}

int main(int argc, char** argv) {
    int i;
    bool list = false;
    int numToFind;
    int n_th_prime;
    int *primeList;

    // optional args start with - and precede mandatory args
    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') { // no optional arg given
            break;
        }

        if (!strcmp(argv[i], "-list")) {
            list = true;
        } else {
            usage();
        }
    }

     // 1 mandatory arg
    if ((argc - i) != 1) {
        usage();
    }

    numToFind = atoi(argv[i++]);

    if (list) {
        primeList = (int*)malloc(numToFind*sizeof(int));
        n_th_prime = prime(numToFind, primeList);

        for (int i = 0; i < numToFind; i++) {
            printf("%d ", primeList[i]);
        }

        printf("\n");
        free(primeList);
    } else {
        n_th_prime = prime(numToFind, NULL);
    }

    printf("%d\n", n_th_prime);

}