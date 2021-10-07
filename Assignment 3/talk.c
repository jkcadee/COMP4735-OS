#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void talk(int numberOfLines, char* talker) {
    for (int i = 0; i < numberOfLines; i++)
    {
        printf("%s: %d\n", talker, i);
        for (int i = 0; i < numberOfLines * numberOfLines; i++);
        for (int i = 0; i < numberOfLines * numberOfLines; i++);
        for (int i = 0; i < numberOfLines * numberOfLines; i++);
        for (int i = 0; i < numberOfLines * numberOfLines; i++);
        for (int i = 0; i < numberOfLines * numberOfLines; i++);
    }
}

int main(int argc, char* argv[]) {
    talk(atoi(argv[1]), argv[2]);
    return 0;
}