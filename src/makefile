CC=gcc
# CC=riscv64-unknown-elf-gcc
CFLAGS=-I. 

rrt: rrt.o tools.o ogm.o
	$(CC) -o rrt rrt.o tools.o ogm.o -g -lm


rrt.o: rrt.c tools.h
	$(CC) -c rrt.c -g -lm

tools.o: tools.c tools.h
	$(CC) -c tools.c -g -lm

ogm.o: ogm.c ogm.h
	$(CC) -c ogm.c -g -lm


clean:
	rm rrt *.o
