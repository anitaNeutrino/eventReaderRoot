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
#include <cstring>

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

 if(otherPtr->gHdr.code!=PACKET_OTHER_MONITOR ||
     otherPtr->gHdr.verId!=VER_OTHER_MON ||
     otherPtr->gHdr.numBytes!=sizeof(OtherMonitorStruct_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << otherPtr->gHdr.code << "\t" << PACKET_OTHER_MONITOR 
	      << "\nversion:\t" << otherPtr->gHdr.verId 
	      << "\t" << VER_OTHER_MON 
	      << "\nsize:\t" << otherPtr->gHdr.numBytes << "\t"
	      << sizeof(OtherMonitorStruct_t) << std::endl;
  }
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


const char *OtherMonitorHk::getDirName(int dirInd)
{
  const char *dirNames[3]={"acqd","eventd","prioritizerd"};
  if(dirInd<0 || dirInd>2)
    return "unknown";
  return dirNames[dirInd];
 
}

const char *OtherMonitorHk::getProcName(int procInd)
{
  
  const char *procName[16]=
    {"Acqd","Archived","Calibd","Cmdd","Eventd","GPSd","Hkd","LOSd"
     "Prioritizerd","SIPd","Monitord","Playbackd","Logwatchd","Neobrickd",
     "n/a","n/a"};
  if(procInd<0 || procInd>15)
    return "No Proc";
  return procName[procInd];
}

int OtherMonitorHk::isInProcessMask(int procInd) 
{
  
  if(procInd<0 || procInd>15)
    return 0;

 return (processBitMask&(1<<procInd) ? 1 : 0) ;
  
}
