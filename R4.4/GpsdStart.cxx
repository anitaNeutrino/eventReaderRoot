//////////////////////////////////////////////////////////////////////////////
/////  GpsdStart.h        Gpsd Start                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Gpsd Start structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "GpsdStart.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(GpsdStart);

GpsdStart::GpsdStart() 
{
   //Default Constructor
}

GpsdStart::~GpsdStart() {
   //Default Destructor
}


GpsdStart::GpsdStart(Int_t trun, Int_t trealTime, GpsdStartStruct_t *startPtr)
{

 if(startPtr->gHdr.code!=PACKET_GPSD_START ||
     startPtr->gHdr.verId!=VER_GPSD_START ||
     startPtr->gHdr.numBytes!=sizeof(GpsdStartStruct_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << startPtr->gHdr.code << "\t" << PACKET_GPSD_START 
	      << "\nversion:\t" << startPtr->gHdr.verId 
	      << "\t" << VER_GPSD_START 
	      << "\nsize:\t" << startPtr->gHdr.numBytes << "\t"
	      << sizeof(GpsdStartStruct_t) << std::endl;
  }
  run=trun;
  realTime=trealTime;
  payloadTime=startPtr->unixTime;
  memcpy(ackCount,startPtr->ackCount,sizeof(UChar_t)*3);
  memcpy(nakCount,startPtr->nakCount,sizeof(UChar_t)*3);
  rioBitMask=startPtr->rioBitMask;
  tstBitMask=startPtr->tstBitMask;
}

