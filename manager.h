#ifndef MANAGER
#define MANAGER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>

#include <iostream>
#include <queue>

using namespace std;


void sig_handler(int signum){
    signal(SIGCHLD, sig_handler);
}

void child_handler(int signum){
    signal(SIGCONT, child_handler);
}
void child1_handler(int signum){
    signal(SIGSTOP, child1_handler);
}
void handler_1(int signum){
    signal(SIGINT, handler_1);
}



char* takeFifo(pair<pid_t, char* > p){
	// Gives second element from queue pair
	char* s = p.second;
	return s;
}

char * separeta(char* buff){
        char s[2] = " ";
        char* token;
        char *old = buff;
   
        /* get the first token */
        token = strtok(buff, s);
            
        /* walk through other tokens */
        while( token != NULL ) {
            strcpy(old, token);
            token = strtok(NULL, s);
        }
    return buff;
}



#endif
