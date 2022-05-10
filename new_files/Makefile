
OBJS 	= manager.o workers.o
SOURCE	= manager.cpp workers.cpp
HEADER  = manager.h 
OUT  	= sniffer
CC	= g++
FLAGS   = -g -Wall
DIR = ./new_files

all:$(OUT)

$(OUT): $(OBJS)
	$(CC) $(FLAGS2) $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<< 
manager.o: manager.cpp
	$(CC) $(FLAGS) manager.cpp
workers.o: workers.cpp
	$(CC) $(FLAGS) workers.cpp

# valgrind
valgrind:
	valgrind  --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./sniffer -p $(DIR)

#valgrind2:

run:
	$(MAKE) ./sniffer -p $(DIR)
clean:
	rm -f $(OBJS) $(OUT) manager.o workers.o