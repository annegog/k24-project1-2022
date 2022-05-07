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

#define WORKERS "./workers"

#define MAXBUFF 1024
#define FIFO1   "/tmp/fifo-manager-workers.1"
#define FIFO2   "/tmp/fifo-manager-workers.2"


void handler(int signal){ 
    // LOGIKA THELEI KI ALLA
    // SIGCHILD: 
    //    na dw poio paidi to esteile 
    //    kai poia h katastash toy
    //    ta shmeiwnw kapoy 
    //    gia na ta xrhsimopoihsw meta 
    exit(0);
}

int main (int argc, char **argv) {

    signal(SIGINT /* | SIGCHLD */, handler);

    pid_t pid, child;
    int fd[2];

    char* dir_to_watch = "./new_files/";
    
    if(pipe(fd) == -1){ 
        perror ("pipe error"); 
        exit(EXIT_FAILURE);
    }
    if( (pid = fork()) < 0 ){
        perror ("fork faild"); 
        exit(EXIT_FAILURE);
    }
    
    if(pid == 0){         // child and "writer" to the pipe // Listener 
        close(fd[READ]);
        dup2(fd[WRITE], 1); // the (new)standrad output 
                            // => goes to the input of manager  
        
        for(;;){
            if(execlp("inotifywait","inotifywait -m -e create -e moved_to",
            dir_to_watch, NULL) < 0){
                perror("execlp (listener) failed");
                exit(EXIT_FAILURE);
            }
        }
    }
    if(pid > 0){        // parent and reacever
        close(fd[WRITE]);
        dup2(fd[READ], 0);
        
        // na apothikeysw kapoy ta arxeia poy diabazw?????

    }
    /////////////////// MANAGER-WORKERS CONNECTION ///////////////////
    
    int readfd, writefd;
    char buff[MAXBUFF];
    int n;

    if( (child = fork()) < 0 ){ 
        perror ("fork faild"); 
        exit(EXIT_FAILURE);
    }
    if(child > 0){      //manager
        
        if( mkfifo(FIFO1, 0666) == -1 ){
            if( errno != EEXIST ){
                perror ("manager: mkfifo 1 error");
                exit(6);
            }
        }
        if( mkfifo(FIFO2, 0666) == -1 ){
            if( errno != EEXIST ){
                perror ("manager: mkfifo 2 error");
                exit(6);
            }
        }

        if( (writefd = open(FIFO1 , O_RDWR )) < 0){ 
            perror("m: fifo2 open error!"); 
            exit(1);
        }
        if( (readfd = open(FIFO1 , O_RDONLY )) < 0){ 
            perror("m: fifo1 open error!"); 
            exit(1);
        }
        
        /* Read the filename from standard input, 
        *  write it to the IPC descriptor.
        */
        if (fgets(buff, MAXBUFF, stdin) == NULL)
            perror("m: filename read error \n");

        n = strlen(buff);
        if (buff[n-1] == '\n')
            n--;     /* ignore newline from fgets() */

        if (write(writefd, buff, n) != n)
            perror("m: filename write error");
        
        /* Read data from the IPC descriptor and write to
        * standard output. 
        * εγω δεν θελω να τα γραψω στο standard output !!
        */

        while ( (n = read(readfd, buff, MAXBUFF)) > 0)
            if (write(1, buff, n) != n)   /* fd 1 = stdout */ {
                perror("m: data write error \n");
            }
        if (n <0) { 
            perror("m: data read error \n");
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