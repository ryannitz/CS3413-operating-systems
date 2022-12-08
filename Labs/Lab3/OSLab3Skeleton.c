#include<pthread.h>
#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define RANDOM_WITHIN_RANGE(a,b,seed) (rand_r(&seed)%b+a)
/* This section is to be uncommented and the statements to be moved to the 
appropriate place where they belong
#include <semaphore.h>
pthread_mutex_init(&mutex, NULL);
sem_t sem;
pthread_mutex_t mutex;
int gv = 1;
pthread_mutex_destroy(&mutex);
sem_init(&sem,0,0);
*/






void* minus (){
    int i =0 ;
    for (i = 0 ; i < 10; i++){
        printf("-");
    }
    return NULL;
}

void* plus (void* argg){
    unsigned int seed = *((unsigned int*) argg);
    int interval = RANDOM_WITHIN_RANGE(100000,500000,seed);
    int i =0 ;

    for (i = 0 ; i < 10; i++){
        printf("+");
        usleep(interval);
    }
    return NULL;
}

int main(int argc, char** argv){
    setvbuf(stdout, NULL, _IONBF,0);
    int i = 0;
    

//   for (i = 0 ; i < 1; i++){
//         unsigned int* t = (unsigned int *) malloc(sizeof(unsigned int));
//         *t = rand();
        
//         pthread_create(&(thread[i]), NULL, &plus,(void*)t);
//         pthread_create(&(thread[i+1]), NULL, &minus,NULL);
//     }

    return 0;
}
