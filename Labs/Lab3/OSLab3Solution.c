#include<pthread.h>
#include<stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#define RANDOM_WITHIN_RANGE(a,b,seed) (rand_r(&seed)%b+a)
sem_t sem;
pthread_mutex_t mutex;
int gv = 1;
void* inc_hundred (){
    /* Solution to Q2 */
    int i =0 ;
    for (i = 0 ; i < 100; i++){
        gv+=1;
    }
    /* Answer to Q3:
    likely, you have seen it always printing one.
    It is puzzling as it contradicts Lectures about critical section problem
    What causes the 1 to be printed seemingly properly, is that pthread_create takes longer time than running the loop
    for a hundred times */
    return NULL;
}

void* dec_hundred(){
    /* Solution to Q2 */
    int i =0 ;
    for (i = 0 ; i < 100; i++){
        gv-=1;
    }
    /* Answer to Q3:
    likely, you have seen it always printing one.
    It is puzzling as it contradicts Lectures about critical section problem
    What causes the 1 to be printed seemingly properly, is that pthread_create takes longer time than running the loop
    for a hundred times */
    return NULL;
}

void* inc_thousand (){
    /* Solution to Q4 */
    int i =0 ;
    for (i = 0 ; i < 1000; i++){
        gv+=1;
    }
    /* Answer to Q5:
    likely, you have seen it always printing one. Or if you are really lucky, a weird number would appear once or twice.
    It is puzzling as it contradicts Lectures about critical section problem
    What causes the 1 to be printed seemingly properly, is that pthread_create takes longer time than running the loop
    even for a thousand times */
    return NULL;
}

void* dec_thousand(){
    /* Solution to Q4 */
    int i =0 ;
    for (i = 0 ; i < 1000; i++){
        gv-=1;
    }
    /* Answer to Q5:
    likely, you have seen it always printing one. Or if you are really lucky, a weird number would appear once or twice.
    It is puzzling as it contradicts Lectures about critical section problem
    What causes the 1 to be printed seemingly properly, is that pthread_create takes longer time than running the loop
    even for a thousand times */
    return NULL;
}
void* inc_ten_thousand (){
    /* Solution to Q6 */
    int i =0 ;
    for (i = 0 ; i < 10000; i++){
        gv+=1;
    }
    /* Answer to Q7:
    likely, you have never seen it printing one. Or if you are really lucky, one would appear once or twice.
    It is puzzling as it contradicts Lectures about critical section problem
    It is at 10000 iterations we are more or less guaranteed to experience data corruption
    */
    return NULL;
}

void* dec_ten_thousand(){
    /* Solution to Q6 */
    int i =0 ;
    for (i = 0 ; i < 10000; i++){
        gv-=1;
    }
    /* Answer to Q7:
    likely, you have never seen it printing one. Or if you are really lucky, one would appear once or twice.
    It is puzzling as it contradicts Lectures about critical section problem
    It is at 10000 iterations we are more or less guaranteed to experience data corruption
    */

    return NULL;
}
void* inc_synched (){
    /* Solution to Q8 */
    int i =0 ;
    // pthread_mutex_lock(&mutex); Placing this here in combination with the commented out pthread_mutex_unlock would ensure one is printed BUT it also makes your two threads 
    // pretty much sequential, as the thread who gets to call pthread_mutex lock the second has to wait for 10k iterations
    for (i = 0 ; i < 10000; i++){
        pthread_mutex_lock(&mutex);
        gv+=1;
        pthread_mutex_unlock(&mutex);
    }
    // pthread_mutex_unlock(&mutex);Placing this here in combination with the commented out pthread_mutex_lock would ensure one is printed BUT it also makes your two threads 
    // pretty much sequential, as the thread who gets to call pthread_mutex lock the second has to wait for 10k iterations
    /* Answer to Q9
    * Yes, one is  always displayed as the mutexes protect the access to the shared variable
    * and only one is allowed at a time */
    return NULL;
}

