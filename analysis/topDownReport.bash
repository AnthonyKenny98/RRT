# Source amplxe-cl tool
echo "Sourcing amplxe-cl"
cd ~/intel/vtune_amplifier
source amplxe-vars.sh
echo

# Change directory back to projects working directory
echo "Changing back to project root dir"
cd $1
echo

# Run VTune TopDown Report
echo "Running Vtune TopDown Report"
amplxe-cl -report top-down -result-dir=$2 -report-output $3 -format csv -csv-delimiter comma