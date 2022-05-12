####
#Anna Gogoula
###

# compiler
CC = g++
# Compile options
CFLAGS = -Wall -Werror -g
# Αρχεία .o
OBJS = manager.o workers.o
HEADER  = manager.h

#all:$(OUT1) $(OUT2)

OUT1 = manager
OUT2 = workers

$(OUT1): manager.o $(OBJS)
	$(CC) $(CFLAGS) manager.o $(OBJS) -o $(OUT1)


$(OUT2): workers.o $(OBJS)
	$(CC) $(CFLAGS) workers.o $(OBJS) -o $(OUT2)

# Το εκτελέσιμο πρόγραμμα
EXEC = sniffer

# path
DIR = ./new_files

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS = -p $(DIR)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) (OUT1) $(OUT2) -o $(EXEC)
	@if [ -f $(EXEC).exe ]; then ln -sf $(EXEC).exe $(EXEC); fi

clean:
	rm -f $(OBJS) $(EXEC) $(OUT1) $(OUT2)

run: $(EXEC)
	./$(EXEC) $(ARGS)

valgrind: $(EXEC)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)