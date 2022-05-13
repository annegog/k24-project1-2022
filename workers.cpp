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

#include "worker.h"

#define MAXBUFF 2048
#define BUFREAD 1024

char* separate(char* );


void handler(int signum){
    signal(SIGSTOP, handler);
}

int main(int argc, char **argv){
    
    if(argc != 2){
        fprintf(stderr,"Wrong arguments!\n");
        exit(EXIT_FAILURE);
    }
    
    int fd;    
    char buff[BUFSIZ];
    int take;
    char* pipename = argv[1];

    char buffer_file[BUFSIZ];
    int read_file;

    char buff_http[BUFREAD];
    char* token;
    char *new_token = buff_http;
    char s[10] = " ";
    char *ret;
    //char out[BUFREAD];
    char *out = "10.out";
    int counter = 0;        

    printf("====================================================================\n");
	printf("----------------------------------I'm the Worker %ld\n", (long)getpid());
        
    printf("worker-%d open the pipe: %s\n", getpid(), pipename);
    if( (fd = open(pipename, O_RDONLY))  < 0){
        perror("worker: can't open pipe");
    }
    printf("worker-%d read now\n", getpid());
    while( (take = read(fd, buff, BUFSIZ)) > 0){

        /************************ Making the .out file*********************************/
        
        //sprintf(out, "%s.out", buff);
        FILE *out_file = fopen(out, "w"); // write only 
        // test for files not existing. 
        if ( out_file == NULL){   
            printf("Error! Could not open file\n"); 
            exit(-1); // must include stdlib.h 
        } 
        fprintf(out_file, "The URLs are:\n"); // write to file

        /*********************************************************/

        printf("worker open the file: %s\n",buff);
        if((read_file = open("./new_files/10.txt", O_RDONLY)) < 0){
            perror("worker: can't open file");
            exit(EXIT_FAILURE);
        }
        printf("worker can read the file now!!!!\n"); 
        
        while( read(read_file, buffer_file, BUFREAD) > 0){        
            // get the first token
            token = strtok(buffer_file, s);
            while( token != NULL ) {
                ret = strstr(token, "http://"); //comp
                if(ret){
                    counter++;
                    strcpy(new_token,token);
                    // strcpy(new_token, separate(new_token)); printf("%s\n",new_token);
                    
                    fprintf(out_file, "-- %s\n", new_token); // write to file 
                }
                token = strtok(NULL, s);
            }
        }
        close(read_file);
        fprintf(out_file,"*Contains: %d*", counter);
        fclose(out_file);

        printf("worker-%d stop, darling\n", getpid());
        printf("EXITING OF CHILD: %d \n", getpid());

    }
    // close file
    //kill(child,SIGSTOP); 
    return 0;
}

char* separate(char* http){
    char s[2] = "/";
    char* temp;
    char* url = http;
    temp = strtok(http, s);
    for( int i=0; i < 2; i++) {
        strcpy(url, temp);
        temp = strtok(NULL, s);
    }
    //printf("%s \n",url);
    return url;
}