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

void handler(int signum){
    signal(SIGSTOP, handler);
}

int main(int argc, char **argv){
    
    if(argc != 2){
        fprintf(stderr,"Wrong arguments!\n");
        exit(EXIT_FAILURE);
    }
    
    int fd, n;    
    char buff[BUFSIZ];
    char* pipe = argv[1];
    
    printf("pipe: %s\n", pipe);
	printf("------------- I'm the Worker %ld ------------------------ \n", (long)getpid());

    printf("worker-%d opening the fifo\n", getpid());
    if( (fd = open(pipe, O_RDONLY))  < 0){
        perror("worker: can't open fifo");
    }

    printf("worker-%d elaaaaaaa\n", getpid());
    
    while(( n = read(fd, buff, BUFSIZ)) >0 ){
        printf("worker: %s\n", buff);
    }
    printf("worker-%d elaaaaaaa 2\n", getpid());
    
    


    // otan teleiwsei stelnei sigstop
    handler(SIGSTOP);


}
