CC = gcc
AR = ar
FLAGS = -Wall -g

all: mytee shell



mytee: mytee.o
	$(CC) -o mytee mytee.o

mytee.o: mytee.c
	$(CC) $(FLAGS) -c mytee.c 

mync: mync.o
	$(CC) -o mync mync.o

mync.o: mync.c
	$(CC) $(FLAGS) -c mync.c 

shell: shell.o
	$(CC) -o shell shell.o

shell.o: shell.c
	$(CC) $(FLAGS) -c shell.c 

clean:
	rm -f *.o *.so *.txt mytee shell mync