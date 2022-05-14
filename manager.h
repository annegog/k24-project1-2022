#ifndef MANAGER_H_
#define MANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>

#include <iostream>
#include <queue>

using namespace std;


/*********************** Manager's handlers ******************/

void sig_handler(int signum){
    printf("-- Inside a handler// Stop the prorgam now! Kill your childs now\n");
    signal(SIGCHLD, sig_handler);
}

void child_handler(int signum){
    printf("-- Inside a handler// Continue to the child\n");
    signal(SIGCONT, child_handler);
}
void child1_handler(int signum){
    printf("-- Inside a handler// The child is stopped\n");
    signal(SIGSTOP, child1_handler);
}

/***********************************************************/

// The manager can write to the pipe.
void manager_messege(char* file_name, int manager_read, int fd1){
    //printf("manager write to the pipe now!!!!\n");
    if (write(fd1, file_name, manager_read) != manager_read){
        perror("manager: write error");
    }
    if(manager_read < 0)
        perror("manager: error!");
}


/************* Queue  ***************/

char* takeFifo(pair<pid_t, char* > p){
	// Gives second element from queue pair
	char* s = p.second;
	return s;
}

int takeChild(pair<pid_t, char* > p){
	// Gives first element from queue pair
	int s = p.first;
	return s;
}

void printPair(pair<pid_t, char* > p){
	// Gives first element from queue pair
	pid_t f = p.first;
	// Gives second element from queue pair
	char* s = p.second;
    printf("reeeeeeeeeeeeeeeeee\n");
	cout << "(" << f << ", " << s << ") ";
}

// Print the Queue of Pairs
void showQueue(queue<pair<pid_t, char*> > oyra){
	// Print element until the
	// queue is not empty
    printf("ela kalh moy oyraaaaaaaa\n");
	while (!oyra.empty()) {
		printPair(oyra.front());
		oyra.pop();
	}
	cout << '\n';
}

/*******************************************************/

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
    return old;
}

#endif
