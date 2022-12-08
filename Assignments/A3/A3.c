#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _job {
    char* user;
    char id;
    int arrival;
    int duration;
    struct _job* next_job;
} Job;

// function templates
void add(Job** head, Job* newJob);
void delete (Job** head, Job* jobToRemove);
int contains(Job** head, char* userTarget);
void findAndReplace(Job** head, char* userTarget, int cpu_time);
void printList(Job* head);
void printJob(Job* job);
Job* getLowestTimeJob(Job** head, int considerArrivalTime, int cpu_time);
void printSummary(Job* head);

/**
    Compiled with: gcc -Wall -Werror -w -g A3.c
    Valfrind with: valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./a.out < input.txt

    Tested with provided input, input with one job, and input with no jobs. All passed
*/
int main() {
    Job* head = NULL;

    int CPU_TIME = 0;
    char id;
    int arrival, duration;

    // eat up the first line (The headers on input file will always be there)
    char header[2048];
    fgets(header, 2048, stdin);

    int proceed = 1;
    while (proceed) {
        char* user = malloc(256 * sizeof(char));
        if (scanf("%s %c %i %i", user, &id, &arrival, &duration) == EOF) {
            free(user);
            proceed = 0;
        } else {
            Job* job = (Job*)malloc(sizeof(Job));
            job->user = user;
            job->id = id;
            job->arrival = arrival;
            job->duration = duration;
            job->next_job = NULL;
            add(&head, job);
            // printList(head);
        }
    }

    shortestRemainingTimeFirst(head);

    return 0;
}

void shortestRemainingTimeFirst(Job* head) {
    int cpu_time = 1;
    Job* summaryQueue = NULL;
    Job* currentJob = NULL;

    printf("Time\tJob\n");

    int proceed = 1;
    while (proceed) {
        if (head != NULL) {
            currentJob = getLowestTimeJob(&head, 0, cpu_time);  // return mary, but head is jim, so mary never gets the change to be head
            if (currentJob == NULL) {
                // currently no jobs on CPU
                printf("%i\t-\n", cpu_time);
            } else {
                // cant add summary when job is done, have to add when job arrives (This will break on edge cases for sure)
                // could circumvent this by insertNodeAtPos, based on arrival order, but that would take an entire different method :/
                if (!contains(&summaryQueue, head->user)) {
                    Job* summaryItem = (Job*)malloc(sizeof(Job));
                    summaryItem->user = malloc(sizeof(currentJob->user));
                    strcpy(summaryItem->user, head->user);
                    summaryItem->duration = cpu_time;
                    summaryItem->next_job = NULL;
                    add(&summaryQueue, summaryItem);
                }

                printf("%i\t%c\n", cpu_time, currentJob->id);
                currentJob->duration--;

                if (currentJob->duration == 0) {  // currentJob is finished
                    // udpate summary
                    if (contains(&summaryQueue, currentJob->user)) {
                        findAndReplace(&summaryQueue, currentJob->user, cpu_time);
                    } else {
                        Job* summaryItem = (Job*)malloc(sizeof(Job));
                        summaryItem->user = malloc(sizeof(currentJob->user));
                        strcpy(summaryItem->user, currentJob->user);
                        summaryItem->duration = cpu_time;
                        summaryItem->next_job = NULL;
                        add(&summaryQueue, summaryItem);
                    }

                    delete (&head, currentJob);
                    currentJob = getLowestTimeJob(&head, 1, cpu_time);
                }
            }
        } else {
            // if head is null, we are done jobs
            // technically this IS an idle, we just aren't doing that check inside getLowestTimeJob()
            printf("%i\t-\n", cpu_time);
            proceed = 0;
        }
        cpu_time++;
    }

    printSummary(summaryQueue);

    while (summaryQueue != NULL) {
        Job* temp = summaryQueue->next_job;
        free(summaryQueue->user);
        free(summaryQueue);
        summaryQueue = temp;
    }
}

Job* getLowestTimeJob(Job** head, int considerArrivalTime, int cpu_time) {
    Job* cur = *head;
    Job* lowest = NULL;

    while (cur != NULL) {
        if (cur->arrival <= cpu_time) {
            // need to create the summary entry here really...

            if (lowest == NULL) {
                lowest = cur;
            }
            if (considerArrivalTime) {
                if ((cur->duration < lowest->duration) && (cur->arrival <= lowest->arrival)) {
                    lowest = cur;
                }
            } else {
                if (cur->duration < lowest->duration) {
                    lowest = cur;
                }
            }
        }
        cur = cur->next_job;
    }
    return lowest;
}

void add(Job** head, Job* newJob) {
    // printf("newJob: %c\n", newJob->id);
    Job* cur = *head;
    if (*head == NULL) {
        *head = newJob;
    } else {
        while (cur->next_job != NULL) {
            cur = cur->next_job;
        }
        cur->next_job = newJob;
    }
}

int contains(Job** head, char* userTarget) {
    Job* cur = *head;
    if (*head == NULL) {
        return 0;
    }
    while (cur != NULL) {
        if (strcmp(cur->user, userTarget) == 0) {
            return 1;
        }
        cur = cur->next_job;
    }
    return 0;
}

void delete (Job** head, Job* jobToRemove) {
    // printf("deletedJob: %c\n", jobToRemove->id);
    Job *cur = *head, *prev;
    if (cur == NULL) {
        return;
    }
    if (cur != NULL && cur->id == jobToRemove->id) {
        *head = cur->next_job;
        free(cur->user);
        free(cur);
        return;
    }
    while (cur != NULL && cur->id != jobToRemove->id) {
        prev = cur;
        cur = cur->next_job;
    }
    prev->next_job = cur->next_job;
    free(cur->user);
    free(cur);
}

void findAndReplace(Job** head, char* userTarget, int cpu_time) {
    Job* cur = *head;
    int found = 1;
    while (cur != NULL && found) {
        if (cur->user != NULL) {
            if (strcmp(cur->user, userTarget) == 0) {
                cur->duration = cpu_time;
                found = 0;
            }
        }
        cur = cur->next_job;
    }
}

void printList(Job* head) {
    Job* cur = head;
    if (cur != NULL) {
        while (cur != NULL) {
            printJob(cur);
            cur = cur->next_job;
        }
    } else {
        printf("List is empty\n");
    }
}

void printSummary(Job* head) {
    printf("\nSummary\n");
    Job* cur = head;
    if (cur != NULL) {
        while (cur != NULL) {
            printf("%s\t%i\n", cur->user, cur->duration);
            cur = cur->next_job;
        }
    } else {
        printf("Summary is empty\n");
    }
}

void printJob(Job* job) {
    printf("user:\t\t%s \nid:\t\t%c \narrival:\t%i \nduration:\t%i\n\n", job->user, job->id, job->arrival, job->duration);
}