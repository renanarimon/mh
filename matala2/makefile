CC = gcc
AR = ar
FLAGS = -Wall -g

all: mytee shell

mytee: mytee.o
	$(CC) -o mytee mytee.o

mytee.o: mytee.c
	$(CC) $(FLAGS) -c mytee.c 

shell: shell.o
	$(CC) -o shell shell.o

shell.o: shell.c
	$(CC) $(FLAGS) -c shell.c 

main: main.o
	$(CC) -o main main.o

main.o: main.c
	$(CC) $(FLAGS) -c main.c 

clean:
	rm -f *.o *.so *.txt mytee shell