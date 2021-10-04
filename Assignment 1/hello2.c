#include <stdio.h>
#include <stdlib.h>

#define MALLOC_SIZE 99

int main() {
	int* pointer = (int*) malloc(MALLOC_SIZE);
	
	printf("Hello\n");
	
	free(pointer);
	return 0;
}
