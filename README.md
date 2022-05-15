### k24-project1-2022
Anna Googula 115201800305

------------------
* manager/ manager.cpp: 
    - he is taking as an input a string, that contains the filename. We separate the file name.
    (The manager doesn't want to find the available workers, he just makes a new worker every time!)
    Making a __new worker__ (initially, the manager is making workers/childs) 
        1) we make a name for the pipe
        2) creating a named pipe/ fifo.
        3) push in the queue the worker and the pipename (we use it for the available workers).
        4) exec the child-manager
        5) we open the pipe from the parent side so that the manager can send the file name to the worker (with the pipe).
    - ???????? When the manager wants to stop the program, we have to close the fifos, pop the queue, and ???
* manager.h: 
    - function manager_messege: takes the fifoname and he is writing to the pipe
* workers/ workers.cpp:
    - The worker opens the pipe and waits to read from the manager.
        1) We make a .out file for the filename that the worker reads,
        2) open the file and reads bytes to bytes. We separate every word we read and if we find a string that matched the "http://" string we keep it
        3) Now we parse the URL when we find a "/". The second string is the one that we want. We save it. (I did not separate the www. from the urls...) 
        4) We write to the file_name.out the "domains" (without the times we found it in the file...).
        5) close the file we were reading, close the .out file and 
        When it's stops send a signal to the parent! 
* listener: it's a child (pid) of manager process. Just executing (/calling) the inotifywait to keep an eye on the current directory (we putting the new files here).
* finder.sh: 
-------------------------------------------------
__further observations__
- the pipes are open until the end.
- Makefile is a pice of shit :) You can do this instead
g++ -g -Wall manager.cpp -o sniffer 
g++ -g -Wall workers.cpp -o workers 
./sniffer