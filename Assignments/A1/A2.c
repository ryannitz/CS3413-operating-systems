#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define READ 0
#define WRITE 1


void oneChild();
void addNumbers();
void exec();
void processTree(int generation, int i);

/**
    Compiled with: "gcc -Wall -Werror -w -g A2.c"
    Memcheck with: "valgrind --track-origins=yes --leak-check=full ./a.out"
*/
int main() {
    char cmd_stop[] = "stop";
    char cmd_onechild[] = "onechild";
    char cmd_addnumbers[] = "addnumbers";
    char cmd_exec[] = "exec";
    char cmd_processtree[] = "processtree";

    char line[255];

    int inputloop = 1;
    while(inputloop) {
        printf("Please input your shell command:\n");
        scanf("%s", line);

        if(strcmp(cmd_stop, line) == 0) {
            inputloop = 0;
        }else if(strcmp(cmd_onechild, line) == 0) {
            oneChild();
        }else if(strcmp(cmd_addnumbers, line) == 0) {
            addNumbers();
        }else if(strcmp(cmd_exec, line) == 0) {
            exec();
        }else if(strcmp(cmd_processtree, line) == 0) {
            printf("Please input generation number\n");
            int generation;
            scanf("%d", &generation);
            printf("I am %i, and I am the parent\n", getpid());
            processTree(generation, 0);
        }else {
            //printf("Invalid.\n");
        }
    }

    return 0;
}

void oneChild() {
    pid_t fork_pid = fork();
    if(fork_pid < 0) {
        printf("Failed to fork process\n");
    }else if(fork_pid == 0) { //child
        printf("Hello, I am a parent process %i\n", getppid());
        printf("Hello, I am a child process %i\n", getpid());
        kill(getpid(), SIGTERM);
    }else if(fork_pid > 0) {
        wait(NULL);//need to wait so that the input prompt displays last
    }
}

void addNumbers() {
    int fd1[2];
    int fd2[2];
    pid_t pid;
    if (pipe(fd1) < 0 || pipe(fd2) < 0) {
        printf("Failed to create pipes\n");
    }else {
        pid = fork();
        if (pid < 0) {
            printf("Failed to fork process\n");
        }else if (pid == 0) { // child
            //printf("child pid: %d\n", getpid());
            close(fd1[READ]);
            close(fd2[WRITE]);
            int num = 0;
            int read_length;
            int sum = 0;
            int proceed = 1;
            while(proceed != 0) {
                read_length = read(fd2[READ], &num, sizeof(num));
                if(read_length < 0) {
                    printf("Failed to read number from pipe\n");
                }else if(read_length == 0) {
                    printf("Pipe EOF error\n");
                }else {
                    sum += num;
                    //printf("Sum: %d\n", sum);
                }
                proceed = num;
            }
            //printf("Returning sum: %d\n", sum);
            if(write(fd1[WRITE], &sum, sizeof(sum)) < 0) {
                printf("Failed to write to pipe\n");
            }
            close(fd2[READ]);
            close(fd1[WRITE]);
            kill(getpid(), SIGTERM);
        }else if(pid > 0) { //parent
            //printf("parent pid: %d\n", getpid());
            close(fd2[READ]);
            close(fd1[WRITE]);
            int num = 0;
            int proceed = 1;
            while(proceed) {
                printf("Please input numbers, (0) to terminate\n");
                scanf("%d", &num);
                //printf("Num: %d\n", num);
                if(write(fd2[WRITE], &num, sizeof(num)) < 0){
                    printf("Failed to write to pipe\n");
                }
                proceed = num;
            }
            int read_length = read(fd1[READ], &num, sizeof(num));
            if(read_length < 0) {
                printf("Failed to read sum from pipe\n");
            }else if(read_length == 0) {
                printf("EOL broken 1\n");
            }else {
                printf("Sum is %d\n", num);
            }
            close(fd1[READ]);
            close(fd2[WRITE]);
            wait(NULL);
        }else {
            //close the opened pipes
            close(fd1[READ]);
            close(fd1[WRITE]);
            close(fd2[READ]);
            close(fd2[WRITE]);
        }
    }
}


void exec() {
    printf("Please input exec subcommand\n");
    int max_args = 5;
    int arg_length = 64;
    char** args = malloc((max_args+1) * arg_length * sizeof(char));//[max_args][arg_length];
    char arg[arg_length];

    int proceed = 1;
    int arg_count = 0;
    while(proceed && arg_count < max_args) {
        scanf("%s", arg);
        if(arg[0] != ';') {
            args[arg_count] = malloc(strlen(arg)*sizeof(char));
            strcpy(args[arg_count], arg);
        }else{
            proceed = 0;
        }
        arg_count++;
    }
    args[max_args] = NULL;
    proceed = 0;

    pid_t pid = fork();
    if (pid < 0) {
        printf("Failed to fork process");
    }else if (pid == 0) { // child
        //printf("child pid: %d\n", getpid());
        char* program = args[0];
        execvp(program, args);
        kill(getpid(), SIGTERM);
    }else if(pid > 0) { //parent
        //printf("parent pid: %d\n", getpid());
        wait(NULL); 
    }

    while(proceed < max_args+1) {
        free(args[proceed]);
        proceed++;
    }
    free(args);
}

/**
    For some reason my i is inverted and I can't fix it without some extraneous measures
    There is probably and easy way to fix it but I couldn't figure it out.
*/
void processTree(int generation, int i) {
    if(generation == 0) {
        return 0;
    }
    pid_t pid = fork();
    if(pid < 0) {
        printf("Failed to fork process\n");
    }else if(pid == 0) { //child
        int count = 0;
        while(count < i+1) {
            printf("        ");
            count++;
        }
        printf("I am %i the child of %i, i is %i, generation is %i\n", getpid(), getppid(), generation, i+1);
        processTree(generation-1, i+1);
        kill(getpid(), SIGTERM);
    }else if(pid > 0) { //parent 
        wait(NULL);
        processTree(generation-1, i);
    }
}