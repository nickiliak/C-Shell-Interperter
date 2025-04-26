CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

all: shell.c  H_functions.c 
	$(CC) $(CFLAGS) $^ -o shell

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f shell *.o


