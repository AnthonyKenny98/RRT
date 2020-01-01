CC=gcc
CFLAGS=-I. 

rrt: rrt.o tools.o naive.o ogm.o
	$(CC) -o rrt rrt.o tools.o naive.o ogm.o -g -lm


rrt.o: rrt.c tools.h naive.h
# Naive Implementation
# 	gcc -c rrt.c -g -lm -DNAIVE
# Not Naive
	gcc -c rrt.c -g -lm

tools.o: tools.c tools.h
	gcc -c tools.c -g -lm

naive.o: naive.c naive.h
	gcc -c naive.c -g -lm

ogm.o: ogm.c ogm.h
	gcc -c ogm.c -g -lm


clean:
	rm -r rrt *.o
