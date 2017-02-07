# A simple shell script to pull a run from anita-aware, then produce, save and plot power spectra produced from a root session.
echo Enter the run number:
read runNumber
echo Fetching run $runNumber...

awareIP="157.132.95.239" # IP here, subject to change

scp anita@$awareIP:/data/flight1617/telem/root/run$runNumber/gpuFile$runNumber.root . # Secure copy from anita-aware to current dir (/ change to your dir)

root -b -l <<EOF # Start a root session
.L quickPlotSpectra.C
quickPlotSpectra("gpuFile$runNumber.root",false,20)
EOF
# Save file as gpuFile gpuFile$runNumber.root

# The part underneath is optional and just displays the plot in a portable manner (both bash and zsh, different OSs)
osname=`uname -s`
case $osname in
    "Linux") # Linux
	     xdg-open gpuFile$runNumber.root_Spec.png
	     ;; 
    "Darwin"*) # Mac
	     open gpuFile$runNumber.root_Spec.png
	     ;;
esac
