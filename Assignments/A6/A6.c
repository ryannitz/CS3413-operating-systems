#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define OFFSETBITS 12
#define PAGESIZE (1 << OFFSETBITS)
#define PAGETABLEBITS 10
#define PAGEDIRBITS 10
#define PAGEDIRSIZE (1 << (PAGETABLEBITS + OFFSETBITS))
#define ARCH 32
int framesSize;
unsigned int pageFault = 0;
unsigned int pageHits = 0;
unsigned int pagesSwapped = 0;

unsigned int totalRequests = 0;
unsigned int copyOnWrite = 0;

unsigned long* frames;
int* useQueue;
int* bits;

/**



    Notes:
    -The Read/Write share pretty much Identical code and could absolutly be consolidated...
    I didn't because I was really pressed for time this week
    - I left all of my comments in to show my line of thought
    - I noticed I could put my three arrays into a struct holding three values... Too late to optimize


 */

int printFrames() {
    printf("Frames: {");
    int index = 0;
    while (index < framesSize) {
        printf("%i,", frames[index]);
        index++;
    }
    printf("]\n");
}

int printQueue() {
    printf("useQueue: {");
    int index = 0;
    while (index < framesSize) {
        printf("%i,", useQueue[index]);
        index++;
    }
    printf("]\n");
}

int updateLatestUse(int val) {
    // printFrames();
    int currentlyInStack = -1;
    int index = 0;
    while (index < framesSize) {
        if (useQueue[index] == val) {
            currentlyInStack = index;
            // we found it, so now we need to move it to the top and flow everything else down
            break;
        }
        index++;
    }

    index = framesSize - 1;
    if (currentlyInStack > -1) {
        // printf("Page %i IS in current useQueue\n", val);
        pageHits++;
        index = currentlyInStack - 1;
    } else {
        pageFault++;
        // printf("Page %i not in current useQueue\n", val);
    }

    while (index >= 0) {
        useQueue[index + 1] = useQueue[index];
        index--;
    }
    useQueue[0] = val;
    // printQueue();
}

unsigned long getPageNumber(unsigned long logicalAddress) {
    return logicalAddress / PAGESIZE;
}

unsigned long getPageOffsetAddress(unsigned long logicalAddress) {
    return logicalAddress % PAGESIZE;
}

void printPhysicalAddress(int frameID, unsigned long logicalAddress) {
    printf("%lu -> %i\n", logicalAddress, frameID * PAGESIZE + getPageOffsetAddress(logicalAddress));
    // printf("%lu -> %i\n",logicalAddress,0*PAGESIZE+getPageOffsetAddress(logicalAddress));
    // printf("%lu -> %i\n",logicalAddress,1*PAGESIZE+getPageOffsetAddress(logicalAddress));
    // printf("%lu -> %i\n",logicalAddress,2*PAGESIZE+getPageOffsetAddress(logicalAddress));
}

/**
 * @brief Absolutely cannot get the last value to be correct
 *
 * @return double
 */
double computeFormula() {
    // page fault rate is the amout of physical memory vs page size...
    //  printf("totalRequests: %i\n", totalRequests);
    //  printf("pageHits: %i\n", pageHits);
    //  printf("pageFaults: %i\n", pageFault);
    //  printf("swapped: %i\n", pagesSwapped);
    //  printf("2copy: %i\n", copyOnWrite);

    double pNoPageFault = (1.0 - (pageFault / (double)totalRequests));
    // printf("pNoPageFault: %f\n", pNoPageFault);
    // printf("pNoPageFault*10: %f\n", pNoPageFault*10);

    double pMajorPageFaultWithOneCopy = (pageFault / (double)totalRequests);
    // printf("pMajorPageFaultWithOneCopy: %f\n", pMajorPageFaultWithOneCopy);
    // printf("pMajorPageFaultWithOneCopy*1000: %f\n", pMajorPageFaultWithOneCopy*1000);

    // printf("Everything before last: %f\n", (pNoPageFault*10)+(pMajorPageFaultWithOneCopy*1000));
    double pMajorPageFaultWithTwoCopy = (copyOnWrite / (double)totalRequests);
    // printf("pMajorPageFaultWithTwoCopy: %f\n", pMajorPageFaultWithTwoCopy);
    // printf("pMajorPageFaultWithTwoCopy*3000: %f\n", pMajorPageFaultWithTwoCopy*3000);
    return (pNoPageFault * 10) + (pMajorPageFaultWithOneCopy * 1000) + (pMajorPageFaultWithTwoCopy * 3000);
}

