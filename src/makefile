CC=gcc
CFLAGS=-I. 

rrt: rrt.o tools.o ogm.o
	$(CC) -o rrt rrt.o tools.o ogm.o -g -lm


rrt.o: rrt.c tools.h
	gcc -c rrt.c -g -lm

tools.o: tools.c tools.h
	gcc -c tools.c -g -lm

ogm.o: ogm.c ogm.h
	gcc -c ogm.c -g -lm


clean:
	rm rrt *.o *.csv
