# Source amplxe-cl tool
echo "Sourcing amplxe-cl"
cd ~/intel/vtune_amplifier
source amplxe-vars.sh
echo

# Change directory back to projects working directory
echo "Changing back to project root dir"
cd $1
echo

# Make RRT Project
echo "Making RRT Project"
make
echo

# Run VTune Hotspots Analysis
echo "Running Vtune Hotspots Analysis"
amplxe-cl -collect hotspots -result-dir=$2 -finalization-mode=full $1/src/rrt

# Clean Up
echo "Cleaning Up"
make clean
echo