int readFromAddress(unsigned long logicalAddress) {
    int pageHit = 0;
    int frameID = -1;
    int i = 0;
    // printFrames();
    // if frame contains page, no need to update the frames, only the latestUse
    while (i < framesSize) {  // probably a more arithmatic way of getting frameID
        if (frames[i] == getPageNumber(logicalAddress)) {
            // printf("Frame %i contains the page: %i\n", i, getPageNumber(logicalAddress));
            pageHit = 1;
            frameID = i;

            // this is never hit
            // if(bits[i] == 1) {
            // copyOnWrite++;
            //}
            updateLatestUse(getPageNumber(logicalAddress));
            break;
        }
        i++;
    }

    // if no frames contain the page, we need to:
    // overwrite the frame that containts the leastUsedPage (useQueue[frameSize-1]) with the new page,
    // and, updateLatestUse
    if (!pageHit) {
        // printf("No frame contains the page: %i\n", getPageNumber(logicalAddress));
        // find least used page (Should be the value stored in the last useQueue pos)
        int i = 0;
        while (i < framesSize) {
            // if the frame contains the page that is least used, swap it
            // printf("Least used pageNum: %i\n", useQueue[0]);
            // printf("frame %i contains: %i\n", i, frames[i]);
            if (frames[i] == useQueue[framesSize - 1]) {
                if (bits[i] == 1) {
                    pagesSwapped++;
                    // copyOnWrite++;//this breaks make 2
                }
                frameID = i;
                // printf("Page swap attempt at frame: %i\n", i);
                frames[i] = getPageNumber(logicalAddress);       // swap
                updateLatestUse(getPageNumber(logicalAddress));  // now the latest used page
                bits[i] = 0;
                break;
            }
            i++;
        }
    }
    // printFrames();
    printPhysicalAddress(frameID, logicalAddress);
}

int writeToAddress(unsigned long logicalAddress) {
    // same thing as read, but we need to check clean/dirty before adjusting the useQueue.
    int pageHit = 0;
    int frameID = -1;
    int i = 0;
    // printFrames();
    // if frame contains page, no need to update the frames, only the latestUse and clean/dirty
    while (i < framesSize) {  // probably a more arithmatic way of getting frameID
        if (frames[i] == getPageNumber(logicalAddress)) {
            // printf("Frame %i contains the page: %i\n", i, getPageNumber(logicalAddress));

            pageHit = 1;
            frameID = i;
            bits[i] = 1;
            updateLatestUse(getPageNumber(logicalAddress));
            break;
        }
        i++;
    }

    // if no frames contain the page, we need to:
    // overwrite the frame that containts the leastUsedPage (useQueue[frameSize-1]) with the new page,
    // and, updateLatestUse
    if (!pageHit) {
        // TODO: NOT SWAPPING
        // printf("No frame contains the page: %i\n", getPageNumber(logicalAddress));
        // find least used page (Should be the value stored in the last useQueue pos)
        int i = 0;
        while (i < framesSize) {
            // if the frame contains the page that is least used, swap it
            // printf("Least used pageNum: %i\n", useQueue[0]);
            // printf("frame %i contains: %i\n", i, frames[i]);
            if (frames[i] == useQueue[framesSize - 1]) {
                if (bits[i] == 1) {
                    pagesSwapped++;
                    copyOnWrite++;
                }
                bits[i] = 1;  // set dirty AFTER writting to it
                frameID = i;
                // printf("Page swap attempt at frame: %i\n", i);
                frames[i] = getPageNumber(logicalAddress);       // swap
                updateLatestUse(getPageNumber(logicalAddress));  // now the latest used page
                break;
            }
            i++;
        }
    }
    // printFrames();
    printPhysicalAddress(frameID, logicalAddress);
}

int main(int argc, char** argv) {
    framesSize = atoi(argv[1]);
    frames = (unsigned long*)malloc(pow(2, ARCH - PAGESIZE));
    useQueue = (int*)calloc(framesSize, sizeof(int));
    bits = (int*)calloc(framesSize, sizeof(int));
    unsigned long logicalAddress;
    char operation;
    printf("Logical addresses -> Physical addresses:\n");
    while (EOF != scanf("%c %lu\n", &operation, &logicalAddress)) {
        if (operation == 'r') {
            totalRequests++;
            // printf("Logical addr: %lu, pagenum: %i\n", logicalAddress, getPageNumber(logicalAddress));
            readFromAddress(logicalAddress);
        } else {
            totalRequests++;  // leaving these in the control structure for safety
            writeToAddress(logicalAddress);
        }
    }

    printf("\nStats:\nmajor page faults = %u\npage hits = %u\npages swapped out = %u\nEffective Access Time = %.4f\n", pageFault, pageHits, pagesSwapped, computeFormula());
    free(frames);
    return 0;
}