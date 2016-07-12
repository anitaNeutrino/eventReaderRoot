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

  memcpy(surfIdBytes,startPtr->surfIdBytes,sizeof(UChar_t)*4*ACTIVE_SURFS);
  memcpy(surfIdVersion,startPtr->surfIdVersion,sizeof(UInt_t)*ACTIVE_SURFS);

  printTurfioVersion();
  printTurfVersion();
  printSurfVersion();

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


void AcqdStart::printSurfVersion() {

  for(int surf=0;surf<ACTIVE_SURFS;surf++) {
    printf("SURF: %c %c %c %c\n",surfIdBytes[surf][0],surfIdBytes[surf][1],surfIdBytes[surf][2],surfIdBytes[surf][3]);
    //    printf("Version: %#x\n",surfIdVersion[surf]);
    //printf("Version: %x %x %x %x\n",surfIdVersion[surf]&0xFF,(surfIdVersion[surf]&0xFF00)>>8,(surfIdVersion[surf]&0xFF0000)>>16,(surfIdVersion[surf]&0xFF000000)>>24);
    int boardRevision=surfIdVersion[surf]>>28;
    int month=((surfIdVersion[surf]>>24)&0xf);
    int day=((surfIdVersion[surf])&0x00ff0000)>>16;
    int majorRev=((surfIdVersion[surf]>>12)&0xf);
    int minorRev=((surfIdVersion[surf]>>8)&0xf);
    int rev=surfIdVersion[surf]&0xff;
    
    printf("SURF %d: Version: %d %d/%d %d-%d %d\n",surf+1,boardRevision,month,day,majorRev,minorRev,rev);
  }


}
