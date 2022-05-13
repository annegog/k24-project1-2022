####
#Anna Gogoula
###

# compiler
CC = g++
# Compile options
CFLAGS = -Wall -g
# Αρχεία .o
OBJS = manager.o workers.o
HEADER  = manager.h


# Το εκτελέσιμο πρόγραμμα
EXEC = manager

# path
DIR = ./new_files/

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS = -p $(DIR)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)
	@if [ -f $(EXEC).exe ]; then ln -sf $(EXEC).exe $(EXEC); fi

clean:
	rm -f $(OBJS) $(EXEC) $(OUT1) $(OUT2)

run: $(EXEC)
	./$(EXEC) $(ARGS)

valgrind: $(EXEC)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)