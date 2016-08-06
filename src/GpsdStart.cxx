//////////////////////////////////////////////////////////////////////////////
/////  GpsdStart.h        Gpsd Start                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Gpsd Start structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "GpsdStart.h"
#include "AnitaPacketUtil.h"
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
  simplePacketCheck(&(startPtr->gHdr),PACKET_GPSD_START);
  run=trun;
  realTime=trealTime;
  payloadTime=startPtr->unixTime;
  memcpy(ackCount,startPtr->ackCount,sizeof(UChar_t)*3);
  memcpy(nakCount,startPtr->nakCount,sizeof(UChar_t)*3);
  rioBitMask=startPtr->rioBitMask;
  tstBitMask=startPtr->tstBitMask;
}

