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
#include <cstring>

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

 if(monPtr->gHdr.code!=PACKET_MONITOR ||
     monPtr->gHdr.verId!=VER_MONITOR ||
     monPtr->gHdr.numBytes!=sizeof(MonitorStruct_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << monPtr->gHdr.code << "\t" << PACKET_MONITOR 
	      << "\nversion:\t" << monPtr->gHdr.verId 
	      << "\t" << VER_MONITOR 
	      << "\nsize:\t" << monPtr->gHdr.numBytes << "\t"
	      << sizeof(MonitorStruct_t) << std::endl;
  }

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



char *MonitorHk::getDriveName(int driveInd) 
{
  char *diskNames[NUM_DISK_SPACES]={"Ramdisk","Var","Home","PMC",satabladeLabel,sataminiLabel,"Home","Neobrick"};
  if(driveInd<0 || driveInd>7)
    return "Invalid";
  return diskNames[driveInd];
}

int MonitorHk::getDriveCapacity(int driveInd) 
{
  int diskMax[NUM_DISK_SPACES]={2000,8000,2000,9000,125000,125000,32000,1000000};
  if(driveInd<0 || driveInd>7)
    return -1;
  return diskMax[driveInd];
}

int MonitorHk::getDiskSpaceAvailable(int driveInd)
{

 int multiplier[NUM_DISK_SPACES]={1,1,1,1,2,2,1,16};
 if(driveInd<0 || driveInd>7)
   return -1;
 return (diskSpace[driveInd]*multiplier[driveInd]);
}

char *MonitorHk::getHkQueueName(int hkInd)
{
  char *telemNames[21]=
    {"LosCmd","SipCmd","Monitor","Header","Acromag",
     "Adu5aSat","Adu5bSat","G12Sat","Adu5aPat","Adu5bPat","G12Pos",
     "Adu5aVtg","Adu5bVtg","G12Gga","Adu5aGga","Adu5bGga","SurfHk",
     "TurfHk","Other","Pedestal","Request"};
  if(hkInd<0 || hkInd>20)
    return "No Queue";
  return telemNames[hkInd];
}

char *MonitorHk::getProcName(int procInd)
{
  
  char *procName[16]=
    {"Acqd","Archived","Calibd","Cmdd","Eventd","GPSd","Hkd","LOSd"
     "Prioritizerd","SIPd","Monitord","Playbackd","Logwatchd","Neobrickd",
     "n/a","n/a"};
  if(procInd<0 || procInd>15)
    return "No Proc";
  return procName[procInd];
}
