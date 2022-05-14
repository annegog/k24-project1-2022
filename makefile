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
EXEC = sniffer

# Παράμετροι για δοκιμαστική εκτέλεση
#ARGS = 

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)
	@if [ -f $(EXEC).exe ]; then ln -sf $(EXEC).exe $(EXEC); fi

clean:
	rm -f $(OBJS) $(EXEC) $(OUT1) $(OUT2)

run: $(EXEC)
	./$(EXEC)

valgrind: $(EXEC)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)