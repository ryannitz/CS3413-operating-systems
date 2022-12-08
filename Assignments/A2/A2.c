#include <pthread.h> // for pthread_create(), pthread_join(), etc.
#include <stdio.h> // for scanf(), printf(), etc.
#include <stdlib.h> // for malloc()
#include <unistd.h> // for sleep()

#define NUMBER_OF_BOXES_PER_DWELLER 5
#define ROOM_IN_TRUCK 10
#define MIN_BOX_WEIGHT 5
#define MAX_BOX_WEIGHT 50
#define MAX_TIME_FOR_HOUSE_DWELLER 7
#define MIN_TIME_FOR_HOUSE_DWELLER 1
#define MAX_TIME_FOR_MOVER 3
#define MIN_TIME_FOR_MOVER 1
#define MIN_TIME_FOR_TRUCKER 1
#define MAX_TIME_FOR_TRUCKER 3
#define MIN_TRIP_TIME 5
#define MAX_TRIP_TIME 10
#define RANDOM_WITHIN_RANGE(a,b,seed) (a+rand_r(&seed)%(b-a))
// For pipes
#define READ_END 0
#define WRITE_END 1

// Pipe between house dwellers and movers
int houseFloor[2];
// Pipe between movers and truckers
int nextToTrucks[2];



/**
    Compiled with: gcc -Wall -Werror -w -g A2.c -std=c99 -lpthread
    Valgrind with: valgrind --track-origins=yes --leak-check=full ./a.out

    No failures/leaks with input: 5 2 2
    Questions to ask prof:
        What are the input restrictions here.. Right now I have that we must have at least 1 of each
            Are there any other restrictions we should know about?
*/


// some function definitions follow
void* dweller(void* arg);
void* mover(void* arg);
void* driver(void* arg);

typedef struct thread_args {
    int id;
    int seed;
}ThreadArgs;


int main(int argc, char** argv){
    srand(time(NULL));

    int dweller_count = 0, mover_count = 0, driver_count = 0;
    printf("Please input numer of people living in the house, number of movers and number of truck drivers\n");
    scanf("%i %i %i", &dweller_count, &mover_count, &driver_count);

    if(dweller_count < 1 || mover_count < 1 || driver_count < 1) {
        printf("Must have at least 1 of each dweller, mover, and driver");
        return 0;
    }

    if(pipe(houseFloor) < 0 || pipe(nextToTrucks) < 0) {
        printf("Failed to create pipes\n");
        return 0;
    }

    pthread_t dweller_thread[dweller_count];
    for(int i = 0; i < dweller_count; i++) {
        ThreadArgs* args = (ThreadArgs*) (malloc(sizeof(ThreadArgs)));
        args->id = i;
        args->seed = rand();
        pthread_create(&dweller_thread[i], NULL, &dweller, (void*)args);
    } 
    pthread_t mover_thread[mover_count];
    for(int i = 0; i < mover_count; i++) {
        ThreadArgs* args = (ThreadArgs*) (malloc(sizeof(ThreadArgs)));
        args->id = i;
        args->seed = rand();
        pthread_create(&mover_thread[i], NULL, &mover, (void*)args);
    }
    pthread_t driver_thread[driver_count];
    for(int i = 0; i < driver_count; i++) {
        ThreadArgs* args = (ThreadArgs*) (malloc(sizeof(ThreadArgs)));
        args->id = i;
        pthread_create(&driver_thread[i], NULL, &driver, (void*)args);
    }

    // Must close the correct pipes in certain order so that the final move can be made in each thread.
    // If not, the thread will hang waiting for an input to the pipe.

    for(int i = 0; i < dweller_count; i++) {
        pthread_join(dweller_thread[i], NULL);
    }  
    close(houseFloor[WRITE_END]);
    for(int i = 0; i < mover_count; i++) {
        pthread_join(mover_thread[i], NULL);
    }
    close(houseFloor[READ_END]);
    close(nextToTrucks[WRITE_END]);
    for(int i = 0; i < driver_count; i++) {
        pthread_join(driver_thread[i], NULL);
    }
    close(nextToTrucks[READ_END]);

    printf("Moving is finished!\n");    
    return 0;
}

