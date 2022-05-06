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

#define MSGSIZE 65

char* fifo = "manager_workers";

main (int argc, char* argv[]) {
    int fd, i, nwrite ;
    char msgbuf[MSGSIZE +1];
    
    if(argc >2) {
        printf("Usage: receive the message\n") ;
        exit(1) ;
    }
    
    if( ( fd = open(fifo, O_RDONLY )) < 0) {
        perror ("fifo open problem !");
        exit(3);
    }
    for (;;) {
        if( read(fd, msgbuf, MSGSIZE +1) < 0) {
            perror("problem in reading !");
            exit(5);
        }
        printf("\nMessage Received: %s\n", msgbuf);
        fflush(stdout);
    }

}