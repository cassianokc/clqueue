CC=gcc
CCFLAGS=-std=c99 -march=native -mtune=native -Wall -pedantic -framework OpenCL
CLFLAGS=



#Removes all tildes ending files, objects codes and test executable
clean:
	rm -rf *~ *.o .*.swp utest
