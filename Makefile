CC = gcc
CFLAGS = -Wall -g

# Add the new .o files to the list
OBJS = main.o prompt.o utils.o builtins.o history.o signals.o execute.o

all: myshell

myshell: $(OBJS)
	$(CC) $(CFLAGS) -o myshell $(OBJS)

# Add dependencies for new object files
builtins.o: builtins.c builtins.h history.h
history.o: history.c history.h
signals.o: signals.c signals.h
execute.o: execute.c execute.h builtins.h utils.h

# Existing rules will still work for these
main.o: main.c prompt.h history.h signals.h execute.h utils.h
prompt.o: prompt.c prompt.h utils.h
utils.o: utils.c utils.h

clean:
	rm -f *.o myshell
