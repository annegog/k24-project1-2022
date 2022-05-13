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
    char buff[BUFREAD];
    int take;
    char* pipename = argv[1];

    char buffer_file[BUFSIZ];
    int read_file;

    char buff_http[BUFREAD];
    char* token;
    char *new_token = buff_http;
    char s[10] = " ";
    char *ret;

    char out[BUFREAD];
    int counter = 0;
    
	printf("----------------------------------I'm the Worker %ld\n", (long)getpid());
        
    printf("worker-%d open the pipe: %s\n", getpid(), pipename);
    //opening the named-pipe
    if( (fd = open(pipename, O_RDONLY))  < 0){
        perror("worker: can't open pipe");
    }
    printf("worker-%d read now\n", getpid());
    //reading what you reaceve from the parent-manager
    while( (take = read(fd, buff, MAXBUFF)) > 0){
    
        /************************ Making the .out file******************************/
        sprintf(out, "./out_files/%s.out", buff);   //all the path 
        FILE *out_file = fopen(out, "w"); // write only 
        if ( out_file == NULL){   
            printf("Error! Could not open file\n"); 
            exit(EXIT_FAILURE); 
        } 
        fprintf(out_file, "The URLs are:\n");
    
        /*********************************************************/
        
        printf("worker open the file: %s\n",buff);
        // open the new file, to processed it 
        if((read_file = open(buff, O_RDONLY)) < 0){
            perror("worker: can't open file");
            exit(EXIT_FAILURE);
        }
        printf("worker can read the file now!!!!\n"); 
        
        /****************************************************/
        // read every 1024 bytes from the file_name.txt
        while( read(read_file, buffer_file, BUFREAD) > 0){        
            // get the first token
            token = strtok(buffer_file, s);
            while( token != NULL ) {
                ret = strstr(token, "http://"); // compare the token with the string
                if(ret){    //found a "http://....."
                    counter++;
                    strcpy(new_token,token);    
                    // strcpy(new_token, separate(new_token)); printf("%s\n",new_token);
                    
                    fprintf(out_file, "-- %s\n", new_token); // write the url file 
                }
                token = strtok(NULL, s);
            }
        }
        close(read_file);
        fprintf(out_file,"*Contains: %d*", counter);
        fclose(out_file);

        /******************************************************/

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