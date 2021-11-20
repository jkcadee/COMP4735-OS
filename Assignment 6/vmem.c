#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SIZE_OF_PAGE_TABLE 100
#define SIZE_OF_ADDRESS 8

typedef struct {
    unsigned char num;
    int pageNumber;
    int mappedVal;
    char** address;
} PageTableEntry;

typedef struct {
    PageTableEntry entries[SIZE_OF_PAGE_TABLE];
} PageTable;

static PageTable table;

int convertToBinary(char *numToConvert) {
    PageTableEntry *entry;
    int index;
    int addressIndex = 0;
    char address[SIZE_OF_ADDRESS];
    while(numToConvert[index]) {
        switch (numToConvert[index]) {
        case '0':
                address[addressIndex] = 0;
                address[addressIndex + 1] = 0;
                address[addressIndex + 2] = 0;
                address[addressIndex + 3] = 0;
                break;
            case '1':
                address[addressIndex] = 0;
                address[addressIndex + 1] = 0;
                address[addressIndex + 2] = 0;
                address[addressIndex + 3] = 1;
                break;
            case '2':
                address[addressIndex] = 0;
                address[addressIndex + 1] = 0;
                address[addressIndex + 2] = 1;
                address[addressIndex + 3] = 0;
                break;
            case '3':
                address[addressIndex] = 0;
                address[addressIndex + 1] = 0;
                address[addressIndex + 2] = 1;
                address[addressIndex + 3] = 1;
                break;
            case '4':
                address[addressIndex] = 0;
                address[addressIndex + 1] = 1;
                address[addressIndex + 2] = 0;
                address[addressIndex + 3] = 0;
                break;
            case '5':
                address[addressIndex] = 0;
                address[addressIndex + 1] = 1;
                address[addressIndex + 2] = 0;
                address[addressIndex + 3] = 1;
                break;
            case '6':
                address[addressIndex] = 0;
                address[addressIndex + 1] = 1;
                address[addressIndex + 2] = 1;
                address[addressIndex + 3] = 0;
                break;
            case '7':
                address[addressIndex] = 0;
                address[addressIndex + 1] = 1;
                address[addressIndex + 2] = 1;
                address[addressIndex + 3] = 1;
                break;
            case '8':
                address[addressIndex] = 1;
                address[addressIndex + 1] = 0;
                address[addressIndex + 2] = 0;
                address[addressIndex + 3] = 0;
                break;
            case '9':
                address[addressIndex] = 1;
                address[addressIndex + 1] = 0;
                address[addressIndex + 2] = 0;
                address[addressIndex + 3] = 1;
                break;
            case 'A':
            case 'a':
                address[addressIndex] = 1;
                address[addressIndex + 1] = 0;
                address[addressIndex + 2] = 1;
                address[addressIndex + 3] = 0;
                break;
            case 'B':
            case 'b':
                address[addressIndex] = 1;
                address[addressIndex + 1] = 0;
                address[addressIndex + 2] = 1;
                address[addressIndex + 3] = 1;
                break;
            case 'C':
            case 'c':
                address[addressIndex] = 1;
                address[addressIndex + 1] = 1;
                address[addressIndex + 2] = 0;
                address[addressIndex + 3] = 0;
                break;
            case 'D':
            case 'd':
                address[addressIndex] = 1;
                address[addressIndex + 1] = 1;
                address[addressIndex + 2] = 0;
                address[addressIndex + 3] = 1;
                break;
            case 'E':
            case 'e':
                address[addressIndex] = 1;
                address[addressIndex + 1] = 1;
                address[addressIndex + 2] = 1;
                address[addressIndex + 3] = 0;
                break;
            case 'F':
            case 'f':
                address[addressIndex] = 1;
                address[addressIndex + 1] = 1;
                address[addressIndex + 2] = 1;
                address[addressIndex + 3] = 1;
                break;
        }
    }
}

int main(int argc, char **argv) {

}