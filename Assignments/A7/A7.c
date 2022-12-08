#include <stdio.h>
#include <stdlib.h>
char currentDirection;

int diskSize = 10000;
int disk[9999];

typedef struct _request {
    int position;
    int arrivalTime;
    int serviced;
    struct _request* next;
} Request;
Request* head = NULL;

void enqueue(Request** head, Request* newRequest) {
    if (*head == NULL) {
        *head = newRequest;
        return;
    }
    Request* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newRequest;
}

void printRequest(Request* request) {
    if(request != NULL) {
        printf("position: %i, arrivalTime: %i, isServiced: %i\n", 
        request->position, request->arrivalTime, request->serviced);
    }else {
        printf("Cannot print this request\n");
    }
    
}

void printQueue() {
    Request *cur = head;
    if(cur != NULL) {
        while(cur != NULL) {
            printRequest(cur);
            cur = cur->next;
        }
    }else {
        printf("Request queue is empty\n");
    }
}


void fcfs(int start){
    int movement = 0;
    double time = 0;

    double lastRequestArrivalTime = 0.00;
    int requestMovement = 0;
    int headPosition = start; //makes more sense to me

    double dirChangeTime = 15.00;
    double travelTimeRatio = 5.00;

    Request *request = head;
    if(request != NULL) {
        while(request != NULL) {
            if(time < request->arrivalTime) {
                time = time + (request->arrivalTime - lastRequestArrivalTime);
            }
            lastRequestArrivalTime = request->arrivalTime;

            if(currentDirection == 'a') {
                if(headPosition <= request->position){
                    requestMovement = request->position-headPosition;
                }else {
                    time = time + dirChangeTime;
                    requestMovement = headPosition-request->position;
                    currentDirection = 'd';
                }
            }else {
                if(headPosition >= request->position){
                    requestMovement = headPosition-request->position;
                }else {
                    time = time + dirChangeTime;
                    requestMovement = request->position-headPosition;
                    currentDirection = 'a';
                }
            }
            time = time + requestMovement/travelTimeRatio;
            movement = movement + requestMovement;
            headPosition = request->position;
            request = request->next;
        }
    }
    printf("Movement:%i Time:%.1lf\n", movement, time);
}

/**
 * @brief Service everything going up, then service everything going down.
 * Can wraparound if next request is closer to the start than going backwards to get it
 * 
 * @param start 
 * @return int 
 */
