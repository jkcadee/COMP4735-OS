#include "prime.h"

int
prime(int numPrimesToFind, int *primeListSupplied, int argumentType)
{
  int primesCnt = 0;
  int n;
  int p;
  int lastPrime;
  int *primeList;

  if (primeListSupplied)
    primeList = primeListSupplied;
  else
    primeList = (int*)malloc(numPrimesToFind*sizeof(int));

  n = 2;
  while (primesCnt < numPrimesToFind) {
    for (p=0; p<primesCnt; ++p) {
      if (n%primeList[p] == 0)
        break;
    }
    if (p == primesCnt) {
      primeList[primesCnt++] = n;
      if (argumentType == 1) {
        if (primesCnt == numPrimesToFind) {
          printf("%d\n", n);
        } else {
          printf("%d ", n);
        }
      } 
    }
    ++n;
  }
  lastPrime = primeList[primesCnt-1];
  return lastPrime;
}
