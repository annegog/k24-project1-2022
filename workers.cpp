#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <error.h>
#include <semaphore.h>
#include <sys/shm.h>

#define MAXBUFF 1024

#define SEM_NAME "semaphore1"
#define SEM2_NAME "semaphore2"
#define SEM3_NAME "semaphore3"


void handler(int signum){
    signal(SIGSTOP, handler);
}

int main(int argc, char **argv){
    
    if(argc != 2){
        fprintf(stderr,"Wrong arguments!\n");
        exit(EXIT_FAILURE);
    }
    
    int fd;    
    // char buff[BUFSIZ];
    char* pipename = argv[1];
    
    printf("====================================================================\n");
	printf("----------------------------------I'm the Worker %ld\n", (long)getpid());
    
    printf("pipe: %s\n", pipename);
    
    printf("worker-%d open the pipe\n", getpid());
    if( (fd = open(pipename, O_RDONLY))  < 0){
        perror("worker: can't open pipe");
    }

    // read

    printf("worker-%d stop now, darling lalalalaalalalala\n", getpid());


    printf("EXITING OF CHILD: %d \n", getpid());

    return 0;
                    
    //kill(child,SIGSTOP);

}