void* dweller(void* arg) {
    ThreadArgs* args = (ThreadArgs*) arg;
    int i = args->id;
    int seed = args->seed;
    int remainingBoxes = NUMBER_OF_BOXES_PER_DWELLER;

    printf("Hello from dweller %i\n", i);
    while(remainingBoxes > 0) {
        int interval = RANDOM_WITHIN_RANGE(MIN_TIME_FOR_HOUSE_DWELLER, MAX_TIME_FOR_HOUSE_DWELLER, seed);
        int weight = RANDOM_WITHIN_RANGE(MIN_BOX_WEIGHT, MAX_BOX_WEIGHT, seed);
        sleep(interval);
        printf("House dweller %i created a box that weighs %i in %i units of time\n", i, weight, interval);
        if(write(houseFloor[WRITE_END], &weight, sizeof(weight)) < 0){
            printf("Failed to write to pipe\n");
        }
        remainingBoxes--;
    }
    printf("House dweller %i is done packing\n", i);

    free(arg);
    return NULL;
}

void* mover(void* arg) {
    ThreadArgs* args = (ThreadArgs*) arg;
    int i = args->id;
    int seed = args->seed;
    int proceed = 1;

    printf("Hello from mover %i\n", i);
    while(proceed) {
        int box_weight = 0;
        int read_length = read(houseFloor[READ_END], &box_weight, sizeof(box_weight));
        int interval = RANDOM_WITHIN_RANGE(MIN_TIME_FOR_MOVER, MAX_TIME_FOR_MOVER, seed);
        sleep(interval);
        if(read_length < 0) {
            printf("Failed to read number from pipe\n");
            proceed = 0;
        }else if(read_length == 0) {
            //printf("Pipe EOF\n"); //EOF will be our exit condition for mover and trucker
            proceed = 0;
        }else{
            printf("Mover %i brought down a box that weighs %i in %i units of time\n", i, box_weight, interval);
            if(write(nextToTrucks[WRITE_END], &box_weight, sizeof(box_weight)) < 0){
                printf("Failed to write to pipe\n");
            }
        }
    }
    printf("Move %i is done moving boxes downstairs\n", i); 

    free(arg);
    return NULL;
}

void* driver(void* arg) {
    ThreadArgs* args = (ThreadArgs*) arg;
    int i = args->id;
    int seed = args->seed;
    int proceed = 1;

    printf("Hello from driver %i\n", i);
    while(proceed) {
        int total_weight = 0;
        int loading = 1;
        int remaining_room = ROOM_IN_TRUCK;
        while(loading && remaining_room > 0) {
            int box_weight = 0;
            int interval = RANDOM_WITHIN_RANGE(MAX_TIME_FOR_TRUCKER, MIN_TIME_FOR_TRUCKER, seed);
            sleep(interval);
            //printf("Waiting for nextToTrucks read\n");
            int read_length = read(nextToTrucks[READ_END], &box_weight, sizeof(box_weight));
            if(read_length < 0) {
                printf("Failed to read number from pipe\n");
                loading = 0;
            }else if(read_length == 0) {
                //printf("Pipe EOF\n");
                loading = 0;
            }else{
                printf("Trucker %i loaded up a box that weighs %i to the truck, took %i units of time, room left:%i\n", i, box_weight, interval, remaining_room);
                total_weight += box_weight;
            }
            remaining_room--;
        }
        int trip_time = RANDOM_WITHIN_RANGE(MAX_TRIP_TIME, MIN_TRIP_TIME, seed);
        if(remaining_room == 0) {
            printf("Full truck %i with load of %i of mass departed, round-trip will take %i\n", i, total_weight, trip_time);
        }else {
            printf("Not full truck with load of %i of mass departed, round-trip will take %i\n", total_weight, trip_time);
            proceed = 0;
        }
    }
    printf("Trucker %i is finished\n", i);
    
    free(arg);
    return NULL;
}

