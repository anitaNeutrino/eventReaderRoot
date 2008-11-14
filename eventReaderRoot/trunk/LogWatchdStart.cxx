//////////////////////////////////////////////////////////////////////////////
/////  LogWatchdStart.h        LogWatchd Start                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing LogWatchd Start structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "LogWatchdStart.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(LogWatchdStart);

LogWatchdStart::LogWatchdStart() 
{
   //Default Constructor
}

LogWatchdStart::~LogWatchdStart() {
   //Default Destructor
}


LogWatchdStart::LogWatchdStart(Int_t trun, Int_t trealTime, LogWatchdStart_t *startPtr)
{

 if(startPtr->gHdr.code!=PACKET_LOGWATCHD_START ||
     startPtr->gHdr.verId!=VER_LOGWATCHD_START ||
     startPtr->gHdr.numBytes!=sizeof(LogWatchdStart_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << startPtr->gHdr.code << "\t" << PACKET_LOGWATCHD_START 
	      << "\nversion:\t" << startPtr->gHdr.verId 
	      << "\t" << VER_LOGWATCHD_START 
	      << "\nsize:\t" << startPtr->gHdr.numBytes << "\t"
	      << sizeof(LogWatchdStart_t) << std::endl;
  }

  run=trun;
  realTime=trealTime;
  payloadTime=startPtr->unixTime;
  runReported=startPtr->runNumber;
  upTime=startPtr->upTime;
  idleTime=startPtr->idleTime;
}

