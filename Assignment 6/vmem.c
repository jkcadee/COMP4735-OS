#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SIZE_OF_PAGE_TABLE 100
#define SIZE_OF_ADDRESS 8

typedef struct {
    unsigned char num;
    int mappedVal;
    char* address;
} PageTableEntry;

typedef struct {
    int arrayOfPageMap[SIZE_OF_ADDRESS];
    int count;
    PageTableEntry entries[SIZE_OF_PAGE_TABLE];
} PageTable;

static PageTable table;

void convertToBinary(char *numToConvert, PageTableEntry *entry) {
    int index = 0;
    int addressIndex = 0;
    char address[SIZE_OF_ADDRESS];
    while(numToConvert[index]) {
        switch (numToConvert[index]) {
            case '0':
                address[addressIndex] = '0';
                address[addressIndex + 1] = '0';
                address[addressIndex + 2] = '0';
                address[addressIndex + 3] = '0';
                break;
            case '1':
                address[addressIndex] = '0';
                address[addressIndex + 1] = '0';
                address[addressIndex + 2] = '0';
                address[addressIndex + 3] = '1';
                break;
            case '2':
                address[addressIndex] = '0';
                address[addressIndex + 1] = '0';
                address[addressIndex + 2] = '1';
                address[addressIndex + 3] = '0';
                break;
            case '3':
                address[addressIndex] = '0';
                address[addressIndex + 1] = '0';
                address[addressIndex + 2] = '1';
                address[addressIndex + 3] = '1';
                break;
            case '4':
                address[addressIndex] = '0';
                address[addressIndex + 1] = '1';
                address[addressIndex + 2] = '0';
                address[addressIndex + 3] = '0';
                break;
            case '5':
                address[addressIndex] = '0';
                address[addressIndex + 1] = '1';
                address[addressIndex + 2] = '0';
                address[addressIndex + 3] = '1';
                break;
            case '6':
                address[addressIndex] = '0';
                address[addressIndex + 1] = '1';
                address[addressIndex + 2] = '1';
                address[addressIndex + 3] = '0';
                break;
            case '7':
                address[addressIndex] = '0';
                address[addressIndex + 1] = '1';
                address[addressIndex + 2] = '1';
                address[addressIndex + 3] = '1';
                break;
            case '8':
                address[addressIndex] = '1';
                address[addressIndex + 1] = '0';
                address[addressIndex + 2] = '0';
                address[addressIndex + 3] = '0';
                break;
            case '9':
                address[addressIndex] = '1';
                address[addressIndex + 1] = '0';
                address[addressIndex + 2] = '0';
                address[addressIndex + 3] = '1';
                break;
            case 'A':
            case 'a':
                address[addressIndex] = '1';
                address[addressIndex + 1] = '0';
                address[addressIndex + 2] = '1';
                address[addressIndex + 3] = '0';
                break;
            case 'B':
            case 'b':
                address[addressIndex] = '1';
                address[addressIndex + 1] = '0';
                address[addressIndex + 2] = '1';
                address[addressIndex + 3] = '1';
                break;
            case 'C':
            case 'c':
                address[addressIndex] = '1';
                address[addressIndex + 1] = '1';
                address[addressIndex + 2] = '0';
                address[addressIndex + 3] = '0';
                break;
            case 'D':
            case 'd':
                address[addressIndex] = '1';
                address[addressIndex + 1] = '1';
                address[addressIndex + 2] = '0';
                address[addressIndex + 3] = '1';
                break;
            case 'E':
            case 'e':
                address[addressIndex] = '1';
                address[addressIndex + 1] = '1';
                address[addressIndex + 2] = '1';
                address[addressIndex + 3] = '0';
                break;
            case 'F':
            case 'f':
                address[addressIndex] = '1';
                address[addressIndex + 1] = '1';
                address[addressIndex + 2] = '1';
                address[addressIndex + 3] = '1';
                break;
        }
        index++;
        addressIndex = addressIndex + 4;
    }
    entry->address = address;
    printf("address: %s\n", table.entries[0].address);
}

void addMapping(char *mappedVal) {
    table.arrayOfPageMap[(int) mappedVal[0]] = mappedVal[strlen(mappedVal) - 1];
}

bool readPageMapFile(char *fn) {
    FILE *fp;

    if (!(fp = fopen(fn, "r"))) { // if there is an error with opening the file
        // print an error message and return false
        printf("Cannot open '%s' for reading\n", fn);
        return false; 
    }
    
    #define MAX_LINE_LEN 80 // max line length for file
    char line[MAX_LINE_LEN];

    while ((fscanf(fp, "%s", line)) != EOF) {
        addMapping(line);
    }
    

    // close the file and return true
    fclose(fp);
    return true;
}

bool readMemRefFile(char *fn) {
    FILE *fp;
    if (!(fp = fopen(fn, "r"))) { // if there is an error with opening the file
        // print an error message and return false
        printf("Cannot open '%s' for reading\n", fn);
        return false; 
    }
    
    #define MAX_LINE_LEN 80 // max line length for file
    char line[MAX_LINE_LEN];

    while ((fscanf(fp, "%s", line)) != EOF) {
        PageTableEntry* entry = &table.entries[table.count++];
        convertToBinary(line, entry);
        //printf("entry: address %s\n", table.entries[table.count].address);
        // for (int i = 0; i < table.count; i++) {
        //     printf("entry: address %s\n", table.entries[i].address);
        // }
    }

    // for (int i = 0; i < table.count; i++) {
    //     printf("entry: address %s\n", table.entries[i].address);
    // }

    // close the file and return true
    fclose(fp);
    return true;
}

int main(int argc, char **argv) {
    table.count = 0;

    for (int i = 0; i < argc; i++) {
        if (i == 1) {
            readMemRefFile(argv[i]);
        } else if (i == 2) {
            readPageMapFile(argv[i]);
        }
    }
    
    // for (int i = 0; i < table.count; i++) {
    //     printf("entry: address %s\n", table.entries[i].address);
    // }
    
    for (int i = 0; i < table.count; i++) {
        
    }
    

}
