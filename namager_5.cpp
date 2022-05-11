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

#define WORKERS "./workers"

#define READ    0
#define WRITE   1 

#define MAXBUFF 1024

using namespace std;


void sig_handler(int signum){
    signal(SIGCHLD, sig_handler);
}

void child_handler(int signum){
    signal(SIGCONT, child_handler);
}
void child1_handler(int signum){
    signal(SIGSTOP, child1_handler);
}
void handler_1(int signum){
    signal(SIGINT, handler_1);
}


char* takeFifo(pair<pid_t, char* > p){
	// Gives second element from queue pair
	char* s = p.second;
	return s;
}



int main(int argc, char **argv){
    
    // if(argc != 3){
    //     fprintf(stderr,"Wrong arguments!\n");
    //     exit(EXIT_FAILURE);
    // }

    pid_t pid, child;
    int fd[2];
    int fd1, fd2;
    char buffer[BUFSIZ];
    char buff[BUFSIZ];
    int n;
    char pipename[MAXBUFF];

    
    // char* dir_to_watch = argv[2];
    char* dir_to_watch = "./new_files";

    queue <pair<pid_t, char*> > queue_workers;
    
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
        if( execl("/usr/bin/inotifywait","usr/bin/inotifywait", "-m", "-e", "create", "-e", "moved_to", "./new_files",NULL) < 0){
            perror("execl-(listener) failed");
            exit(EXIT_FAILURE);
        }
    }
    if(pid > 0){ 
        close(fd[WRITE]);
        dup2(fd[READ], 0);

        while( read(fd[READ], buffer, BUFSIZ) > 0){
            printf("parent is reading: %s", buffer);

            if(signal(SIGINT,handler_1)){
                kill(child, SIGINT);
            }

            signal(SIGSTOP,child1_handler);
            
            ////????????????????????????????????
            //????????
            // waitpid(-1, &status, WNOHANG | WUNTRACED)
            //??????????????????????????????????????
            // if(!queue_workers.empty() && signal(SIGCHLD, sig_handler)){
            //     // an yparxei diathesimow worker 
            //     // prepei na parw ayton = den ftiaxnw kainoyrgio worker
                
            //     signal(SIGCONT, child_handler);

            //     // if((fd1 = open(,O_WRONLY)) < 0){
            //     //     perror("manager: can't open pipe");
            //     // }

            // }
            // else{
                printf("We are making a new worker now\n");

                if( (child = fork()) < 0 ){ 
                    perror ("fork faild"); 
                    exit(EXIT_FAILURE);
                }
                if(child > 0){
                    printf("---------------------------I'm the parent-manager--------------------\n");
                
                    
                }
                // naming the pipe
                sprintf(pipename, "worker_%d.pipe", getpid());
                printf("||||||||||||||||--- pipe %s\n", pipename);
                
                if(child == 0){
                    
                    printf("----------------------- Manager's worker -------------------\n");

                    // Create a namedpipe for the worker-manager connection
                    printf("start creating a pipe...\n");
                    if(mkfifo(pipename, 0666) == -1){
                        if( errno != EEXIST ){
                            perror("manager: mkfifo error");
                            exit(6);
                        }
                    }
                    printf("YOURE PIPE IS READY BITCH!!!! %s\n", pipename);
                    
                    printf("pushing worker-%d and his/her pipe:%s\n",getpid(), pipename);
                    queue_workers.push({getpid(), pipename});
                    
                    printf("execl the worker %d -- pipe:%s\n", getpid(), pipename);
                    // if(execl(WORKERS, WORKERS, pipename , NULL) < 0){
                    //     perror("execl failed");
                    //     exit(EXIT_FAILURE);
                    // }

                }

                printf("------------------------------- i'm the parent again --------------------------\n");

                    // manager is opening the pipe so 
                    // so he can send the filename to the worker.
                    printf("manager open the pipe %s\n", pipename);
                    if((fd1 = open(pipename,O_WRONLY)) < 0){
                        perror("manager: can't open pipe");
                    }

                    // if(strcpy(buffer, buff)){
                    //     printf("egine h antigrafh apo to ena buffer sto allo-- %s -- %s\n", buff, buffer);
                    // }
                    // printf("manager write to the pipe now!!!!\n");
                    // write(fd1,buffer,BUFSIZ);   

            // }
                           
        }            

    }


}





