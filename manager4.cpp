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

#define SEM_NAME "semaphore1"
#define SEM2_NAME "semaphore2"
#define SEM3_NAME "semaphore3"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1

using namespace std;

bool not_available(pair<pid_t, int> q);

void sig_handler(int signum){
    signal(SIGCHLD, sig_handler);
}


void printPair(pair<int, int> p)
{
	// Gives first element from queue pair
	int f = p.first;

	// Gives second element from queue pair
	int s = p.second;

	cout << "(" << f << ", " << s << ") ";
}

// Print the Queue of Pairs
void showQueue(queue<pair<int, int> > gq)
{
	// Print element until the
	// queue is not empty
	while (!gq.empty()) {
		printPair(gq.front());
		gq.pop();
	}

	cout << '\n';
}

int main(){
        
    pid_t pid, child;
    int fd[2], fd1;
    char buff[BUFSIZ];
    char buffer[BUFSIZ];
    int n;

    char* dir_to_watch = "./new_files";
    
    queue <pair<pid_t, int> > queue_workers;

    ///// -------------- semaphores --------------
    // We initialize the semaphore counter to 1 (INITIAL_VALUE)
    // sem_t *semaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    // // initialize them to 0. 
    // // we want to block some processes
    // sem_t *semaphore2 = sem_open(SEM2_NAME, O_CREAT | O_EXCL, SEM_PERMS, 0);
    //sem_t *semaphore3 = sem_open(SEM3_NAME, O_CREAT | O_EXCL, SEM_PERMS, 0);

    // sem_close(semaphore);
    // sem_unlink(SEM_NAME);
    // sem_close(semaphore2);
    // sem_unlink(SEM2_NAME);
    
    // sem_close(semaphore3);
    // sem_unlink(SEM3_NAME);

    // if(semaphore == SEM_FAILED){
    //     perror("sem_open(1) error");
    //     exit(EXIT_FAILURE);
    // }
    // if(semaphore2 == SEM_FAILED){
    //     perror("sem2_open(2) error");
    //     exit(EXIT_FAILURE);
    // }
    // if(semaphore3 == SEM_FAILED){
    //     perror("sem3_open(3) error");
    //     exit(EXIT_FAILURE);
    // }

    ///////////////////////// Listener and Manager /////////////////////////////

    if(pipe(fd) == -1){ 
        perror ("pipe error"); 
        exit(EXIT_FAILURE);
    }

    // -----------------------------------------------------
    
    if( (pid = fork()) < 0 ){
        perror ("fork faild"); 
        exit(EXIT_FAILURE);
    }
    if(pid == 0){         // child and Listener 
        close(fd[READ]);
        dup2(fd[WRITE], 1); // the (new)standrad output 
                            // => goes to the input of manager
        if( execl("/usr/bin/inotifywait","usr/bin/inotifywait", "-m", "-e", "create", "-e", "moved_to",dir_to_watch ,NULL) < 0){
            perror("execl-(listener) failed");
            exit(EXIT_FAILURE);
        }
    }
    if(pid > 0){    // parent and reacever
        close(fd[WRITE]);
        
        while(1){
            dup2(fd[READ], 0);
            
            n = read(fd[READ], buffer, BUFSIZ);
            printf("parent: %s", buffer);

            signal(SIGCHLD, sig_handler);

            if( queue_workers.empty() || not_available(queue_workers.front()) ){
                // make a worker
                printf("make a new worker\n");

                if( (child = fork()) < 0 ){ 
                    perror ("fork faild"); 
                    exit(EXIT_FAILURE);
                }

                if(child > 0){     // manager
                    //char fifo[] = "/tmp/fifo";
                    printf("eeelaaa\n");
                    if( (mkfifo(FIFO1, 0666) < 0) && (errno != EEXIST) ) {
                        perror("can't make fifo");
                    }
                    printf("eeelaaa 2\n");
                    if((fd1 = open(FIFO1,O_WRONLY)) < 0){
                        perror("manager: can't open fifo");
                    }
                    printf("manager write to pipe..\n");
                    if(write(fd1, buff, n) != n ){
                        perror("manager: filename write error");
                    }
                    // pushing the worker in the queue
                    queue_workers.push({getpid(),fd1});
                    
                    printf("elaaa 3\n");
                    printf("the queue is:\n");
                    showQueue(queue_workers);
                }
                if(child == 0){
                    printf("manager's child = worker %ld\n", (long)getpid());
                    if(execl(WORKERS, WORKERS, NULL) < 0){
                        perror("execl failed");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else if(!not_available(queue_workers.front())  ){
                // we don't keep the workers after they done 
                // we do not need this child-worker anymore
                // so we killling ittt
                printf("killing the child-worker %ld\n", (long)getpid());
                kill(child,SIGKILL);
            }
            else{
                // ???????????????
            }
            printf("the queue is:\n");
            showQueue(queue_workers);
            
        //end of while
        }   

    }


    // while(1){
    //     char* p;
    //     fscanf(stdin,"%s",p);
    //     if(strcmp(p,"exit"))
    //         break;
    // }

}

    



bool not_available(pair<pid_t, int> q){
	pid_t f = q.first; // Gives first element from queue pair
    if(waitpid(f, NULL, WNOHANG) == 0)
        return true;
    return false;
}

