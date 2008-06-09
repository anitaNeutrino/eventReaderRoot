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
  run=trun;
  realTime=trealTime;
  payloadTime=startPtr->unixTime;
  runReported=startPtr->runNumber;
  upTime=startPtr->upTime;
  idleTime=startPtr->idleTime;
}

