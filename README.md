# RRT
> Simple OCM based RRT implementation in C

This repository holds a C implementation of Rapidly-Exploring Random Tree, built as the function of interest in my [Senior Thesis](https://github.com/AnthonyKenny98/Thesis). It has a 3D and 2D branch

![RRT Graph](doc/RRTGraph.png)


## Table of Contents
+ [Branches](#branches)
+ [License](#license)

## <a name=branches></a>Branches
This repository has the following current branches:
+ [Master](https://github.com/AnthonyKenny98/RRT) is a 3D implementation of RRT with internally coded performance metrics and no reliance on external profiling tools.
+ [3D-x86](https://github.com/AnthonyKenny98/RRT/tree/3D-x86) is a 3D implementation of RRT but relies on the "analysis" directory and Intel's VTune tool for execution profiling. The limitation of this is that profiling can only occur on x86 machines, while the focus of the Thesis is the RISC-V ISA. Hence why internal profiling was added to the Master branch.
+ [2D](https://github.com/AnthonyKenny98/RRT/tree/2D) is a 2D implementation of RRT.

## <a name=license></a>License

