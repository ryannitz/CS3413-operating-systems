#include <pthread.h> // for pthread_create(), pthread_join(), etc.
#include <stdio.h> // for scanf(), printf(), etc.

// For pipes
#define READ_END 0
#define WRITE_END 1

int maximum_pipe[2];
int minimum_pipe[2];
int average_pipe[2];

int maximum_val = 0, minimum_val = 0, average_val = 0;

/**
    Compiled with: gcc -Wall -Werror -w -g L2.c -std=c99 -lpthread
    Valgrind with: valgrind --track-origins=yes --leak-check=full ./a.out

    Note: This could have been cleaner by simply closing the pipes when main reads in "0",
         but I let the threads also handle the "0" case individually
*/


// some function definitions follow
void* maximum(void* arg);
void* minimum(void* arg);
void* average(void* arg);


int main(int argc, char** argv){

    
    if(pipe(maximum_pipe) < 0 || pipe(minimum_pipe) < 0 || pipe(average_pipe)) {
        printf("Failed to create pipes\n");
        return 0;
    }

    //These threads can go before or after the while loop I beleive. 
    //But before allows each thread to do their calculations the instant a write cycle is done. 
    pthread_t maximum_thread;
    pthread_t minimum_thread;
    pthread_t average_thread;
    pthread_create(&maximum_thread, NULL, &maximum, NULL);
    pthread_create(&minimum_thread, NULL, &minimum, NULL);
    pthread_create(&average_thread, NULL, &average, NULL);

    int proceed = 1;
    while(proceed) {
        int num = 0;
        scanf("%i", &num);
        //printf("num: %i\n", num);
        if(write(maximum_pipe[WRITE_END], &num, sizeof(num)) < 0){
            printf("Failed to write to pipe\n");
        }
        if(write(minimum_pipe[WRITE_END], &num, sizeof(num)) < 0){
            printf("Failed to write to pipe\n");
        }
        if(write(average_pipe[WRITE_END], &num, sizeof(num)) < 0){
            printf("Failed to write to pipe\n");
        }
        if(num == 0) {
            proceed = 0;
        }
    }

    pthread_join(maximum_thread, NULL);
    pthread_join(minimum_thread, NULL);
    pthread_join(average_thread, NULL);

    close(maximum_pipe[READ_END]);
    close(maximum_pipe[WRITE_END]);
    close(minimum_pipe[READ_END]);
    close(minimum_pipe[WRITE_END]);
    close(average_pipe[READ_END]);
    close(average_pipe[WRITE_END]);

    printf("The average value is %i\n", average_val);
    printf("The minimum value is %i\n", minimum_val);
    printf("The maximum value is %i\n", maximum_val);
    return 0;
}

void* maximum(void* arg) {
    int num = 0;
    int proceed = 1;
    while(proceed) {
        int read_length = read(maximum_pipe[READ_END], &num, sizeof(num));
        if(read_length < 0) {
            printf("Failed to read number from pipe\n");
            proceed = 0;
        }else if(read_length == 0) {
            printf("Pipe EOF. Thread is done\n"); //EOF will be our exit in case
            proceed = 0;
        }else{
            if(num == 0) {
                proceed = 0;
            }else if(num > maximum_val || maximum_val == 0) {
                //printf("Setting new maximum: %i\n", num);
                maximum_val = num;
            }
        }
    }
    return NULL;
}

void* minimum(void* arg) {
    int num = 0;
    int proceed = 1;
    while(proceed) {
        int read_length = read(minimum_pipe[READ_END], &num, sizeof(num));
        if(read_length < 0) {
            printf("Failed to read number from pipe\n");
            proceed = 0;
        }else if(read_length == 0) {
            printf("Pipe EOF. Thread is done\n"); //EOF will be our exit condition for mover and trucker
            proceed = 0;
        }else{
            if(num == 0) {
                proceed = 0;
            }else if(num < minimum_val ||  minimum_val == 0) {
                //printf("Setting new minimum: %i\n", num);
                minimum_val = num;
            }
        }
    }
    return NULL;
}

void* average(void* arg) {
    int sum = 0;
    int num = 0;
    int proceed = 1;
    int count = 0;
    while(proceed) {
        int read_length = read(average_pipe[READ_END], &num, sizeof(num));
        if(read_length < 0) {
            printf("Failed to read number from pipe\n");
            proceed = 0;
        }else if(read_length == 0) {
            printf("Pipe EOF. Thread is done\n"); //EOF will be our exit condition for mover and trucker
            proceed = 0;
        }else{
            if(num == 0) {
                proceed = 0;
            }else if(num < minimum) {
                //printf("Updating average\n");
                sum += num;
                count++;
            }
        }
    }
    average_val = (int)(sum/count);
    
    return NULL;
}

