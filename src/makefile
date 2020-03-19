# Must execute the following to compile for riscv:
# export PATH=$PATH:$HOME/Documents/rocket-chip/rocket-tools/bin
# CC=riscv64-unknown-elf-gcc

3D=1

CC=gcc
CFLAGS=-D _3D=$(3D)

rrt:
	$(CC) $(CFLAGS) -o rrt rrt.c

clean:
	rm rrt *.o
