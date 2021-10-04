#include "prime.c"

int main(int argc, char *argv[]) {
  int res = prime(atoi(argv[1]), NULL, atoi(argv[2]));

  if (strcmp(argv[2], "0") == 0) {
    printf("%d\n", res);
  }

  return 0;
}