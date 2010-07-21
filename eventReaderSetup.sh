#!/bin/bash
# The purpose of this script is to set-up (somewhat) necessary environmental
# variables to allow easy use of the event reader utilities.
# People should edit this script and source it before using the
# event reader. 


## ROOT
## You can comment this out if already set ROOT up in a different location
## (eg. login script)
#export ROOTSYS=/unix/anita/software/root
#export PATH=${ROOTSYS}/bin:${PATH}
#export LD_LIBRARY_PATH=${ROOTSYS}/lib:${LD_LIBRARY_PATH}

##Uncomment the below line if you are bored of the "naked lady"
#alias root="root -l"
#for the MakeAskaryan program
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}

##Event Reader Stuff
export EVENT_READER_DIR=/home/mottram/work/eventReaderRoot/trunk
#export ANITA_CALIB_DIR=${EVENT_READER_DIR}/calib
LD_LIBRARY_PATH=${EVENT_READER_DIR}:${LD_LIBRARY_PATH}

##fftw stuff
#export FFTW_DIR=/home/mottram/work/libRootFftwWrapper/trunk
#export LD_LIBRARY_PATH=${FFTW_DIR}:${LD_LIBRARY_PATH}

#Aesop stuff
export EVENT_SIMULATION_DIR=/home/mottram/work/Aesop/trunk 
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$EVENT_SIMULATION_DIR

#Flight soft stuff
#export ANITA_FLIGHT_SOFT_DIR=/home/mottram/work/flightSoft/trunk
#export LD_LIBRARY_PATH=${ANITA_FLIGHT_SOFT_DIR}:${LD_LIBRARY_PATH}
#export ANITA_FLIGHT_SOFT_LIBS=/home/mottram/work/flightSoft/trunk/lib
#LD_LIBRARY_PATH=${ANITA_FLIGHT_SOFT_LIBS}:${LD_LIBRARY_PATH}
#source /home/mottram/work/flightSoft/trunk/script/anitaFlightSoftSetup.sh

#magicDisplay stuff
export ANITA_UTIL_INSTALL_DIR=/home/mottram/work/install
LD_LIBRARY_PATH=/unix/anita1/software/install/lib:${ANITA_UTIL_INSTALL_DIR}/lib:${LD_LIBRARY_PATH}

#for the bedmap stuff
export BEDMAP_DATA_DIR=/home/mottram/work/eventCorrelator/data
#for the antenna stuff
export ANTENNA_DATA_DIR=/home/mottram/work/relocationWork/data
#for the flight root data
export FLIGHT_ROOT_DATA_DIR=/home/mottram/work/relocationWork/data

#export LIB64_DIR=/usr/lib64/
#export LD_LIBRARY_PATH=${LIB64_DIR}:${LD_LIBRARY_PATH}
#export USRLIB_DIR=/usr/lib/
#export LD_LIBRARY_PATH=${USRLIB_DIR}:${LD_LIBRARY_PATH}

#rawWebPlotter stuff
#export WEBPLOTTER_DIR=/home/mottram/work/rawWebPlotter/trunk

