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

#include "manager.h"

#define WORKERS "./workers"

#define READ    0
#define WRITE   1 

#define MAXBUFF 2048
#define NAMESBUFF 1024

using namespace std;

queue <pair<pid_t, char*> > queue_workers;


int main(){
    
    pid_t pid, child;
    int fd[2], fd1;
    char buffer[NAMESBUFF];
    int manager_read, counter = 1;
    char pipename[NAMESBUFF];
    
    int status;

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
        if( execl("/usr/bin/inotifywait","usr/bin/inotifywait", "-m", "-e", "create", "-e", "moved_to", "./", NULL) < 0){
            perror("execl-(listener) failed");
            exit(EXIT_FAILURE);
        }
    }
    if(pid > 0){    //parent
        close(fd[WRITE]);
        dup2(fd[READ], 0);

        while( (manager_read = read(fd[READ], buffer, MAXBUFF)) > 0){

            if(signal(SIGINT, sig_handler)){
                kill(child, SIGINT);
            }

            //sleep(2);

            /****************************************************/

            char* file_name = separeta(buffer);
            int len = strcspn(file_name,"\n");
            file_name[len] = '\0';
            printf("manager is reading: %s\n",file_name);

            /****************************************************/

            // catch the signal of a stopped child
            signal(SIGSTOP,child1_handler);
            
            // ????????????????????????????????
            // ????????
            // waitpid(-1, &status, WNOHANG | WUNTRACED)
            // ??????????????????????????????????????
            
            if( child = waitpid(-1, &status, WNOHANG | WUNTRACED)){
                // if there is a available worker
                // I'm giving him the file to proccesse it
                // don't make a new worker
                
                // seek for the child that is available
                while(!queue_workers.empty()){
                    if(child == takeChild(queue_workers.front())){
                        printf("------------------ %d",getpid());

                        signal(SIGCONT, child_handler);

                        // the pipe (it's supposed to be)/(is already) open now. 
                        // So we just give the file name
                        manager_messege(file_name, manager_read, fd1);
                    }
                }

            }
            else{
                // naming the name pipe, with the path so it goes to the temporary 
                sprintf(pipename, "/tmp/worker-manager.pipe_%d", counter);
                counter++;
                // creating a child-worker                    
                if( (child = fork()) < 0 ){ 
                    perror ("fork faild"); 
                    exit(EXIT_FAILURE);
                }
                if(child == 0){
                    //printf("---------------------Manager's worker\n");
                    
                    // Create a namedpipe for the worker-manager connection
                    printf("start creating a pipe...\n");
                    if(mkfifo(pipename, 0666) == -1){
                        if( errno != EEXIST ){
                            perror("manager: mkfifo error");
                            exit(6);
                        }
                    }
                    
                    /****************************************************/
                    // pushing the worker and the pipename in the queue
                    //printf("pushing worker: %d and his/her pipe: %s\n",getpid(), pipename);
                    queue_workers.push({getpid(), pipename});
                    
                    //printf("execl the worker %d -- pipe:%s\n", getpid(), pipename);
                    if(execl(WORKERS, WORKERS, pipename, NULL) < 0){
                        perror("execl failed");
                        exit(EXIT_FAILURE);
                    }
                }

                sleep(1);  // maybe we don't need this!!! I don't know yet...
                //printf("------------------I'm the parent-manager\n");

                // manager is opening the pipe so 
                // so he can send the file name to the worker
                //printf("manager open the pipe- %s\n", pipename);
                if((fd1 = open(pipename,O_WRONLY)) < 0){
                    perror("manager: can't open pipe");
                }
                else{
                    manager_messege(file_name, manager_read, fd1);
                }

            }
       
        } 

        while (!queue_workers.empty()){
                queue_workers.pop();
            }
              //να κλείσω fifo και δεν ξερω τι αλλο     
                   
    }
    close(fd[1]);
    close(fd[2]);

    return 0;
}

