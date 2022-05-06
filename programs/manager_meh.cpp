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

#define READ 0
#define WRITE 1

#define MSGSIZE 65
#define WORKERS "./workers"

char* fifo = "manager_workers";

void handler(int signal){ 
    // LOGIKA THELEI KI ALLA
    // SIGCHILD: 
    //    na dw poio paidi to esteile 
    //    kai poia h katastash toy
    //    ta shmeiwnw kapoy 
    //    gia na ta xrhsimopoihsw meta 
    exit(0);
}

int main (int argc , char **argv) {

    signal(SIGINT /* | SIGCHLD */, handler);

    pid_t pid, child;
    int fd[2];

    char* dir_to_watch = "./new_files";
    
    int fd1, i, nwrite;
    char msgbuf[MSGSIZE +1];
    
    if (pipe(fd) == -1){ 
        perror ("pipe error"); 
        exit(EXIT_FAILURE);
    }
    if( (pid = fork()) < 0 ){ 
        perror ("fork faild"); 
        exit(EXIT_FAILURE);
    }
    if (pid == 0){         // child and "writer" to the pipe => Listener 
        close(fd[READ]);
        dup2(fd[WRITE], 1);
        

        if(execlp("inotifywait","inotifywait -m -e create -e moved_to",
           dir_to_watch, NULL) < 0){
            perror("execlp (listener) failed");
            exit(EXIT_FAILURE);
        }
    }
    else{                   // parent and reader
        close(fd[WRITE]);
        dup2(fd[READ], 0);

        //
        
    }

    /////////////////// MANAGER-WORKERS CONNECTION ///////////////////

    if( (child = fork()) < 0 ){ 
        perror ("fork faild"); 
        exit(EXIT_FAILURE);
    }
    if(child > 0){      //manager
        if( mkfifo(fifo, 0666) == -1 ){
            if( errno != EEXIST ){
                perror ("receiver: mkfifo error");
                exit(6);
            }
        }
        if( (fd1 = open(fifo , O_RDWR | O_NONBLOCK )) < 0){ 
            perror("fifo open error!"); 
            exit(1);
        }
        for ( i =1; i < argc; i ++) {
            if(strlen( argv [i]) > MSGSIZE ){
                printf("Message with Prefix %.*s Too long - Ignored\n" ,10 , argv[i]);
                fflush(stdout);
                continue;
            }
            strcpy(msgbuf, argv[i]);
            if( (nwrite = write(fd1, msgbuf, MSGSIZE +1) ) == -1){ 
                perror ("Error in Writing"); 
                exit(2); 
            }
        }
    }
    if (child == 0){
        //workers
        if(execl(WORKERS, WORKERS, NULL) < 0){
                perror("execl (workers) failed");
                exit(EXIT_FAILURE);
        }
    }   
    
}