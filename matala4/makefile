CC = gcc
AR = ar
FLAGS = -Wall -g

all: my_mutex my_fcntl
# run: main
# 	./main
my_mutex: my_mutex.o 
	$(CC) -pthread -o my_mutex my_mutex.o

my_mutex.o: my_mutex.c
	$(CC) $(FLAGS) -c my_mutex.c 

my_fcntl: my_fcntl.o
	$(CC) -pthread -o my_fcntl my_fcntl.o

my_fcntl.o: my_fcntl.c
	$(CC) $(FLAGS) -c my_fcntl.c 

clean:
	rm -f *.o *.so my_mutex my_fcntl