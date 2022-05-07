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

#define MAXBUFF 1024
#define FIFO1   "/tmp/fifo-manager-workers.1"
#define FIFO2   "/tmp/fifo-manager-workers.2"

main (int argc, char** argv) {
    int fd, n;    
    int readfd, writefd;

    char buff[MAXBUFF];
    char errmesg[256];

    //////////////////////

    if ( (readfd = open(FIFO1, O_RDONLY))  < 0){
        perror("worker: can't open read fifo");
    }
 
    if ( (writefd = open(FIFO2, O_WRONLY))  < 0){
        perror("worker: can't open write fifo");
    }

    // Read the filename from the IPC descriptor.
    if ((n= read(readfd, buff, MAXBUFF)) <= 0) {
        perror("w: filename read error ");
    }

    buff[n] = '\0';  // null terminate filename 

    if( (fd = open(buff, 0)) <0) {
        /* Error. Format an error message and send it
        *  back to the client */
        sprintf(errmesg, ":can't open, %s\n", buff);
        strcat(buff, errmesg);
        
        n = strlen(buff);
        if (write(writefd, buff, n) != n)  {
            perror("w: errmesg write error");
        }
    } 
    else{
    /* Read the data from the file and write to
     * the IPC descriptor. */

        while ( (n = read(fd, buff, MAXBUFF)) > 0)
            if (write(writefd, buff, n) != n) {
                perror("w: data write error");
            }
        if (n < 0) {
        perror("w: read error");
        }
    }

}