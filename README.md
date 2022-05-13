# k24-project1-2022
//Anna Googula 115201800305

------------------

* manager/ manager.cpp: 
    - he is taking as an input a string, that contains the filename. We separate the file name.
    The manager is looking for available workers, and if he found one he gives him the job. But, if he doesn't find anyone, he is making a new worker.
    Making a __new worker__ (initially, the manager is making workers/childs) 
        1) we make a name for the pipe
        2) creating a named pipe
        3) push in the queue the worker and the pipename (we use it for the available workers).
        4) exec the child-manager
        5) we open the pipe from the parent side so that the manager can send the file name to the worker (with the pipe).
    - ???????? When the manager wants to stop the program, we have to close the fifos, pop the queue, and ???

* workers/ workers.cpp:
    - The worker opens the pipe and waits to read from the manager.
        1) We make a .out file for the filename that the worker reads,
        2) open the file and reads bytes to bytes. We separate every word we read and if we find a string that matched the "http://" string we keep it
        3) ???? Now we parse the URL when we found a "/". The second string is the one that we want. We saved it και τωρα πρεπει να ξεσωρισουμε τα www. αν υπάρχουν.
        4) We write to the file_name.out the domains/
        5) close the file we were reading, close the .out file and 
        be stopped so the parent knows! 

* listener: it's a kid (pid) of manager process. Just executing (/calling) the inotifywait to keep an eye on the directory we want. 

-------------------------------------------------
__further observations__
* manager.h: 
    - function manager_messege: takes the fifoname and he is writing to the pipe

-----------------
* Makefile is a pice of shit :)
// g++ -g -Wall manager.cpp -o sniffer 
// g++ -g -Wall workers.cpp -o workers 
// ./sniffer -p ./new_files/