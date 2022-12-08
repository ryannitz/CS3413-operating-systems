#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node {
    int arrivalTime;
    int duration;
    int last_job;
    char processName;
    char* userName;
    struct _node* next;
} Job;

typedef struct cpu {
    Job* current;
    int time;
} CPU;

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

void simulate(CPU* cpu, Job** queue, Job* summary) {
    printf("Time Job\n");
    while (*queue != NULL) {
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
            printf("%d\t%c\n", cpu->time, cpu->current->processName);
            cpu->current->duration--;
            if (cpu->current->duration == 0) {
                updateSummary(summary, cpu->current->userName, cpu->time);
                removeJob(queue, cpu->current);
                cpu->current = NULL;
            }
        } else {
            printf("%d\t-\n", cpu->time);
        }
        cpu->time++;
    }
    printf("%d\t-\n", cpu->time);
}
void printSummary(Job* queue) {
    printf("\nSummary\n");
    while (queue != NULL) {
        printf("%s\t%d\n", queue->userName, queue->last_job);
        queue = queue->next;
    }
}
int main(int argc, char** argv) {
    Job* summary = NULL;
    Job* queue = NULL;
    char buf[100];
    char buf2[100];
    char buf3[100];
    char buf4[100];
    CPU cpu;
    cpu.time = 1;
    char userName[100];

    char processName;
    int arrival;
    int duration;
    Job* newJob;
    scanf("%s %s %s %s \n", buf, buf2, buf3, buf4);
    while (EOF != scanf("%s %c %d %d", userName, &processName, &arrival, &duration)) {
        if (contains(summary, userName) == 0) {
            newJob = (Job*)malloc(sizeof(Job));
            newJob->userName = strdup(userName);
            newJob->processName = processName;
            newJob->arrivalTime = arrival;
            newJob->duration = duration;
            newJob->next = NULL;
            enqueue(&summary, newJob);
        }
        newJob = (Job*)malloc(sizeof(Job));
        newJob->userName = strdup(userName);
        newJob->processName = processName;
        newJob->arrivalTime = arrival;
        newJob->duration = duration;
        newJob->next = NULL;
        enqueue(&queue, newJob);
    }
    simulate(&cpu, &queue, summary);
    printSummary(summary);
    return 0;
}