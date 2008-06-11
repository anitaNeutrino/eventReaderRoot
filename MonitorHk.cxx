//////////////////////////////////////////////////////////////////////////////
/////  MonitorHk.cxx        Monitor Hk                                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Gpsd Start structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "MonitorHk.h"
#include <iostream>
#include <fstream>

ClassImp(MonitorHk);

MonitorHk::MonitorHk() 
{
   //Default Constructor
}

MonitorHk::~MonitorHk() {
   //Default Destructor
}


MonitorHk::MonitorHk(Int_t trun, Int_t trealTime, MonitorStruct_t *monPtr)
{
  run=trun;
  realTime=trealTime;
  payloadTime=monPtr->unixTime;
  memcpy(diskSpace,monPtr->diskInfo.diskSpace,sizeof(UShort_t)*8);
  memcpy(sataminiLabel,monPtr->diskInfo.sataminiLabel,sizeof(Char_t)*12);
  memcpy(satabladeLabel,monPtr->diskInfo.satabladeLabel,sizeof(Char_t)*12);
  memcpy(usbLabel,monPtr->diskInfo.usbLabel,sizeof(Char_t)*12);
  memcpy(eventLinks,monPtr->queueInfo.eventLinks,sizeof(UShort_t)*NUM_PRIORITIES);
  memcpy(hkLinks,monPtr->queueInfo.hkLinks,sizeof(UShort_t)*12);
  memcpy(utime,monPtr->procInfo.utime,sizeof(UInt_t)*NUM_PROCESSES);
  memcpy(stime,monPtr->procInfo.stime,sizeof(UInt_t)*NUM_PROCESSES);
  memcpy(vsize,monPtr->procInfo.vsize,sizeof(UInt_t)*NUM_PROCESSES);
}

