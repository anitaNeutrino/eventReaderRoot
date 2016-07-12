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
#include <cstring>

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


 if(startPtr->gHdr.code!=PACKET_RUN_START ||
     startPtr->gHdr.verId!=VER_RUN_START ||
     startPtr->gHdr.numBytes!=sizeof(RunStart_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << startPtr->gHdr.code << "\t" << PACKET_RUN_START 
	      << "\nversion:\t" << startPtr->gHdr.verId 
	      << "\t" << VER_RUN_START 
	      << "\nsize:\t" << startPtr->gHdr.numBytes << "\t"
	      << sizeof(RunStart_t) << std::endl;
  }
  run=trun;
  realTime=trealTime;
  payloadTime=startPtr->unixTime;
  runReported=startPtr->runNumber;
  eventNumber=startPtr->eventNumber;
}

