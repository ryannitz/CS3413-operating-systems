#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 1 - best fit
// 2 - worst fit
// 3 - first fit
int algorithm;
int memSize;
int totalAllocated = 0;
int totalMemAllocated = 0;
int totalFailed = 0;
int totalTerminated = 0;
int totalFreedMemory = 0;

int* stack;

int doFree(int processId) {
    int freed = 0;
    int index = 0;
    while (index < memSize) {
        if (stack[index] == processId) {
            stack[index] = 0;
            freed = 1;
            totalFreedMemory++;
        }
        index++;
    }
    if (freed) {
        totalTerminated++;
    } else {
        totalFailed++;
        printf("Process %d failed to free memory\n", processId);
    }

    return 0;
}

int bestFit(int howMuchToAllocate, int processId) {
    int bestFitChunk = memSize;
    int bestFitChunkStart = -1;
    int insideChunk = 0;
    int curChunk = 0;  // lol "kerchunk"
    int curChunkStart = -1;
    int index = 0;
    while (index < memSize) {
        if (!insideChunk && (stack[index] == 0)) {  // we start the chunk
            insideChunk = 1;
            curChunkStart = index;
        }
        if (insideChunk && (stack[index] == 0)) {  // chunk is getting chunkier
            curChunk++;
        }
        if (insideChunk && (stack[index] != 0) || (index + 1 == memSize)) {  // we end the chunk
            insideChunk = 0;
            if (curChunk <= bestFitChunk && curChunk >= howMuchToAllocate) {
                bestFitChunk = curChunk;
                bestFitChunkStart = curChunkStart;
            }
            curChunk = 0;
        }
        index++;
    }

    if (bestFitChunkStart != -1) {
        // set all the zeroes of smallest fitting chunk to hold memory for that processId
        int counter = 0;
        while (counter < howMuchToAllocate) {
            stack[bestFitChunkStart + counter] = processId;
            counter++;
        }
        totalAllocated++;
        totalMemAllocated += howMuchToAllocate;
    } else {
        printf("Process %d failed to allocate %d memory\n", processId, howMuchToAllocate);
        totalFailed++;
    }
    return 0;
}

int worstFit(int howMuchToAllocate, int processId) {
    int worstFitChunk = 0;
    int worstFitChunkStart = -1;
    int insideChunk = 0;
    int curChunk = 0;  // lol "kerchunk"
    int curChunkStart = -1;
    int index = 0;
    while (index < memSize) {
        if (!insideChunk && (stack[index] == 0)) {  // we start the chunk
            insideChunk = 1;
            curChunkStart = index;
        }
        if (insideChunk && (stack[index] == 0)) {  // chunk is getting chunkier
            curChunk++;
        }
        if (insideChunk && (stack[index] != 0) || (index + 1 == memSize)) {  // we end the chunk
            insideChunk = 0;
            if (curChunk > worstFitChunk && curChunk >= howMuchToAllocate) {
                worstFitChunk = curChunk;
                worstFitChunkStart = curChunkStart;
            }
            curChunk = 0;
        }
        index++;
    }

    if (worstFitChunkStart != -1) {
        // set all the zeroes of smallest fitting chunk to hold memory for that processId
        int counter = 0;
        while (counter < howMuchToAllocate) {
            stack[worstFitChunkStart + counter] = processId;
            counter++;
        }
        totalAllocated++;
        totalMemAllocated += howMuchToAllocate;
    } else {
        printf("Process %d failed to allocate %d memory\n", processId, howMuchToAllocate);
        totalFailed++;
    }
    return 0;
}

int firstFit(int howMuchToAllocate, int processId) {
    int wasAllocated = 0;
    int potentialFit = 0;
    int potentialStartLocation = -1;
    int index = 0;
    while (!wasAllocated && (index < memSize)) {
        if (stack[index] == 0) {
            // we hit our first empty byte
            if (potentialStartLocation == -1) {
                potentialStartLocation = index;
            }
            // we found a spot that fits
            if (potentialFit == howMuchToAllocate - 1) {
                // set all the zeroes to hold memory for that processId
                int counter = 0;
                while (counter < howMuchToAllocate) {
                    stack[potentialStartLocation + counter] = processId;
                    counter++;
                }
                wasAllocated = 1;  // we quit once it is allocated
            }
            potentialFit++;
        } else if (stack[index] == 1) {
            if (potentialFit < howMuchToAllocate) {
                // reset everything because we hit a 1 :(
                potentialFit = 0;
                potentialStartLocation = -1;
            }
        }
        index++;
    }

    if (!wasAllocated) {
        printf("Process %d failed to allocate %d memory\n", processId, howMuchToAllocate);
        totalFailed++;
    } else {
        totalAllocated++;
        totalMemAllocated += howMuchToAllocate;
    }
    return 0;
}

