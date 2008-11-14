//////////////////////////////////////////////////////////////////////////////
/////  AcqdStart.h        Acqd Start                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Acqd Start structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AcqdStart.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(AcqdStart);

AcqdStart::AcqdStart() 
{
   //Default Constructor
}

AcqdStart::~AcqdStart() {
   //Default Destructor
}


AcqdStart::AcqdStart(Int_t trun, Int_t trealTime, AcqdStartStruct_t *startPtr)
{
  if(startPtr->gHdr.code!=PACKET_ACQD_START ||
     startPtr->gHdr.verId!=VER_ACQD_START ||
     startPtr->gHdr.numBytes!=sizeof(AcqdStartStruct_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << startPtr->gHdr.code << "\t" << PACKET_ACQD_START 
	      << "\nversion:\t" << startPtr->gHdr.verId 
	      << "\t" << VER_ACQD_START 
	      << "\nsize:\t" << startPtr->gHdr.numBytes << "\t"
	      << sizeof(AcqdStartStruct_t) << std::endl;
  }
  run=trun;
  realTime=trealTime;
  payloadTime=startPtr->unixTime;
  numEvents=startPtr->numEvents;
  memcpy(testBytes,startPtr->testBytes,sizeof(UChar_t)*8);
  memcpy(chanMean,startPtr->chanMean,sizeof(Float_t)*ACTIVE_SURFS*CHANNELS_PER_SURF);
  memcpy(chanRMS,startPtr->chanRMS,sizeof(Float_t)*ACTIVE_SURFS*CHANNELS_PER_SURF);
  memcpy(threshVals,startPtr->threshVals,sizeof(UShort_t)*10);
  memcpy(scalerVals,startPtr->scalerVals,sizeof(UShort_t)*10*ACTIVE_SURFS*SCALERS_PER_SURF);
}

