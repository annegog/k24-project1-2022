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
#include <sys/shm.h>

#include <iostream>
#include <queue>
#include <deque>

#include "manager.h"

#define WORKERS "./workers"

#define READ    0
#define WRITE   1 

#define MAXBUFF 2048
#define NAMESBUFF 1024

using namespace std;

queue <pair<pid_t, char*>> queue_workers;


void catchinterrupt(int signo){
    if (signo == 2){
        printf("I caught a SIGINT\n");
	}
    printf("Catching: returning\n");
}

int main(int argc, char **argv){
    
    // if(argc != 3){
    //     fprintf(stderr,"Wrong arguments!\n");
    //     exit(EXIT_FAILURE);
    // }

    pid_t pid, child;
    int fd[2], fd1;
    char buffer[NAMESBUFF];
    int manager_read, counter = 1;
    char pipename[NAMESBUFF];
    
    int status;

    /************************* Listener and Manager *****************/

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
        if( execl("/usr/bin/inotifywait","usr/bin/inotifywait", "-m", "-e", "create", "-e", "moved_to", "./", NULL) < 0){
            perror("execl-(listener) failed");
            exit(EXIT_FAILURE);
        }
    }
    if(pid > 0){    //parent
        close(fd[WRITE]);
        dup2(fd[READ], 0);

        while( (manager_read = read(fd[READ], buffer, MAXBUFF)) > 0){
            
            static struct sigaction act;
            act.sa_handler=catchinterrupt;
            sigfillset(&(act.sa_mask));

            sigaction(SIGINT, &act, NULL);
            sigaction(SIGSTOP, &act, NULL);
            
            // if(signal(SIGINT, sig_handler)){
            //     kill(child, SIGINT);
            // }

            //sleep(1);
            /****************************************************/
            //separeting the string we getting from the listener
            char* file_name = separeta(buffer);
            int len = strcspn(file_name,"\n");
            file_name[len] = '\0';
            printf("manager is reading: %s\n",file_name);

            /****************************************************/

            // catch the signal of a stopped child
            
            // supposedly here we're looking for the available childs 
            // BUUUUUT I didn't do it! (I tried) ->*
            /*
            pid_t child_pid;
            if( child_pid = waitpid(-1, &status, WNOHANG | WUNTRACED) > 0){
                // if there is an available worker
                // I'm giving him the file to proccesse it
                // don't make a new worker
                
                // signal(SIGCONT, child_handler);
                if(execl(WORKERS, WORKERS, takeFifo(queue_workers.front()), NULL) < 0){
                    perror("execl failed");
                    exit(EXIT_FAILURE);
                }
                // the pipe (it's supposed to be)/(is already) open now. We don't need to make another  
                manager_messege(takeFifo(queue_workers.front()), manager_read, fd1);

            }
            else{
            */
            // ->* So everytime we have a new file, we're making a new worker :(

                // naming the name pipe, with the path so it goes to the temporary 
                sprintf(pipename, "/tmp/worker-manager.pipe_%d", counter);
                counter++;
                // creating a child-worker  
                printf("make a child\n");                  
                if( (child = fork()) < 0 ){ 
                    perror ("fork faild"); 
                    exit(EXIT_FAILURE);
                }
                if(child == 0){                    
                    // Create a namedpipe for the worker-manager connection
                    printf("start creating a pipe...\n");
                    if(mkfifo(pipename, 0666) == -1){
                        if( errno != EEXIST ){
                            perror("manager: mkfifo error");
                            exit(6);
                        }
                    }
                    /****************************************************/
                    // pushing the worker and the pipename in the queue and execute the child
                    queue_workers.push(pair <pid_t, char*>(getpid(),pipename));
                    if(execl(WORKERS, WORKERS, pipename, NULL) < 0){
                        perror("execl failed");
                        exit(EXIT_FAILURE);
                    }
                }
                sleep(1);  // maybe we don't need this!!! I don't know yet...

                // manager is opening the pipe
                // so he can send the file name to the worker
                if((fd1 = open(pipename,O_WRONLY)) < 0){
                    perror("manager: can't open pipe");
                }
                else{
                    // write in the fifo/ pipe.
                    manager_messege(file_name, manager_read, fd1);
                }
            //}
        } 
        
        printf("Out of the while read\n");
        while(waitpid(-1, &status, WNOHANG)){
            printf("kill the child:%d", getpid());
            kill(getpid(),SIGHUP);
        }
        printf("Your childs are DEAD!\n");
        while (!queue_workers.empty()){
            printf("Empty the queue\n");
            queue_workers.pop();
        }
              //να κλείσω fifo και δεν ξερω τι αλλο     
                   
    }
    close(fd[1]);
    close(fd[2]);

    return 0;
}
