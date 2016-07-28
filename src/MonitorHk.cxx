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
   std::cerr << "Mismatched packet\t" << packetCodeAsString(PACKET_MONITOR) << "\t" 
	     << "code:\t" << (int)monPtr->gHdr.code << "\t" << (int)PACKET_MONITOR 
	     << "\nversion:\t" << (int)monPtr->gHdr.verId 
	     << "\t" << (int)VER_MONITOR 
	     << "\nsize:\t" << (int)monPtr->gHdr.numBytes << "\t"
	     << sizeof(MonitorStruct_t) << std::endl;
  }

  run=trun;
  realTime=trealTime;
  payloadTime=monPtr->unixTime;
  memcpy(diskSpace,monPtr->diskInfo.diskSpace,sizeof(UShort_t)*8);
  memcpy(ntuLabel,monPtr->diskInfo.ntuLabel,sizeof(Char_t)*12);
  memcpy(otherLabel,monPtr->diskInfo.otherLabel,sizeof(Char_t)*12);
  memcpy(usbLabel,monPtr->diskInfo.usbLabel,sizeof(Char_t)*12);
  memcpy(eventLinks,monPtr->queueInfo.eventLinks,sizeof(UShort_t)*NUM_PRIORITIES);
  memcpy(hkLinks,monPtr->queueInfo.hkLinks,sizeof(UShort_t)*21);
  memcpy(utime,monPtr->procInfo.utime,sizeof(UInt_t)*NUM_PROCESSES);
  memcpy(stime,monPtr->procInfo.stime,sizeof(UInt_t)*NUM_PROCESSES);
  memcpy(vsize,monPtr->procInfo.vsize,sizeof(UInt_t)*NUM_PROCESSES);
}



const char *MonitorHk::getDriveName(int driveInd) 
{
  const char *diskNames[NUM_DISK_SPACES]={"Ramdisk","Var","Home","Root","Helium1","Helium2",usbLabel,ntuLabel};
  if(driveInd<0 || driveInd>7)
    return "Invalid";
  return diskNames[driveInd];
}

int MonitorHk::getDriveCapacity(int driveInd) 
{
  int diskMax[NUM_DISK_SPACES]={4000,3000,4000,8000,6000000,6000000,256000,1000000};
  if(driveInd<0 || driveInd>7)
    return -1;
  return diskMax[driveInd];
}


float MonitorHk::getDiskSpacePercentage(int driveInd) {
  if(driveInd<0 || driveInd>7)
    return -1;
  return 100.*float(getDiskSpaceAvailable(driveInd))/getDriveCapacity(driveInd);
}

int MonitorHk::getDiskSpaceAvailable(int driveInd)
{

  int multiplier[NUM_DISK_SPACES]={1,1,1,1,128,128,4,16};
 if(driveInd<0 || driveInd>7)
   return -1;
 return (diskSpace[driveInd]*multiplier[driveInd]);
}

const char *MonitorHk::getHkQueueName(int hkInd)
{
  const char *telemNames[21]=
    {"LosCmd","SipCmd","Monitor","Header","Acromag",
     "Adu5aSat","Adu5bSat","G12Sat","Adu5aPat","Adu5bPat","G12Pos",
     "Adu5aVtg","Adu5bVtg","G12Gga","Adu5aGga","Adu5bGga","SurfHk",
     "TurfHk","Other","Pedestal","Request"};
  if(hkInd<0 || hkInd>20)
    return "No Queue";
  return telemNames[hkInd];
}

const char *MonitorHk::getProcName(int procInd)
{
  
  const char *procName[16]=
     {"Acqd","Archived","Calibd","Cmdd","Eventd","GPSd","Hkd","LOSd",
     "Prioritizerd","SIPd","Monitord","Playbackd","Logwatchd","Neobrickd",
     "n/a","n/a"};
  if(procInd<0 || procInd>15)
    return "No Proc";
  return procName[procInd];
}
