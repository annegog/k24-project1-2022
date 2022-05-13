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
#include <dirent.h>

#include "worker.h"

#define FOLDER "./new_files/"

#define MAXBUFF 2048

void handler(int signum){
    signal(SIGSTOP, handler);
}

int main(int argc, char **argv){
    
    if(argc != 2){
        fprintf(stderr,"Wrong arguments!\n");
        exit(EXIT_FAILURE);
    }
    
    int fd, file;    
    char buff[MAXBUFF];
    char buffer_file[BUFSIZ];
    int take, read_file;
    char* pipename = argv[1];
    char* file_name;

    char http[8] = "http://"; 
    char buff_http[MAXBUFF];
    char* token;
    char *old = buff_http;

    printf("====================================================================\n");
	printf("----------------------------------I'm the Worker %ld\n", (long)getpid());
    
    // DIR *folder;
    // folder = opendir("./new_files");
    // if(folder == NULL){
    //     puts("Unable to read directory");
    //     return(1);
    // }else{
    //     puts("Directory is opened!");
    // }

    char path[200] = "./new_files/";

    printf("pipe: %s\n", pipename);
    printf("worker-%d open the pipe\n", getpid());
    if( (fd = open(pipename, O_RDONLY))  < 0){
        perror("worker: can't open pipe");
    }

    printf("worker-%d read now\n", getpid());
    while( (take = read(fd, buff, BUFSIZ)) > 0){
        printf("worker open the file: %s!!!!!!!!!size= %zu\n",buff, strlen(buff));
        strncat(path,buff,strlen(buff));
        printf("%s!!!!!!!!! size= %zu\n",path, strlen(path));
        if((read_file = open(path, O_RDONLY)) < 0){
            perror("worker: can't open file");
        }
        else{
            printf("worker read the file now!!!!\n");
            while( read(read_file, buffer_file, MAXBUFF) > 0){
                //printf("reading: %s\n",buffer_file);
                printf("Search for the URLs\n");
                /* get the first token */
                // token = strtok(buff, http);
                    
                // /* walk through other tokens */
                // while( token != NULL ) {
                //     strcpy(old, token);
                //     token = strtok(NULL, http);
                // }

            }           
        }
    
        
    }
    
    printf("worker-%d stop, darling\n", getpid());
    printf("EXITING OF CHILD: %d \n", getpid());
    // close file
    // closedir(folder);
    //kill(child,SIGSTOP); 
    return 0;
}

/*  char buff[] = "Hello there, peasants!";
    char *found;

    printf("Original string: '%s'\n",buff);

    found = strtok(buff," ");
    if( found==NULL)
    {
        printf("\t'%s'\n",string);
        puts("\tNo separators found");
        return(1);
    }
    while(found)
    {
        printf("\t'%s'\n",found);
        found = strtok(NULL," ");
    }
*/