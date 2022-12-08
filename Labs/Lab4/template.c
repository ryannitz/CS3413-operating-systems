#include <stdio.h>
#include <stdlib.h>
#define OFFSETBITS 12
// fancy way of computing 2^OFFSETBITS 
#define PAGESIZE (1 << OFFSETBITS)
#define ARCH 32
int getPageNumber(int logicalAddress){
    return logicalAddress / PAGESIZE ; 
}

int getPageOffset(int logicalAddress){
    return logicalAddress % PAGESIZE ;
}
int getStartingAddressOfTheFrameByIndex(int indexInFramesArray){
    return indexInFramesArray*PAGESIZE ;
}

int main(int argc, char** argv){
    /* int* frames =  malloc the recommended array here*/;
    int* frames = (int*)malloc(pow(ARCH-PAGESIZE,2));
    int maxPreviouslyUsedFrame = 0;
    int logicalAddress;
    int i;
    int hasAFrameAssignedBefore = 0;
    int tempFrameIndex=0;
    while(EOF != scanf("%d\n",&logicalAddress) )
    {
        /** reset the value of hasAFrameAssignedBefore to 0 */
        hasAFrameAssignedBefore = 0;
        
        /* for all the values from 0 to maxPreviouslyUsedFrame, check if there's a page number of the current logical address in the frame array. if it is, mark hasAFrameAssignedBefore as 1 and store the index of the frame you just found into tempFrameIndex */
        // for(int i = 0; i < maxPreviouslyUsedFrame; i++){
        //     if(frames[i] == getPageNumber(logicalAddress)) {//check if the logical address is within the range of the frame.
        //        hasAFrameAssignedBefore = 1;
        //        tempFrameIndex = i;
        //        break; 
        //     }
        // }

        // I don't like using compile flags just to use a for-loop smh
        int index = 0;
        while(index < maxPreviouslyUsedFrame){
            if(frames[i] == getPageNumber(logicalAddress)) {//check if the logical address is within the range of the frame.
               hasAFrameAssignedBefore = 1;
               tempFrameIndex = i;
               break; 
            }
            index++;
        }
        /* After looking up through all the values from 0 to maxPreviouslyUsedFrame, if the page did not have a frame assigned before, assign the page number to the frame array
        in the maxPreviously used frame position, store the max previously used frame into tempFrameIndex,  increment the value of the maxPreviously used frame */
        if(!hasAFrameAssignedBefore) {
            frames[maxPreviouslyUsedFrame] = getPageNumber(logicalAddress);
            tempFrameIndex = maxPreviouslyUsedFrame;
            maxPreviouslyUsedFrame++;
        }   
        
        /* now that you have the physical frame index corresponding to your page stored in tempFrameIndex, get the starting address of the frame by index and add the page offset to it and voila, you have your physical address. Print it out in the format
        printf("Physical address is %i logical address is %i\n" ..);
        */
        int physicalAddress = getStartingAddressOfTheFrameByIndex(tempFrameIndex) + getPageOffset(logicalAddress);

        printf("Physical address is %i logical address is %i\n", physicalAddress, logicalAddress);

        printf("Read %i\n",logicalAddress);
        
    }
    free(frames);
}