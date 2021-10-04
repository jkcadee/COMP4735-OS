#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <time.h>

#define MAX_COMMAND_CHARS 1000
#define MAX_COMMANDS_LIST 20
#define MAX_PATH_SIZE 100

// reads inputted arguments in the shell, saves them in history and copies them into an input string in main()
int read_line(char* string) {
    char* buffer = (char*) malloc(MAX_COMMAND_CHARS * sizeof(char));

    // capture buffer
    buffer = readline("\n> ");
    // if the length of the buffer isn't 0
    if (strlen(buffer) != 0){
        // add to history
        add_history(buffer);
        // copy into inputStr
        strcpy(string, buffer);
        free(buffer);
        return 0;
    } else {
        return 1;
    }
}

// parses special characters from inputted string, adds each element into the "parsed" array once parsed
void parsing_special_characters(char* string, char** parsed, char* specialCharacter, int* numOfArgs) {
    // for the maximum amount of commands 
    for (int i = 0; i < MAX_COMMANDS_LIST; i++)
    {
        parsed[i] = strsep(&string, specialCharacter);
        
        // number of arguments in the list goes up
        // if the length of the string is reached, break out of the loop
        if (parsed[i] == NULL) {
            break;
        } 
        // if the length of the string reaches 0, decrement i
        if (strlen(parsed[i]) == 0) {
            i--;
        }
        *numOfArgs = *numOfArgs + 1;
    }
    
}

// execute the arguments in parsedInfo
void exec_arguments(char** parsedInfo, int* numOfArgs) {
    int foundAmpersand = 0;

    if (!strcmp(parsedInfo[0], "exit")) {
        exit(0);
    }

    // if the final character of final argument is an "&" then mark it as a bg process
    if (strstr(parsedInfo[*numOfArgs - 1], "&")) {
        foundAmpersand = 1;
        parsedInfo[0][strlen(parsedInfo[0]) - 1] = '\0';
    }

    // forking into child process
    if (fork() == 0) {
        // run the command
        execvp(parsedInfo[0], parsedInfo);
        // exit the process
        exit(0);
    }  else if (strcmp(parsedInfo[*numOfArgs - 1], "&") && foundAmpersand != 1) {
        // if "&" is not added onto the end of the list of arguments
        int exStatus;
        // wait for process to end
        waitpid(-1, &exStatus, 0);
        // print exit status of child process
        printf("Child has exited with status: %d\n", WEXITSTATUS(exStatus));
    } 
    
}

// main func
int main() {
    // make space for both inputStr and parsedArgs
    char* inputStr = (char*) malloc(MAX_COMMAND_CHARS * sizeof(char));
    char** parsedArgs = (char**) malloc(MAX_COMMANDS_LIST * sizeof(char));
    // number of arguments in parsedArgs
    //status of shell loop
    int status = 1;
    while(status) {
        int numOfArgs = 0;
        if (read_line(inputStr)) {
            continue;
        }
        // parsing by special character
        parsing_special_characters(inputStr, parsedArgs, " ", &numOfArgs);
        // execute the commands
        exec_arguments(parsedArgs, &numOfArgs);
    }

    // free at the end
    free(inputStr);
    free(parsedArgs);
    return 0;
}