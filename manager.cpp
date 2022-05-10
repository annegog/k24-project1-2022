#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <errno.h>
#include <error.h>
#include <semaphore.h>
#include <sys/shm.h>

#include <iostream>
#include <queue>
#include <stack>

#define READ    0
#define WRITE   1 

#define WORKERS "./workers"

#define MAXBUFF 1024

#define SEM_NAME "semaphore1"
#define SEM2_NAME "semaphore2"
#define SEM3_NAME "semaphore3"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1

using namespace std;

void sig_handler(int signum){
    signal(SIGCHLD, sig_handler);
}

void child_handler(int signum){
    signal(SIGCONT, child_handler);
}
void handler(int signum){
    signal(SIGINT, handler);
}

char* takeFifo(pair<pid_t, char* > p){
	// Gives second element from queue pair
	char* s = p.second;
	return s;
}

int main(int argc, char **argv){
    
    if(argc != 3){
        fprintf(stderr,"Wrong arguments!\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid, child;
    int fd[2], fd1, fd2;
    char buff[BUFSIZ];
    char buffer[BUFSIZ];

    char* dir_to_watch = argv[2];
    
    queue <pair<pid_t, char*> > queue_workers;

    ///// -------------- semaphores --------------
    // We initialize the semaphore counter to 1 (INITIAL_VALUE)
    sem_t *semaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    // initialize them to 0. 
    // we want to block some processes
    sem_t *semaphore2 = sem_open(SEM2_NAME, O_CREAT | O_EXCL, SEM_PERMS, 0);
    sem_t *semaphore3 = sem_open(SEM3_NAME, O_CREAT | O_EXCL, SEM_PERMS, 0);

    // sem_close(semaphore);
    // sem_unlink(SEM_NAME);
    // sem_close(semaphore2);
    // sem_unlink(SEM2_NAME);
    // sem_close(semaphore3);
    // sem_unlink(SEM3_NAME);

    if(semaphore == SEM_FAILED){
        perror("sem_open(1) error");
        exit(EXIT_FAILURE);
    }
    if(semaphore2 == SEM_FAILED){
        perror("sem2_open(2) error");
        exit(EXIT_FAILURE);
    }
    if(semaphore3 == SEM_FAILED){
        perror("sem3_open(3) error");
        exit(EXIT_FAILURE);
    }


    ///////////////////////// Listener and Manager /////////////////////////////

    if(pipe(fd) == -1){ 
        perror ("pipe error"); 
        exit(EXIT_FAILURE);
    }
    
    if( (pid = fork()) < 0 ){
        perror ("fork faild"); 
        exit(EXIT_FAILURE);
    }
    if(pid == 0){         // child and Listener 
        close(fd[READ]);
        dup2(fd[WRITE], 1); // the (new)standrad output => goes to the input of manager
        if( execl("/usr/bin/inotifywait","usr/bin/inotifywait", "-m", "-e", "create", "-e", "moved_to",dir_to_watch ,NULL) < 0){
            perror("execl-(listener) failed");
            exit(EXIT_FAILURE);
        }
    }
    if(pid > 0){ 
        close(fd[WRITE]);
        dup2(fd[READ], 0);

        while( read(fd[READ], buffer, BUFSIZ) ){
            
            if(signal(SIGINT,handler)){
                kill(child, SIGINT);
            }
            
            printf("parent: %s", buffer);
                
            if(!queue_workers.empty() && signal(SIGCHLD, sig_handler)){
                // an yparxei diathesimow worker 
                // prepei na parw ayton = den ftiaxnw kainoyrgio worker
                
                signal(SIGCONT, child_handler);

                // if((fd1 = open(,O_WRONLY)) < 0){
                //     perror("manager: can't open pipe");
                // }

            }
            else{
                printf("We are making a new worker\n");

                if( (child = fork()) < 0 ){ 
                    perror ("fork faild"); 
                    exit(EXIT_FAILURE);
                }
                
                printf("sem down\n");
                sem_wait(semaphore);

                printf("---------------------------I'm the parent-manager--------------------\n");
                
                /****** Create a namedpipe for the worker-manager connection ********/
                
                printf("start creating a pipe...\n");
                
                // naming the pipe
                char pipename[MAXBUFF];
                sprintf(pipename, "worker_%d.pipe", getpid());

                if((mkfifo(pipename, 0666) < 0) && (errno != EEXIST) ) {
                    printf("can't make fifo %s \n", pipename);
                }
                printf("pushing worker-%d and the pipe:%s in the queue\n",getpid(), pipename);
                queue_workers.push({getpid(), pipename});

                /*********************************************************/
                
                printf("hellooo // this is the pipe we just made: %s \n", pipename);
                
                if(child == 0){
                    printf("----------------------- Manager's worker -------------------\n");
                    
                    // printf("worker-%d opening the fifo\n", getpid());
                    // if( (fd2 = open(pipename, O_RDONLY))  < 0){
                    //     perror("worker: can't open fifo");
                    // }

                    printf("execl the worker %d -- pipe:%s\n", getpid(), pipename);
                    if(execl(WORKERS, WORKERS, pipename , NULL) < 0){
                        perror("execl failed");
                        exit(EXIT_FAILURE);
                    }
                }
                
                // manager is opening the pipe so 
                // so he can send the filename to the worker.

                // printf("manager is opening the pipe\n");
                // if((fd1 = open(pipename,O_WRONLY)) < 0){
                //     perror("manager: can't open pipe");
                // }


            }
                           
        }            

    }



}
