# Must execute the following to compile for riscv:
# export PATH=$PATH:$HOME/Documents/rocket-chip/rocket-tools/bin
# CC=riscv64-unknown-elf-gcc


CC=gcc
3D=-D _3D=1
2D=-D _3D=0
OBJECT=-D _OBJECT=1

rrt:
	$(CC) $(3D) $(OBJECT) -o rrt3D rrt.c
	$(CC) $(2D) $(OBJECT) -o rrt2D rrt.c

clean:
	rm rrt2D rrt3D
