/* 
This is a program that prints the Nth prime number
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "prime.h"

// provides usage for the function
static void usage() {
        printf("Usage: primeMain [-list] a1\n");
        exit(1);
}

int main(int argc, char** argv) {
    // variable initialization 
    int i;
    bool list = false; // will determine if all the prime vals are listed or not
    int numToFind; 
    int n_th_prime;
    int *primeList;

    // optional args start with - and precede mandatory args
    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') { // no optional arg given
            break;
        }

        if (!strcmp(argv[i], "-list")) { // flag list as true, all primes will be printed
            list = true;
        } else {
            // print usage if there are any issues with the arguments
            usage();
        }
    }

     // 1 mandatory arg
    if ((argc - i) != 1) {
        usage();
    }

    // get the number to find from the arguments
    numToFind = atoi(argv[i++]);

    // if list is true
    if (list) {
        // allocate a list of primes
        primeList = (int*)malloc(numToFind*sizeof(int));
        // find the n_th prime number
        n_th_prime = prime(numToFind, primeList);

        // print each value in the prime list
        for (int i = 0; i < numToFind; i++) {
            printf("%d ", primeList[i]);
        }

        printf("\n");
        // free the memory
        free(primeList);
    } else {
        // find the n_th prime number
        n_th_prime = prime(numToFind, NULL);
    }

    // print the n_th prime
    printf("%d\n", n_th_prime);

}