#include <stdio.h>
#include <stdlib.h>
//#define OFFSETBITS 12

/*
    Compiled with: gcc -Wall -Werror -w -g L5.c

*/
int main(int argc, char** argv) {
    char text[] = "Hello world, this is my attempt to write to a file using the freade() operation.";
    int cursor = 0;
    char charVal;

    // 1. writing to file
    FILE* fp = NULL;
    fp = fopen("answers.txt", "w+");
    fwrite(text, 1, sizeof(text) - 1, fp);
    // fclose(fp);
    printf("Successfully written file\n");

    // 2. making correction
    // a,b
    fseek(fp, 0, SEEK_SET);
    // c
    fseek(fp, 62, SEEK_SET);
    // d
    cursor = ftell(fp);
    printf("Current pos: %ld\n", cursor);
    // e
    charVal = fgetc(fp);
    printf("Char at cursor: %c\n", charVal);
    // f
    cursor = ftell(fp);
    printf("Current pos: %ld\n", cursor);
    // g
    fseek(fp, -1, SEEK_CUR);
    cursor = ftell(fp);
    printf("Current pos: %ld\n", cursor);
    // h
    char correction[] = "writ";
    fwrite(correction, 1, sizeof(correction) - 1, fp);

    // 3
    fseek(fp, 0, SEEK_END);
    cursor = ftell(fp);
    printf("Current pos: %ld\n", cursor);
    // 4
    fseek(fp, 2, SEEK_CUR);
    cursor = ftell(fp);
    printf("Current pos: %ld\n", cursor);
    // 5
    char hi[] = "hi";
    fwrite(hi, 1, sizeof(hi) - 1, fp);
    // 6
    fclose(fp);

    // 7,8
    /*
        The two periods represent the two empty holes being filled in with the last known char,
        in this case '.', so that the entire string can be read,
        instead of kicking out when it hits an empty value which would give EOF
    */

    return 0;
}