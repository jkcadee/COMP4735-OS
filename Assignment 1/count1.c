#include <stdio.h>
#include <stdlib.h>

#define BASE_WORD_SIZE 255

int main(int argc, char *argv[]) {
	
	char word[255];
	int words = 0;
	
	// inputs file name, gets each word and incrments the words counter
	FILE * fp = fopen(argv[1], "r");
	
	if (fp == NULL) {
		printf("No file found.\n");
		exit(1);
	}
	
	while ((fscanf(fp, "%s", word)) != EOF) {
		words++;
	}
	
	printf("Num of words = %d\n", words);
	
	fclose(fp);
	return 0;
}
