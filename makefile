CC=gcc
CCFLAGS=-std=c99 -march=native -mtune=native -Wall -pedantic -g
CLFLAGS=-framework OpenCL

all: main.o cltask.o
	$(CC) $(CCFLAGS) $^ $(CLFLAGS) -o main

cltask.o: cltask.c cltask.h cltask.cl common.h
	$(CC) $(CCFLAGS) $< -c -o $@

main.o: main.c main.cl common.h
	$(CC) $(CCFLAGS) $< -c -o $@




#Removes all tildes ending files, objects codes and test executable
clean:
	rm -rf *~ *.o .*.swp utest main
