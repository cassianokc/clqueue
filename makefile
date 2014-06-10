CC=gcc
CCFLAGS=-std=c99 -march=native -mtune=native -Wall -pedantic 
CLFLAGS=


all: main.o

main.o: main.c common.h
	$(CC) $(CCFLAGS) $< -c

squeue.o: squeue.c squeue.h common.h
	$(CC) $(CCFLAGS) $< -c

ut: squeue.o
	-@$(CC) $(CCFLAGS) $< unit_test.c -o test
	./test
	@rm test


#Removes all tildes ending files, objects codes and test executable
clean:
	rm -rf *~ *.o .*.swp utest
