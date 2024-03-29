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

#define MAXBUFF 2048
#define BUFREAD 1024

#include "manager.h"

#include <iostream>
#include <iterator>
#include <map>

char* separate(char* );
char* separate_w(char* );

void handler(int signum){
    printf("This is a handler in a worker// Stop the child\n");
    signal(SIGSTOP, handler);
}


int main(int argc, char **argv){
    
    if(argc != 2){
        fprintf(stderr,"Wrong arguments!\n");
        exit(EXIT_FAILURE);
    }
    
    int fd;
    char buff[BUFREAD];
    int take;
    char* pipename = argv[1];

    char buffer_file[BUFSIZ];
    int read_file;

    char out[BUFREAD];

    char buff_http[BUFREAD];
    char* token;
    char *new_token = buff_http;
    char s[10] = " ";
    char *ret;
    char* pr;
    
    /*********************************************************/
    //opening the named-pipe
    if( (fd = open(pipename, O_RDONLY))  < 0){
        perror("worker: can't open pipe");
    }
    //reading what you reaceve from the parent-manager
    while( (take = read(fd, buff, MAXBUFF)) > 0){
        /************************ Making the .out file******************************/
        sprintf(out, "./out_files/%s.out", buff);   //all the path 
        FILE *out_file = fopen(out, "w"); // write only 
        if ( out_file == NULL){   
            printf("Error! Could not open file\n"); 
            exit(EXIT_FAILURE); 
        }    
        /*********************************************************/
        // open the new file, to processed it 
        if((read_file = open(buff, O_RDONLY)) < 0){
            perror("worker: can't open file");
            exit(EXIT_FAILURE);
        }

        /****************************************************/
        // read every 1220 bytes from the file_name.txt
        while( read(read_file, buffer_file, 120) > 0){        
            // get the first token
            token = strtok(buffer_file, s);
            while( token != NULL ) {
                ret = strstr(token, "http://"); // compare the token with the string
                if(ret){    //found a "http://....."
                    new_token = token;   
                    pr = separate(new_token); 
                    // prospathhsa na to counter kathe url, BUUUUUT it was wrong!
                    fprintf(out_file, "%s\n", pr); // write the url file in the file_name.out
                }
                token = strtok(NULL, s);
            }
        }
        close(read_file);
        fclose(out_file);

        /******************************************************/

        //printf("EXITING OF CHILD: %d \n", getpid());
        _exit(EXIT_SUCCESS);
    }
    return 0;
}


char* separate(char* http){
    char s[2] = "/";
    char* temp;
    char* url = http;
    char* ret;
    temp = strtok(http, s);
    for( int i=0; i < 2; i++) {
        strcpy(url, temp);
        temp = strtok(NULL, s);
    }
    // if((ret = strstr(url, "www."))){
    //     separate_w(url);
    // }
    return url;
}

char* separate_w(char* http){
    char s[2] = ".";
    char temp[1024];
    char* url;
    strcpy(temp,http);

    url = strtok(temp, s);
    url = strtok(NULL, s);

    for( int i=0; i < 3; i++) {
        //strcpy(url, temp);    
        url = strtok(NULL, s);
    }
    //printf("%s \n",url);
    return url;
}
