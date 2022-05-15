#ifndef MANAGER_H_
#define MANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>

#include <iostream>
#include <deque>
#include <algorithm>

using namespace std;

/*********************** Manager's handlers ******************/

void sig_handler(int signum){
    printf("-- Inside a handler// Stop the prorgam now! Kill your childs now\n");
    signal(SIGCHLD, sig_handler);
}

/***********************************************************/

// Manager write to the pipe.
void manager_messege(char* file_name, int manager_read, int fd1){
    //printf("manager write to the pipe now!!!!\n");
    if (write(fd1, file_name, manager_read) != manager_read){
        perror("manager: write error");
    }
    if(manager_read < 0)
        perror("manager: error!");
}


/************* Deque  ***************/
void showdq(deque<char*> g){
	deque<char*>::iterator it;
	for (it = g.begin(); it != g.end(); ++it)
		cout << '\t' << *it;
	cout << '\n';
}

void showdq2(deque<int> g){
	deque<int>::iterator it;
	for (it = g.begin(); it != g.end(); ++it)
		cout << '\t' << *it;
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
