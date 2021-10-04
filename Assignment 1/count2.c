#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INCREMENT_MEM 10
#define BASE_WORD_SIZE 10

// struct that holds a word and how many times it appears
struct Words {
	char *word;
	int numberOfAppearances;
};



// reallocates memory if the array is too small
struct Words* memoryReallocationForArray(struct Words* uniqueWords, int usedLength, int *arraySize) {
	// if max arraysize has not been reached yet, return out
	if (usedLength < *arraySize) {
		return uniqueWords;
	}
	*arraySize *= 2;
	
	// reallocate array with two times size
	struct Words *reallocatedArray = (struct Words* ) realloc(uniqueWords, *arraySize * sizeof(struct Words));
	if (reallocatedArray == NULL) {
		printf("Could not allocate space for the array.\n");
		exit(1);
	}
	
	return reallocatedArray;
}

// uses bubble sort to get the struct with the word with the highest appearances
void sortArray(struct Words *uniqueWords, int arraySize) {
	for (int outerIndex = 0; outerIndex < arraySize - 1; outerIndex++) {
		for (int innerIndex = outerIndex + 1; innerIndex < arraySize - 1; innerIndex++) {
			if (uniqueWords[outerIndex].numberOfAppearances < uniqueWords[innerIndex].numberOfAppearances) {
				struct Words temp = uniqueWords[outerIndex];
				uniqueWords[outerIndex] = uniqueWords[innerIndex];
				uniqueWords[innerIndex] = temp;
			}
		}
	}
}

// resizes the string
char* resizeString(char* string, int stringIndex, int *stringLength) {
	if (stringIndex + 1 >= *stringLength) {
		*stringLength += INCREMENT_MEM;
		string = (char*) realloc(string, *stringLength);
		if (string == NULL) {
			printf("Could not allocate space for the string.\n");
			exit(1);
		}
	}
	return string;
}

// reads the file and populates the array with words
struct Words* readFileAndPopulateArray(char *fileName, struct Words* uniqueWords, int arraySize, int *numberOfElements) {
	FILE *file = fopen(fileName, "r");
	char *word = (char*) malloc(BASE_WORD_SIZE * sizeof(char));
	int index = 0;
	int wordSize = BASE_WORD_SIZE;
	
	if (file == NULL) {
		printf("No file found.\n");
		return NULL;
	}
	
	if (uniqueWords == NULL) {
		printf("Could not allocate space for the array.\n");
		exit(1);
	}
	
	if (word == NULL) {
		printf("Could not allocate space for the string.\n");
		exit(1);
	}
	
	// loops through file until the end, checks for unique words and adds onto appearances of words
	while ((fscanf(file, "%s", word)) != EOF) {
		int length = strlen(word);
		for (int i = 0; i < length; i++) {
			char currentCharacter = word[i];
			if (currentCharacter != ' ') {
				word = resizeString(word, index, &wordSize);
				word[index] = currentCharacter;
				index++;
			}
		}
		
		int isDuplicate = 0;
			
		tolower(*word);
		
		if (ispunct(word[length - 1])) {
			if (length - 1 == 0) {
				continue;
			}
			word[length - 1] = '\0';
		} 
		
		char *duplicatedWord = strdup(word);
		
		
		for (int i = 0; i < *numberOfElements; i++) {
			if (!strcmp(uniqueWords[i].word, duplicatedWord)) {
				uniqueWords[i].numberOfAppearances++;
				isDuplicate = 1;
				break;
			}
		}
		
		if (!isDuplicate) {
			(*numberOfElements)++;
			uniqueWords = memoryReallocationForArray(uniqueWords, *numberOfElements, &arraySize);
			struct Words uniqueWord = {duplicatedWord, 1};
			uniqueWords[*numberOfElements - 1] = uniqueWord;
		}
		
		free(duplicatedWord);
	}
	
	free(word);
	fclose(file);
	return uniqueWords;
}

int main(int argc, char *argv[]) {
	int size = INCREMENT_MEM;
	int numberOfElements = 0;

	// allocate size
	struct Words* uniqueWords = (struct Words*) malloc(size * (sizeof(struct Words)));
	
	uniqueWords = readFileAndPopulateArray(argv[1], uniqueWords, size, &numberOfElements);
	sortArray(uniqueWords, numberOfElements);
	
	// print the most common word
	printf("Most common word: %s\nNumber of appearances: %d\n", 
		uniqueWords[0].word, uniqueWords[0].numberOfAppearances);
		
	free(uniqueWords);
	
	return 0;
}
