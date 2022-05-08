// g++ -g -Wall manad2.cpp -o manager -lpthread
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

#define READ    0
#define WRITE   1 

#define WORKERS "./workers"

#define MAXBUFF 1024
#define FIFO1   "/tmp/fifo-manager-workers.1"
#define FIFO2   "/tmp/fifo-manager-workers.2"

#define SEM_NAME "semaphore1_project"
#define SEM2_NAME "semaphore2_project"
#define SEM3_NAME "semaphore3_project"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1

using namespace std;


void handler(int signal){ 
    // LOGIKA THELEI KI ALLA
    // SIGCHILD: 
    //    na dw poio paidi to esteile 
    //    kai poia h katastash toy
    //    ta shmeiwnw kapoy 
    //    gia na ta xrhsimopoihsw meta 
    exit(0);
}

int main(){

    signal(SIGINT /* | SIGCHLD */, handler);

    pid_t pid, child;
    int fd[2];
    char buff[BUFSIZ];

    int fd1;
    int n;

    int take_from_the_listener = 1;

    char* dir_to_watch = "./new_files";
    
    std::queue <pair<pid_t, char*> > que_workers;

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

    //////////////////////////////////////////////////////////////////


    //// ----------- pipe for listener and manager -----------------

    if(pipe(fd) == -1){ 
        perror ("pipe error"); 
        exit(EXIT_FAILURE);
    }

    /// ----------------- making the listener ------------------

    if( (pid = fork()) < 0 ){
        perror ("fork faild"); 
        exit(EXIT_FAILURE);
    }
    if(pid == 0){         // child and Listener 
        fprintf(stderr,"Listener: %ld\n", (long)getpid());

        close(fd[READ]);
        dup2(fd[WRITE], 1); // the (new)standrad output 
                            // => goes to the input of manager  
        while(take_from_the_listener){
            if( execl("/usr/bin/inotifywait","inotifywait -m -e create -e moved_to", dir_to_watch ,NULL) < 0){
                perror("execlp (listener) failed");
                exit(EXIT_FAILURE);
            }
        }
    }
    if(pid > 0){    // parent and reacever
        close(fd[WRITE]);
        dup2(fd[READ], 0);

        printf("manager- received a filename from the listener!\n");
        
        n = read(fd[READ], buff, BUFSIZ);
        printf("%s", buff);
        take_from_the_listener = 0;
        
    }
    
    /////////////////// MANAGER-WORKERS CONNECTION ///////////////////

    if( (child = fork()) < 0 ){ 
        perror ("fork faild"); 
        exit(EXIT_FAILURE);
    }
    if(child > 0){

        if(mkfifo(FIFO1, 0666) == -1){
            if( errno != EEXIST ){
                perror("manager: mkfifo 1 error");
                exit(6);
            }
        }
        if( (fd1 = open(FIFO1, O_RDONLY)) <0){
            perror("m: fifo1 open error");
            exit(1);
        }

        /* Read the filename from the std 
        *   input of the manager. => give it to the worker
        */    
        
        if(write(fd1, buff, BUFSIZ) < BUFSIZ)
            perror("m: filename write error");

    }
    if(child == 0){
        //workers
        if(execl(WORKERS, WORKERS, NULL) < 0){
                fprintf(stderr,"M: Worker: %ld ", (long)getpid());
                perror("execl failed");
                exit(EXIT_FAILURE);
        }
    }   
    
}

