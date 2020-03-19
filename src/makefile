# Must execute the following to compile for riscv:
# export PATH=$PATH:$HOME/Documents/rocket-chip/rocket-tools/bin
# CC=riscv64-unknown-elf-gcc

3D=1

CC=gcc
CFLAGS=-D _3D=$(3D)



rrt: rrt.o tools.o ogm.o
	$(CC) $(CFLAGS) -o rrt rrt.o tools.o ogm.o -g -lm


rrt.o: rrt.c tools.h
	$(CC) $(CFLAGS) -c rrt.c -g -lm

tools.o: tools.c tools.h
	$(CC) $(CFLAGS) -c tools.c -g -lm

ogm.o: ogm.c ogm.h
	$(CC) $(CFLAGS) -c ogm.c -g -lm

clean:
	rm rrt *.o
