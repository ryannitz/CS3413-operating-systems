#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
    Compiled with: gcc -Wall -Werror -w -g A4.c -std=gnu99 -lpthread
    Valgrind with: valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./a.out < inputx.txt
*/

typedef struct _node {
    int arrivalTime;
    int duration;
    int last_job;
    char processName;
    char* userName;
    int affinity;
    struct _node* next;
} Job;

typedef struct cpu {
    int id;
    Job* current;
    int time;
} CPU;

typedef struct cpu_args {
    CPU* cpu;
    Job* queue;
    Job* summary;
} CpuArgs;

int cpu_count = 0;
int remaining_queues = 0;
char* cpu_reports = NULL;
int* flag = NULL;
pthread_mutex_t mutex;
sem_t* reports;
sem_t* execute;

void enqueue(Job** queue, Job* job) {
    if (*queue == NULL) {
        *queue = job;
        return;
    }
    Job* temp = *queue;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = job;
}

void removeJob(Job** queue, Job* job) {
    if (*queue == NULL) {
        return;
    }
    if (*queue == job) {
        Job* toFree = *queue;
        *queue = (*queue)->next;
        free(toFree->userName);
        free(toFree);
        return;
    }
    Job* temp = *queue;
    while (temp->next != job) {
        temp = temp->next;
    }
    Job* toFree = temp->next;
    temp->next = job->next;
    free(toFree->userName);
    free(toFree);
}

int contains(Job* queue, char* userName) {
    int result = 0;
    while (queue != NULL) {
        if (strcmp(queue->userName, userName) == 0) {
            result = 1;
        }
        queue = queue->next;
    }
    return result;
}

void updateSummary(Job* queue, char* userName, int time) {
    while (queue != NULL) {
        if (strcmp(queue->userName, userName) == 0) {
            queue->last_job = time;
        }
        queue = queue->next;
    }
}

void* simulate(void* arg) {
    CpuArgs* args = (CpuArgs*)arg;
    CPU* cpu = args->cpu;
    Job** queue = args->queue;
    Job* summary = args->summary;

    while (*queue != NULL) {
        sem_wait(&execute[cpu->id]);
        Job* queueSnapshot = *queue;
        while (queueSnapshot != NULL) {
            if (cpu->current != NULL) {
                if (cpu->current->duration > queueSnapshot->duration && cpu->time >= queueSnapshot->arrivalTime) {
                    cpu->current = queueSnapshot;
                }
            } else if (cpu->current == NULL) {
                if (cpu->time >= queueSnapshot->arrivalTime) {
                    cpu->current = queueSnapshot;
                }
            }
            queueSnapshot = queueSnapshot->next;
        }
        if (cpu->current != NULL) {
            cpu_reports[cpu->id] = cpu->current->processName;
            cpu->current->duration--;
            if (cpu->current->duration == 0) {
                pthread_mutex_lock(&mutex);
                updateSummary(summary, cpu->current->userName, cpu->time);
                pthread_mutex_unlock(&mutex);
                removeJob(queue, cpu->current);
                cpu->current = NULL;
            }
        } else {
            cpu_reports[cpu->id] = '-';
        }
        sem_post(&reports[cpu->id]);
        cpu->time++;
    }

    sem_wait(&execute[cpu->id]);
    cpu_reports[cpu->id] = '-';
    sem_post(&reports[cpu->id]);
    remaining_queues--;
    flag[cpu->id] = 0;
    free(cpu->current);
    free(cpu);
    free(args);
}

void* printCpuCycle(void* arg) {
    int cpu_time = 1;
    while (remaining_queues > 0) {
        for (int i = 0; i < cpu_count; i++) {
            if (flag[i]) {
                sem_wait(&reports[i]);
            }
        }

        printf("%i", cpu_time);
        for (int i = 0; i < cpu_count; i++) {
            printf("\t%c", cpu_reports[i]);
        }
        printf("\n");
        // need to flush stdio or sometimes the very last cpu cycle will print its random symbols throughout the summary
        fflush(stdout);
        fflush(stdin);

        for (int i = 0; i < cpu_count; i++) {
            if (flag[i]) {
                sem_post(&execute[i]);
            }
        }
        cpu_time++;
    }
}

