CC = gcc
CFLAGS = -Wall -g

OBJS = main.o prompt.o utils.o

all: prompt_test

prompt_test: $(OBJS)
	$(CC) $(CFLAGS) -o prompt_test $(OBJS)

clean:
	rm -f *.o prompt_test
