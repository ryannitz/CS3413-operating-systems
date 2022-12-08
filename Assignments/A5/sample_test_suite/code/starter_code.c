#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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



int doFree(int processId){
    return 0;
   }


int doAllocate(int howMuchToAllocate,int processId){

    switch (algorithm){
        case 1:
            {
                break;
            }
        case 2:
            {
                break;
            }
        case 3:
            {
                break;
            }
        default:
            {
                printf("There was an error, the algorithm is uninitialized");
                exit(0);
            }
    }
}

int calcFinalMemory(){
    return 0;
}
int getNumberOfChunks(){
    return 0;
}
int getSmallest(){
    return 0;
}
int getBiggest(){
    return 0;
}
int main (int argc , char** argv)
{
    int i  = 0;
    for (i = 0 ; i < argc; i++){
        if (strcmp(argv[i],"-b")==0){
            algorithm = 1;
        } else  if (strcmp(argv[i],"-w")==0){
            algorithm = 2;
        } else if (strcmp(argv[i],"-s")==0){
            memSize = atoi(argv[i+1]);
        } else  if (strcmp(argv[i],"-f")==0){
            algorithm = 3;
        }
    }
    if (memSize >= 0)
        {
            // initialize your memory here
        } else
        {
            printf("The program requires size\n");
            exit(0);
        }
    char operation;
    int id=1337;
    int size;
    while (EOF!=scanf("%c",&operation)){
        switch (operation){
            case 'N':
                scanf(" %d %d\n",&id,&size);
                doAllocate(size,id);
                break;
            case 'T':
                scanf(" %d\n",&id);
                doFree(id);
                break;
            case 'S':
                printf("Total Processes created %d, Total allocated memory %d, Total Processes\nterminated %d, Total freed memory %d, Final memory available %d, Final\nsmallest and largest fragmented memory sizes %d and %d, total failed requests:%d, number of memory chunks: %d\n",totalAllocated,totalMemAllocated,totalTerminated,totalFreedMemory,calcFinalMemory(),getSmallest(),getBiggest(),totalFailed,getNumberOfChunks());
                break;
        }
    }
    return 0;
}