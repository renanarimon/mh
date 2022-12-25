CC = gcc
AR = ar
FLAGS = -Wall -g

all: my_mutex my_fcntl my_cond my_packet
run: my_mutex my_fcntl my_cond my_packet
	./my_mutex 
	./my_fcntl 
	./my_cond 
	./my_packet
my_mutex: my_mutex.o 
	$(CC) -pthread -o my_mutex my_mutex.o

my_mutex.o: my_mutex.c
	$(CC) $(FLAGS) -c my_mutex.c 

my_fcntl: my_fcntl.o
	$(CC) -pthread -o my_fcntl my_fcntl.o

my_fcntl.o: my_fcntl.c
	$(CC) $(FLAGS) -c my_fcntl.c 

my_cond: my_cond.o
	$(CC) -pthread -o my_cond my_cond.o

my_cond.o: my_cond.c
	$(CC) $(FLAGS) -c my_cond.c 

my_packet: my_packet.o
	$(CC) -pthread -o my_packet my_packet.o

my_packet.o: my_packet.c
	$(CC) $(FLAGS) -c my_packet.c 

clean:
	rm -f *.o *.so my_mutex my_fcntl my_cond my_packet test.txt