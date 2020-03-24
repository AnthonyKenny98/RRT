# Change directory back to projects working directory
echo "Changing back to project root dir"
cd $1
echo

# Make RRT Project
echo "Making RRT Project"
make
echo

# Run VTune Hotspots Analysis
echo "Running RRT"
cd src
./rrt3D
cd $1

# Clean Up
echo "Cleaning Up"
make clean