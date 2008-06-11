//////////////////////////////////////////////////////////////////////////////
/////  OtherMonitorHk.cxx        Monitor Hk                                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Gpsd Start structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "OtherMonitorHk.h"
#include <iostream>
#include <fstream>

ClassImp(OtherMonitorHk);

OtherMonitorHk::OtherMonitorHk() 
{
   //Default Constructor
}

OtherMonitorHk::~OtherMonitorHk() {
   //Default Destructor
}


OtherMonitorHk::OtherMonitorHk(Int_t trun, Int_t trealTime, OtherMonitorStruct_t *otherPtr)
{
  run=trun;
  realTime=trealTime;
  payloadTime=otherPtr->unixTime;
  ramDiskInodes=otherPtr->ramDiskInodes;
  runStartTime=otherPtr->runStartTime;
  runStartEventNumber=otherPtr->runStartEventNumber;
  runNumber=otherPtr->runNumber;
  memcpy(dirFiles,otherPtr->dirFiles,sizeof(UShort_t)*3);
  memcpy(dirLinks,otherPtr->dirLinks,sizeof(UShort_t)*3);
  processBitMask=otherPtr->processBitMask;
  reserved=otherPtr->reserved;
 
}