int cscan(int start){
    int movement = 0;
    double time = 0;

    double lastRequestArrivalTime = 0.00;
    int requestMovement = 0;
    int headPosition = start; //makes more sense to me

    double dirChangeTime = 15.00;
    double travelTimeRatio = 5.00;

    Request *request = head;
    if(request != NULL) {
        //loop through the requests
        while(request != NULL) {
            if(time == 0) {
                time = request->arrivalTime;
            }

                Request* closestInSameDir = NULL;
                int closestSameDistance = diskSize+1;
                Request* closestWithWrap = NULL;    //only to be assigned if above is not found
                int closestWrapDistance = diskSize+1;
                Request* closestWithReverse = NULL; //only to be assigned if above is not found
                int closestReverseDistance = diskSize+1;
                int keepLooking = 1;
                Request* knownRequest = head;
                while(knownRequest != NULL && keepLooking) {
                    //as soon as we hit a request that is past the known time, kick out and check what we have
                    if(knownRequest->serviced != 1) {//skip all requests that have been serviced
                        if(knownRequest->arrivalTime > time) {
                            keepLooking = 0;
                        }else{
                            if(currentDirection == 'a') {
                                if(knownRequest->position >= headPosition) { //closest in same direction
                                    if(closestInSameDir == NULL){
                                        //printf("Setting first closest in the same dir\n");
                                        closestInSameDir = knownRequest;
                                        closestSameDistance = knownRequest->position-headPosition;
                                    }else {
                                        int knownRequestClosestSameDist = knownRequest->position-headPosition;
                                        if(knownRequestClosestSameDist < closestSameDistance) {
                                            //printf("Setting new closest in the same dir\n");
                                            closestInSameDir = knownRequest;
                                            closestSameDistance = knownRequestClosestSameDist;
                                        }
                                    }
                                }
                                if(knownRequest->position < headPosition) { 
                                    if(closestWithWrap == NULL){//closest wrap
                                        //printf("Setting first closest wrap\n");
                                        closestWithWrap = knownRequest;
                                        closestWrapDistance = diskSize-headPosition+knownRequest->position;
                                    }else {
                                        int knownRequestWrapDistance = diskSize-headPosition+knownRequest->position;
                                        if(knownRequestWrapDistance < closestWrapDistance) {
                                            //printf("Setting new closest wrap\n");
                                            closestWithWrap = knownRequest;
                                            closestWrapDistance = knownRequestWrapDistance;
                                        }
                                    }
                                    if(closestWithReverse == NULL) {//closest with reverse
                                        //printf("Setting first closest reverse\n");
                                        closestWithReverse = knownRequest;
                                        closestReverseDistance = (diskSize-headPosition)+(diskSize-knownRequest->position);
                                    }else {
                                        int knownReqRevDist = (diskSize-headPosition)+(diskSize-knownRequest->position);
                                        if(knownReqRevDist < closestReverseDistance) {
                                            //printf("Setting new closest reverse\n");
                                            closestWithReverse = knownRequest;
                                            closestReverseDistance = knownReqRevDist;
                                        }
                                    }
                                }
                            }else { // descending 
                                if(knownRequest->position <= headPosition) {
                                    if(closestInSameDir == NULL) {
                                        //printf("d Setting first closest same dir\n");
                                        closestInSameDir = knownRequest;
                                        closestSameDistance = headPosition-knownRequest->position;
                                    }else {
                                        int knownRequestClosestSameDist = headPosition-knownRequest->position;
                                        if(knownRequestClosestSameDist < closestSameDistance) {
                                            //printf("d Setting new closest same dir\n");
                                            closestInSameDir = knownRequest;
                                            closestSameDistance = knownRequestClosestSameDist;
                                        }
                                    }
                                }
                                if(knownRequest->position > headPosition) { 
                                    if(closestWithWrap == NULL){//closest wrap
                                        //printf("d Setting first closest wrap\n");
                                        closestWithWrap = knownRequest;
                                        closestWrapDistance = diskSize+headPosition-knownRequest->position;
                                    }else {
                                        int knownRequestWrapDistance = diskSize+headPosition-knownRequest->position;
                                        if(knownRequestWrapDistance < closestWrapDistance) {
                                            //printf("d Setting new closest wrap\n");
                                            closestWithWrap = knownRequest;
                                            closestWrapDistance = knownRequestWrapDistance;
                                        }
                                    }
                                    //adjust this to account for going all the way to the end and back
                                    if(closestWithReverse == NULL) {//closest with reverse
                                        //printf("d Setting first closest reverse\n");
                                        closestWithReverse = knownRequest;
                                        closestReverseDistance = headPosition+knownRequest->position;
                                    }else {
                                        int knownReqRevDist = headPosition+knownRequest->position;
                                        if(knownReqRevDist < closestReverseDistance) {
                                            //printf("d Setting new closest reverse\n");
                                            closestWithReverse = knownRequest;
                                            closestReverseDistance = knownReqRevDist;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    knownRequest = knownRequest->next;
                }

                if(closestInSameDir != NULL) {
                    //printf("Taking closest in same dir\n");
                    closestInSameDir->serviced = 1;
                    movement = movement + closestSameDistance;
                    time = time + (closestSameDistance/5.00);
                    headPosition = closestInSameDir->position;
                }else{
                    //check if Wrap or Reverse is closest and move to it and service it. 
                    if(closestWrapDistance <= closestReverseDistance) {
                        //printf("Taking closest wrap\n");
                        closestWithWrap->serviced= 1;
                        movement = movement + closestWrapDistance;
                        //time = time + 1; //sector change time
                        if(currentDirection == 'a') {               //10
                            time = time + ((diskSize-headPosition)/5.0) + (closestWithWrap->position/5.00);
                        }else{                          //10                                            //10
                            time = time + (headPosition/5.0) + ((diskSize-closestWithWrap->position)/5.00);
                        }
                        headPosition = closestWithWrap->position;
                    }else { //reverse 
                        //printf("Taking closest reverse\n");
                        closestWithReverse->serviced = 1;
                        movement = movement + closestReverseDistance;
                        time = time + dirChangeTime;
                        
                        if(currentDirection == 'a'){                //10.0
                            time = time + ((diskSize-headPosition)/5.0) + ((diskSize-closestWithReverse->position)/5.00);
                            currentDirection = 'd';
                        }else {                         //10.0
                            time = time + (headPosition/5.0) + (closestWithReverse->position/5.00);
                            currentDirection = 'a';
                        }
                        headPosition = closestWithReverse->position;
                    }
                }
            request = request->next;
        }
    }
    printf("Movement: %i Time:%.1lf\n", movement, time);
}

/**
 * @brief My algo for cscan is SO bad because I implemented it based on the original test cases
 *              and the x/10.0. At least it can be easily changed to accept both scenarios lol
 * 
 *          
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main (int argc, char** argv){
    int position, time;
    char algorithm = argv[1][0];
    int start = 0;
    currentDirection  = 'a';



    while ( EOF!=(scanf("%i %i\n",&position,&time)))
    {
        //printf("Delete me: position %i, Delete me: time %i\n",position,time);
        Request* newRequest = (Request*)malloc(sizeof(Request));
        newRequest->position = position;
        newRequest->arrivalTime = time;
        newRequest->serviced = 0;
        newRequest->next = NULL;
        enqueue(&head, newRequest);
    }
    //printQueue(); queue is working :)

    if (algorithm == 'F'){
        fcfs(start);
    }else if ( algorithm == 'C'){
        cscan(start);
    }


    while(head != NULL) {
        Request* temp = head->next;
        free(head);
        head = temp;
    }

    return 0;
}