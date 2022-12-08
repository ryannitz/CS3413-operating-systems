#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node{
    struct _node* next_elem;
    char* data;
} Node;

// function templates
void add(Node** n, char* c);
void delete(Node* n, char* c);
int contains(Node* n, char* c);
void findAndReplace(Node* n, char* c1, char* c2);
void printList(Node* n);
int stop(Node* n);


/**
*   Main method was leaking 255*11 bytes of memory. I KNOW this is from the strtok and strtok_r. The way it tokenizes values 
       without theor correct terminators etc, makes this way more complex than it should be. I tried using scanf to get through this but to no avail.
       I didn't want to hardcode indices and for-loops to pull out data. So I had to modify each token :(
*/
int main() {

    Node *node = NULL;

    char delim[] = " ";
    
    char line[255];
    int proceed = 1;
    
    printf("To exit: Enter an empty line.\n");
    while(proceed) {

        fgets(line, sizeof line, stdin);

        char *remainder;
        char *initial = malloc(sizeof line);

        strcpy(initial, line);
        char *operation = strtok(initial, delim);

        if(*operation == 's') {
            proceed = stop(node);
        }else if(*operation == 'a') {
            char *var_one = strtok(NULL, delim);
            char *c1_mod = malloc(strlen(var_one) + 1);
            strcpy(c1_mod, var_one);
            strcat(c1_mod, "\0");
            add(&node, c1_mod);
        } else if(*operation == 'd') {
            char *var_one = strtok(NULL, delim);
            char *c1_mod = malloc(strlen(var_one) + 1);
            strcpy(c1_mod, var_one);
            strcat(c1_mod, "\0");
            delete(node, c1_mod);
            free(c1_mod);
        } else if(*operation == 'f') {
            char *var_one = strtok(NULL, delim);
            char *c1_mod = malloc(strlen(var_one) + 2);
            strcpy(c1_mod, var_one);
            strcat(c1_mod, "\n");
            strcat(c1_mod, "\0");

            char *var_two = strtok(NULL, delim);
            char *c2_mod = malloc(strlen(var_two) + 1);
            strcpy(c2_mod, var_two);
            strcat(c2_mod, "\0");

            findAndReplace(node, c1_mod, c2_mod);
            free(c1_mod);
        } else if(*operation == 'p') {
            printList(node);
        }else {
            printf("Invalid operator. Please select one of: {a,d,f,p,s}.\n");
        }

        free(initial); //<- this fixes the memory leak but would start seg faulting with "Invalid read of size 1" for the reasons mentioned above.

    }
    return 0;
}


void add(Node** n, char* c) {
    Node *cur = *n;
    Node *new_node = (Node*)malloc(sizeof(Node));
    new_node->next_elem = NULL;
    new_node->data = c;
    

    if(*n == NULL) {
        *n = new_node;
    }else {
        while(cur->next_elem != NULL) {
            cur = cur->next_elem;
        }
        cur->next_elem = new_node;
    }
}


void delete(Node* n, char* c) {
    Node *previous = NULL;

    while(n != NULL) {
        if(n->data != NULL && c != NULL) {
            if(strcmp(n->data, c) == 0) {
                previous->next_elem = n->next_elem;
                free(n->data);
                free(n);
                return;
            }
        }
        previous = n;
        n = n->next_elem;
    }
}


int contains(Node* n, char* c) {
    while(n != NULL) {
        if(strcmp(n->data, c) == 0) {
            return 1;
        }
    }
    return 0;
}


void findAndReplace(Node* n, char* c1, char* c2) {
    if(c1 != NULL && c2 != NULL) {
        int found = 1;
        while(n != NULL && found == 1) {
            if(n->data != NULL) {
                    // printf("data: %s", n->data);
                    // printf("c1: %s", c1);
                    // printf("c2: %s", c2);
                if(strcmp(n->data, c1) == 0) {
                    free(n->data);
                    n->data = c2;
                    found = 0;
                }
            }
            n = n->next_elem;
        }
        if(found) {
            free(c2);
        }
    }
    
}


void printList(Node* n) {
    if(n != NULL) {
        while(n != NULL) {
            printf(n->data);
            n = n->next_elem;
        }
    }else {
        printf("List is empty\n");
    }
}


int stop(Node* n) {
    Node* temp;
    while(n != NULL) {
        //printf(n->data);
        temp = n->next_elem;
        free(n->data);
        free(n);
        n = temp;
    }
    return 0;
}