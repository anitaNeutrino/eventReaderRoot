//////////////////////////////////////////////////////////////////////////////
/////  RunStart.h        Run Start                           /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Run Start structs in a tree   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RunStart.h"
#include <iostream>
#include <fstream>

ClassImp(RunStart);

RunStart::RunStart() 
{
   //Default Constructor
}

RunStart::~RunStart() {
   //Default Destructor
}


RunStart::RunStart(Int_t trun, Int_t trealTime, RunStart_t *startPtr)
{
  run=trun;
  realTime=trealTime;
  payloadTime=startPtr->unixTime;
  runReported=startPtr->runNumber;
  eventNumber=startPtr->eventNumber;
}

