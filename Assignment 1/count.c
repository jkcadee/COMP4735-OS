#include <stdio.h>
#include <stdlib.h>

#define BASE_WORD_SIZE 255

int main() {	
	char word[BASE_WORD_SIZE];
	int words = 0;
	FILE * fp = fopen("assign1.txt", "r");
	
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
