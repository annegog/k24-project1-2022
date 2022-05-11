# k24-project1-2022
//Anna Googula 115201800305

-----------------
// g++ -g -Wall manager.cpp -o sniffer 
// g++ -g -Wall workers.cpp -o workers 
// ./sniffer -p ./new_files
------------------

* listener: it's a kids (pid) of manager process. Just executing (/callling) the inotifywait to keep an eye on the directory we want. 
* manager: 
    - he is taking as an iput the filename that the child writes in the pipe. Until we stop it
    - initially the manager is making workers/childs 
* workers:
    - 


-------------------------------------------------
__further observations__
