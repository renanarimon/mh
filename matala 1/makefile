CC = gcc
AR = ar
FLAGS = -Wall -g

all: cmp copy encode decode codec1.so codec2.so
# encode decode codec1 codec2 codec
cmp: cmp.o
	$(CC) -o cmp cmp.o

cmp.o: cmp.c
	$(CC) $(FLAGS) -c cmp.c 

copy: copy.o
	$(CC) -o copy copy.o

copy.o: copy.c
	$(CC) $(FLAGS) -c copy.c 

encode: encode.o
	$(CC) -o encode encode.o -ldl

encode.o: encode.c
	$(CC) $(FLAGS) -c encode.c -ldl

decode: decode.o
	$(CC) -o decode decode.o -ldl

decode.o: decode.c
	$(CC) $(FLAGS) -c decode.c -ldl

codec1.so: codec.h codec1.o
	$(CC) -shared -o codec1.so codec1.o -ldl

codec2.so: codec.h codec2.o
	$(CC) -shared -o codec2.so codec2.o -ldl

# codec2: codec.h codec2.o
# 	gcc -g -o test codec2.c

# codec2.o: codec2.c
# 	$(CC) $(FLAGS) -c codec2.c 


.PHONY: clean all

clean:
	rm -f *.o *.so cmp copy encode decode