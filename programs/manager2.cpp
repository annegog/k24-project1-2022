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

// Print the current pair
void printPair(pair<int, int> p);
// Print the Queue of Pairs
void showQueue(queue<pair<int, int> > gq);
bool not_available(pair<pid_t, int> q);


void handler(int signum){ 
    // LOGIKA THELEI KI ALLA
    // SIGCHILD: 
    //    na dw poio paidi to esteile 
    //    kai poia h katastash toy
    //    ta shmeiwnw kapoy 
    //    gia na ta xrhsimopoihsw meta
    signal(SIGINT, handler);
    exit(0);
}

void sig_handler(int signum){
    signal(SIGCHLD, sig_handler);
}


int main(){
    
    signal(SIGINT, handler);
    
    pid_t pid, child;
    int fd[2];
    char buff[BUFSIZ];

    int fd1, fd2;
    int n;

    int take_from_the_listener = 1;

    char* dir_to_watch = "./new_files";
    
    queue <pair<pid_t, int> > queue_workers;

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

        // n = read(fd[READ], buff, BUFSIZ);
        // printf("%s", buff);
        //take_from_the_listener = 0;
        
    }
    
    /////////////////// MANAGER-WORKERS CONNECTION ///////////////////
    fprintf(stdout,"M: 1\n");

    // Making 2 named pipes so we can 
    // "send" the files to our workers

    if(mkfifo(FIFO1, 0666) == -1){
        if( errno != EEXIST ){
            perror("manager: mkfifo 1 error");
            exit(6);
        }
    }
    fprintf(stdout,"M: 2\n");
    if(mkfifo(FIFO2, 0666) == -1){
        if( errno != EEXIST ){
            perror("manager: mkfifo 2 error");
            exit(6);
        }
    }
    fprintf(stdout,"M: 3\n");
    if( (fd1 = open(FIFO1, O_WRONLY)) <0){
        perror("m: fifo1 open error");
        exit(1);
    }
    if( (fd2 = open(FIFO2, O_WRONLY)) <0){
        perror("m: fifo1 open error");
        exit(1);
    }
    
    fprintf(stdout,"M: 4\n");

    signal(SIGCHLD, sig_handler);

    // if the queue is empty or there is no available worker
    // we create the (first or) new worker
    while( 1 ){
        fprintf(stdout,"hellooooo\n");
        
        if( not_available(queue_workers.front()) || queue_workers.empty() ){
            
            printf("aaa make a new worker\n");
            
            if( (child = fork()) < 0 ){ 
                perror ("fork faild"); 
                exit(EXIT_FAILURE);
            }
            if(child > 0){     // manager
            // Read the filename from the std input of the manager.
            // => give it to the worker    
            if(write(fd1, buff, BUFSIZ) < BUFSIZ)
                perror("m: filename write error");
            } 
            if(child == 0){     //worker
                // pushing the worker in the queue
                queue_workers.push({getpid(),fd1});
                if(sem_post(semaphore2) < 0){
                    perror("sem_post (sem2) failed on manager");
	            }
                if(execl(WORKERS, WORKERS, NULL) < 0){
                    perror("execl failed");
                    exit(EXIT_FAILURE);
                }
            }

        }
                  
    }
            
    while(waitpid(-1,NULL,WNOHANG | WUNTRACED) > 0){ 
        /*add to queue */ 
    }
        
}


bool not_available(pair<pid_t, int> q){
	pid_t f = q.first; // Gives first element from queue pair
    if(waitpid(f,NULL,WNOHANG)>0)
        return true;
    return false;
}

// Print the current pair
void printPair(pair<pid_t, int> p)
{
	// Gives first element from queue pair
	pid_t f = p.first;
	// Gives second element from queue pair
	int fd = p.second;

	cout << "(" << f << ", " << fd << ") ";

    //if(waitpid(f,NULL,))

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