void* dec_synched (){
    /* Solution to Q8 */
    int i =0 ;
    // pthread_mutex_lock(&mutex); Placing this here in combination with the commented out pthread_mutex_unlock would ensure one is printed BUT it also makes your two threads 
    // pretty much sequential, as the thread who gets to call pthread_mutex lock the second has to wait for 10k iterations
    for (i = 0 ; i < 10000; i++){
        pthread_mutex_lock(&mutex);
        gv-=1;
        pthread_mutex_unlock(&mutex);
    }
    // pthread_mutex_unlock(&mutex);Placing this here in combination with the commented out pthread_mutex_lock would ensure one is printed BUT it also makes your two threads 
    // pretty much sequential, as the thread who gets to call pthread_mutex lock the second has to wait for 10k iterations
    /* Answer to Q9
    * Yes, one is  always displayed as the mutexes protect the access to the shared variable
    * and only one is allowed at a time */
    return NULL;
}

void* minus (){
    int i =0 ;
    for (i = 0 ; i < 10; i++){
        // Answer to Q10 is here. See why on the slide deck 6 slide 28
        sem_wait(&sem);
        printf("-");
    }
    return NULL;
}

void* plus (void* argg){
    unsigned int seed = *((unsigned int*) argg);
    int interval = RANDOM_WITHIN_RANGE(100000,500000,seed);
    int i =0 ;
    /* Answer to Q14 - posting just three times will make the minus thread wait indefinitely. 
    Very much  expected behavior*/
    for (i = 0 ; i < 10; i++){
        printf("+");
        // Answer to Q10 is here. See why on the slide deck 6 slide 28.
        sem_post(&sem);
        // Answer to Q12 : proper way would be to use condition variables, but as we 
        // did not cover them yet, the skeleton uses usleep to make sure
        // that minus thread has enough time to wake up and print - before the next plus is
        // printed, as well as prevents this thread from posting too often (which would allow multiple pluses).
        usleep(interval);
    }
    return NULL;
}

int main(int argc, char** argv){
    pthread_t thread[2];
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF,0);
    int i = 0;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem,0,0);
    for (i = 0 ; i < 1; i++){
        pthread_create(&(thread[i]), NULL, &inc_hundred,NULL);
        pthread_create(&(thread[i+1]), NULL, &dec_hundred,NULL);
    }
    
    for (i = 0 ; i < 2; i++){
        pthread_join(thread[i],NULL);
    }
    printf("In main, gv is %d\n",gv);
    // Solution to Q2 ends
    gv = 1;

    for (i = 0 ; i < 1; i++){
        pthread_create(&(thread[i]), NULL, &inc_thousand,NULL);
        pthread_create(&(thread[i+1]), NULL, &dec_thousand,NULL);
    }
    
    for (i = 0 ; i < 2; i++){
        pthread_join(thread[i],NULL);
    }
    printf("In main, gv is %d\n",gv);
    // Solution to Q4 ends
    gv = 1 ; 
    for (i = 0 ; i < 1; i++){
        pthread_create(&(thread[i]), NULL, &inc_ten_thousand,NULL);
        pthread_create(&(thread[i+1]), NULL, &dec_ten_thousand,NULL);
    }
    
    for (i = 0 ; i < 2; i++){
        pthread_join(thread[i],NULL);
    }

    printf("In main, gv is %d\n",gv);
    // Solution to Q6 ends
    gv = 1 ; 
    for (i = 0 ; i < 1; i++){
        pthread_create(&(thread[i]), NULL, &inc_synched,NULL);
        pthread_create(&(thread[i+1]), NULL, &dec_synched,NULL);
    }
    
    for (i = 0 ; i < 2; i++){
        pthread_join(thread[i],NULL);
    }

    printf("In main, gv is %d\n",gv);
    // Solution to Q6 ends

    for (i = 0 ; i < 1; i++){
        unsigned int* t = (unsigned int *) malloc(sizeof(unsigned int));
        *t = rand();
        
        pthread_create(&(thread[i]), NULL, &plus,(void*)t);
        pthread_create(&(thread[i+1]), NULL, &minus,NULL);
    }
    
    for (i = 0 ; i < 2; i++){
        pthread_join(thread[i],NULL);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}