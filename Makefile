
OBJS 	= manager.o workers.o
SOURCE	= manager.cpp workers.cpp
#HEADER  = 
OUT  	= sniffer
CC	= g++
FLAGS   = -g -Wall
DIR = ./new_files

all:$(OUT)

$(OUT): manager.o $(OBJS)
	$(CC) $(FLAGS2) manager.o $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<< 
manager.o: manager.c
	$(CC) $(FLAGS) manager.c
workers.o: workers.c
	$(CC) $(FLAGS) workers.c

# valgrind
valgrind:
	valgrind  --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./sniffer -p $(DIR)

#valgrind2:

run:
	./sniffer -p $(DIR)
clean:
	rm -f $(OBJS) $(OUT) manager.o workers.o