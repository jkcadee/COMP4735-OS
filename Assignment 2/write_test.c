#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#define SYS "sys"
#define USER "usr"
#define ERROR_MESSAGE "Error! Incorrect argument 1 provided\n"
#define FILE_NAME "data"

int main(int argc, char *argv[]) {
    // converts 2nd argument to all lowercase (will be sys or usr)
    *argv[1] = tolower(*argv[1]);
    // compares 2nd argument with the constants SYS or USER
    // if SYS then it will go in this if statement, if USER it will go into the else if statement
    if (!strcmp(argv[1], SYS)) {
        // opens the file, will create it if it does not exist and will truncate to the beginning
        int fileDescriptor = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC);
        unsigned char b = 1;
        // for the number set in the 3rd argument, write this number of bytes into the file called data 
        for (int i = 0; i < atoi(argv[2]); i++)
        {
            write(fileDescriptor, &b, 1);   
        }
        // close the file
        close(fileDescriptor);
    } else if (!strcmp(argv[1], USER)) {
        // comments for above apply to this section, the only difference is that we are using fwrite instead of write
        FILE *filePointer = fopen(FILE_NAME, "w+");
        for (int i = 0; i < atoi(argv[2]); i++)
        {
            fwrite("A", 1, 1, filePointer);   
        }
        fclose(filePointer);
    } else {
        // if it is not SYS or USER give an error message and exit
        printf(ERROR_MESSAGE);
        exit(-1);
    }

    return 0;
}