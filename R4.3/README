##############################################################################
###### Event Reader --- An ANITA Event Reader in ROOT format            ######
######                                                                  ######
###### Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                          ######
##############################################################################

Website
========
http://www.hep.ucl.ac.uk/uhen/anita/eventReader/

Introduction
============
This is version 4 of the ANITA Event Reader for ROOT Data and Raw Data, and is the first designed for ANITA II data. At present backwards compatibility is broken this will hoepfully be restored at some point once I work out how to do it. It is meant to be a simple class that will read in the raw event data (in ROOT file or raw psev_ format ) and convert it into UsefulAnitaEvent calibrated objects (the raw data reader actually creates text files of times and voltages).

Prerequisites
=============
ROOT

Optional Extras (Necessary for clock calibration
=============== 
FFTW 3 -- Fastest Fourier Transform in the West
libRootFftwWrapper -- a ROOT wrapper for FFTW 3

Installation
============
Checkout the code from the SVN repository, eg.: 
svn co https://delos.mps.ohio-state.edu/anitaGround/eventReaderRoot/trunk myEventReaderDir
Define the ANITA_UTIL_INSTALL_DIR to point to the location you want the library installed (the library files will end up in (ANITA_UTIL_INSTALL_DIR)/lib and the header files in (ANITA_UTIL_INSTALL_DIR)/include).
Do
make
make install

Manual
========
If you are averse to reading web pages (and who wouldn't be) you can download a pdf copy of the reference manual from the website, but be warned it won't be a thrilling read as it was written by a computer program.

Troubleshooting
================
There are a myriad of ways that one can run into problems with the event reader, the most common of which are listed here
Path problems -- the bane of poorly organised code. By far the easiest way to use the ANITA offline code is to just set the ANITA_UTIL_INSTALL_DIR and have it point to the location you want to install all the packages. If things are set up correctly you will end up with a ANITA_UTIL_INSTALL_DIR/include and ANITA_UTIL_INSTALL_DIR/lib and ANITA_UTIL_INSTALL_DIR/share/anitaCalib all populated with essential headers, libraries and calibration constants. A quick round of
make clean all install
in libRootFftwWrapper and eventReader (and magicDisplay, etc.) can solve most such difficulties.
Calibration data -- AnitaEventCalibrator looks for calibration data in the following order (make sure it finds some).
ANITA_CALIB_DIR
ANITA_UTIL_INSTALL_DIR/share/anitaCalib
./calib


Notes
======
As of version 2.0 the implementation of the data interface is frozen. If people have comments, complaints or requests they shouldn't hesitate to contact me. 

In AnitaGeomTool and therefore in the rest of the code it should be noted that a 'C' style numbering convention is used. Therefore antennas are numbered 0-31 and phi sectors numbered 0-15. 

As of version 3.2.0 AnitaGeomTool has an interface to Kurt's photogrammetry data.In particular through the following family of functions:
a)  Double_t getAntZ(int ant);    // along with x,y,z,r and phi	
b)  Double_t getAntFaceZ(int ant) // along with x,y,z,r and phi	
The positions of the centre of the antenna face (the numbers from Kurt) are given by the b) family of functions. The a) family of functions return a position 20cm behind the face of the antenna, this position is meant to approximate teh timing centre of the antenna. At the moment these values are from the ANITA-I flighgt.

