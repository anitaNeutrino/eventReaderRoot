#!/bin/csh
# The purpose of this script is to set-up (somewhat) necessary environmental
# variables to allow easy use of the event reader utilities.
# People should edit this script and source it before using the
# event reader. 


## ROOT
## You can comment this out if already set ROOT up in a different location
## (eg. login script)
setenv ROOTSYS /unix/anita/software/root
setenv PATH ${ROOTSYS}/bin:${PATH}
setenv LD_LIBRARY_PATH ${ROOTSYS}/lib:${LD_LIBRARY_PATH}

##Uncomment the below line if you are bored of the "naked lady"
#alias root "root -l"

##Event Reader Stuff
setenv EVENT_READER_DIR /home/rjn/anita/validation/eventReaderRoot/trunk
setenv ANITA_CALIB_DIR ${EVENT_READER_DIR}/calib
setenv LD_LIBRARY_PATH ${EVENT_READER_DIR}:${LD_LIBRARY_PATH}
