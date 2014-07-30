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

  memcpy(turfIdBytes,startPtr->turfIdBytes,sizeof(UChar_t)*4);
  memcpy(turfioIdBytes,startPtr->turfioIdBytes,sizeof(UChar_t)*4);
  turfIdVersion=startPtr->turfIdVersion;
  turfioIdVersion=startPtr->turfioIdVersion;

  printTurfioVersion();
  printTurfVersion();

}


void AcqdStart::printTurfVersion() {


  printf("TURF: %c %c %c %c\n",turfIdBytes[0],turfIdBytes[1],turfIdBytes[2],turfIdBytes[3]);
  printf("Version: %x\n",turfIdVersion);
  printf("Version: %d %x %d/%d\n",turfIdVersion&0xFF,(turfIdVersion&0xFF00)>>8,(turfIdVersion&0xFF0000)>>16,(turfIdVersion&0xFF000000)>>24);


}



void AcqdStart::printTurfioVersion() {


  printf("TURFIO: %c %c %c %c\n",turfioIdBytes[0],turfioIdBytes[1],turfioIdBytes[2],turfioIdBytes[3]);
  printf("Version: %#x\n",turfioIdVersion);
  printf("Version: %x %x %x %x\n",turfioIdVersion&0xFF,(turfioIdVersion&0xFF00)>>8,(turfioIdVersion&0xFF0000)>>16,(turfioIdVersion&0xFF000000)>>24);
  int boardRevision=turfioIdVersion>>28;
  int month=((turfioIdVersion>>24)&0xf);
  int day=((turfioIdVersion)&0x00ff0000)>>16;
  int majorRev=((turfioIdVersion>>12)&0xf);
  int minorRev=((turfioIdVersion>>8)&0xf);
  int rev=turfioIdVersion&0xff;

  printf("Version: %d %d/%d %d-%d %d\n",boardRevision,month,day,majorRev,minorRev,rev);


}
