# k24-project1-2022
//Anna Googula 115201800305

------------------

* manager/ manager.cpp: 
    - he is taking as an input a string, that contains the filename. We separate the file name.
    The manager is looking for available workers, if he found one he give him the job. But, if he doesn't find anyone, he is making a new worker.
    Making a __new worker__ (initially the manager is making workers/childs) 
        1) we make a name for the pipe
        2) creating a namedpipe
        3) pushing in the queue the worker and the pipename (we use it for the available workers).
        4) exec the child-manager
        5) we opening the pipe from the parent side, so that the manager can send the file name to the worker (with the pipe).
    - ???????? When the manager wants to stop the program, we have to close se fifos, pop the queue and ???

* workers/ workers.cpp:
    - The worker opens the pipe and wainting to read from the manager.
        1) We making a .out file for the filename that the worker reads,
        2) open the file and reads bytes to bytes. We separate every word we read and if we find a string that matched with the "http://" string we keep it
        3) ???? Now we parse se URL when we found a "/". The second sting is the one that we want. We saved it και τωρα πρεπει να ξεσωρισουμε τα www. αν υπάρχουν.
        4) We write to the file_name.out the domains/
        5) close the file we were readind, close the .out file and 
        be stoped so the parent knows! 

* listener: it's a kids (pid) of manager process. Just executing (/callling) the inotifywait to keep an eye on the directory we want. 

-------------------------------------------------
__further observations__


-----------------
* Makefile is a pice of shit :)
// g++ -g -Wall manager.cpp -o sniffer 
// g++ -g -Wall workers.cpp -o workers 
// ./sniffer -p ./new_files/