void printList(Job* queue) {
    Job* cur = queue;
    if (cur != NULL) {
        while (cur != NULL) {
            printJob(cur);
            cur = cur->next;
        }
    } else {
        printf("List is empty\n");
    }
}

void printJob(Job* job) {
    printf("user:\t\t%s\nid:\t\t%c\narrival:\t%i \nduration:\t%i\naffinity:\t%i\n\n",
           job->userName, job->processName, job->arrivalTime, job->duration, job->affinity);
}

void printSummary(Job* queue) {
    printf("\nSummary\n");
    while (queue != NULL) {
        printf("%s\t%d\n", queue->userName, queue->last_job);
        queue = queue->next;
    }
}

int main(int argc, char** argv) {
    scanf("%d", &cpu_count);
    remaining_queues = cpu_count;
    cpu_reports = malloc(sizeof(char) * cpu_count);
    flag = malloc(sizeof(int) * cpu_count);
    execute = (sem_t*)malloc(cpu_count * sizeof(sem_t));
    reports = (sem_t*)malloc(cpu_count * sizeof(sem_t));

    Job* queue[cpu_count];
    printf("Time\t");
    for (int i = 0; i < cpu_count; i++) {
        printf("CPU%i\t", i);
        queue[i] = NULL;
        flag[i] = 1;
    }
    printf("\n");

    Job* summary = NULL;
    char buf[100];
    char buf2[100];
    char buf3[100];
    char buf4[100];
    char buf5[100];

    char userName[100];
    char processName;
    int arrival;
    int duration;
    int affinity;
    Job* newJob;

    scanf("%s %s %s %s %s\n", buf, buf2, buf3, buf4, buf5);
    while (EOF != scanf("%s %c %d %d %d", userName, &processName, &arrival, &duration, &affinity)) {
        if (contains(summary, userName) == 0) {
            newJob = (Job*)malloc(sizeof(Job));
            newJob->userName = strdup(userName);
            newJob->processName = processName;
            newJob->arrivalTime = arrival;
            newJob->duration = duration;
            newJob->affinity = affinity;
            newJob->next = NULL;
            enqueue(&summary, newJob);
        }
        newJob = (Job*)malloc(sizeof(Job));
        newJob->userName = strdup(userName);
        newJob->processName = processName;
        newJob->arrivalTime = arrival;
        newJob->duration = duration;
        newJob->affinity = affinity;
        newJob->next = NULL;
        enqueue(&(queue[affinity]), newJob);
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_t printThread;
    pthread_create(&printThread, NULL, &printCpuCycle, NULL);  // this thread is creating a weird "possibly" lost mem leak

    pthread_t threads[cpu_count];
    for (int i = 0; i < cpu_count; i++) {
        sem_init(&execute[i], 0, 1);
        sem_init(&reports[i], 0, 0);
        CPU* cpu = (CPU*)malloc(sizeof(CPU));
        cpu->time = 1;
        cpu->id = i;
        cpu->current = NULL;
        CpuArgs* cpu_args = (CpuArgs*)malloc(sizeof(CpuArgs));
        cpu_args->cpu = cpu;
        cpu_args->queue = &(queue[i]);
        cpu_args->summary = summary;
        pthread_create(&(threads[i]), NULL, &simulate, (void*)cpu_args);
    }
    for (int i = 0; i < cpu_count; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(&printThread, NULL);  // this may need to come before the join loop above, we shall see lol
    pthread_mutex_destroy(&mutex);

    printSummary(summary);

    free(cpu_reports);
    free(flag);
    free(execute);
    free(reports);
    while (summary != NULL) {
        Job* temp = summary->next;
        free(summary->userName);
        free(summary);
        summary = temp;
    }
    return 0;
}