int doAllocate(int howMuchToAllocate, int processId) {
    switch (algorithm) {
        case 1: {
            bestFit(howMuchToAllocate, processId);
            break;
        }
        case 2: {
            worstFit(howMuchToAllocate, processId);
            break;
        }
        case 3: {
            firstFit(howMuchToAllocate, processId);
            break;
        }
        default: {
            printf("There was an error, the algorithm is uninitialized");
            exit(0);
        }
    }
}

// get final memory available
int calcFinalMemory() {
    int totalFree = 0;
    int index = 0;
    while (index < memSize) {
        if (stack[index] == 0) {
            totalFree++;
        }
        index++;
    }
    return totalFree;
}

int getNumberOfChunks() {
    int chunkCount = 0;
    int insideChunk = 0;
    int index = 0;
    while (index < memSize) {
        if (!insideChunk && (stack[index] == 0)) {
            insideChunk = 1;
            chunkCount++;
        }
        if (insideChunk && (stack[index] != 0)) {
            insideChunk = 0;
        }
        index++;
    }
    return chunkCount;
}

int getSmallest() {
    int smallestChunk = memSize + 1;  // not possible
    int insideChunk = 0;
    int curChunk = 0;  // lol "kerchunk"
    int index = 0;
    while (index < memSize) {
        if (!insideChunk && (stack[index] == 0)) {  // we start the chunk
            insideChunk = 1;
        }
        if (insideChunk && (stack[index] == 0)) {  // chunk is getting chunkier
            curChunk++;
        }
        if (insideChunk && ((stack[index] != 0) || (index + 1 == memSize))) {  // we end the chunk
            insideChunk = 0;
            if (curChunk < smallestChunk) {
                smallestChunk = curChunk;
            }
            curChunk = 0;
        }
        index++;
    }
    // could also just add a flag in the above code to check if we found a chunk... But this is the same :p
    if (smallestChunk > memSize) {
        return 0;
    }
    return smallestChunk;
}

int getBiggest() {
    int biggestChunk = -1;  // not possible
    int insideChunk = 0;
    int curChunk = 0;  // lol "kerchunk"
    int index = 0;
    while (index < memSize) {
        if (!insideChunk && (stack[index] == 0)) {  // we start the chunk
            insideChunk = 1;
        }
        if (insideChunk && (stack[index] == 0)) {  // chunk is getting chunkier
            curChunk++;
        }
        if (insideChunk && ((stack[index] != 0) || (index + 1 == memSize))) {  // we end the chunk
            insideChunk = 0;
            if (curChunk > biggestChunk) {
                biggestChunk = curChunk;
            }
            curChunk = 0;
        }
        index++;
    }
    // could also just add a flag in the above code to check if we found a chunk... But this is the same :p
    if (biggestChunk < 0) {
        return 0;
    }
    return biggestChunk;
}

int printStack() {
    int index = 0;
    while (index < memSize) {
        printf("%d, ", stack[index]);
        index++;
    }
    printf("\n");
    return 0;
}

/**
 * Compiled with `make`
 * Valgrind with: `valgrind --track-origins=yes --leak-check=full ./a.out -s 10 -f < ./in/test1.in` for all tests
 * Tested with `make && make 1 && make 2 && make 3 && make 4 && make 5 && make 6 && make 7 && make 8 && make 9 && make 10`
 */

int main(int argc, char** argv) {
    int i = 0;
    for (i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            algorithm = 1;
        } else if (strcmp(argv[i], "-w") == 0) {
            algorithm = 2;
        } else if (strcmp(argv[i], "-s") == 0) {
            memSize = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-f") == 0) {
            algorithm = 3;
        }
    }
    if (memSize >= 0) {
        // initialize your memory here
        stack = (int*)calloc(memSize, sizeof(int));
        // printStack();
    } else {
        printf("The program requires size\n");
        exit(0);
    }
    char operation;
    int id = 1337;  // lol
    int size;
    while (EOF != scanf("%c", &operation)) {
        switch (operation) {
            case 'N':
                scanf(" %d %d\n", &id, &size);
                doAllocate(size, id);
                break;
            case 'T':
                scanf(" %d\n", &id);
                doFree(id);
                break;
            case 'S':
                printf("Total Processes created %d, Total allocated memory %d, Total Processes\nterminated %d, Total freed memory %d, Final memory available %d, Final\nsmallest and largest fragmented memory sizes %d and %d, total failed requests:%d, number of memory chunks: %d\n", totalAllocated, totalMemAllocated, totalTerminated, totalFreedMemory, calcFinalMemory(), getSmallest(), getBiggest(), totalFailed, getNumberOfChunks());
                break;
        }
        // printStack();
    }
    free(stack);
    return 0;
}