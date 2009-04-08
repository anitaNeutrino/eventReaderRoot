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

##Event Reader Stuff
export EVENT_READER_DIR=/home/rjn/anita/eventReaderRoot/branches/R3.0.2
export ANITA_CALIB_DIR=${EVENT_READER_DIR}/calib
export LD_LIBRARY_PATH=${EVENT_READER_DIR}:${LD_LIBRARY_PATH}