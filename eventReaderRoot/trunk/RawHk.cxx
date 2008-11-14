//////////////////////////////////////////////////////////////////////////////
/////  RawHk.h        Raw Hk Data                                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Raw Hk     structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawHk.h"
#include "AnitaPacketUtil.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(RawHk);

RawHk::RawHk() 
{
   //Default Constructor
}

RawHk::~RawHk() {
   //Default Destructor
}


RawHk::RawHk(Int_t trun, Int_t trealTime, HkDataStruct_t *hkPtr)
{

 if(hkPtr->gHdr.code!=PACKET_HKD ||
     hkPtr->gHdr.verId!=VER_HK_FULL ||
     hkPtr->gHdr.numBytes!=sizeof(HkDataStruct_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_HKD) << "\n" 
	      << "code:\t" << hkPtr->gHdr.code << "\t" << PACKET_HKD 
	      << "\nversion:\t" << hkPtr->gHdr.verId 
	      << "\t" << VER_HK_FULL 
	      << "\nsize:\t" << hkPtr->gHdr.numBytes << "\t"
	      << sizeof(HkDataStruct_t) << std::endl;
  }
  run=trun;
  realTime=trealTime;
  payloadTime=hkPtr->unixTime;
  payloadTimeUs=hkPtr->unixTimeUs;
  acromagCode=hkPtr->ip320.code;  
  memcpy(acromagData[0],hkPtr->ip320.board[0].data,sizeof(UShort_t)*CHANS_PER_IP320);
  memcpy(acromagData[1],hkPtr->ip320.board[1].data,sizeof(UShort_t)*CHANS_PER_IP320);
  memcpy(acromagData[2],hkPtr->ip320.board[2].data,sizeof(UShort_t)*CHANS_PER_IP320);
  magX=hkPtr->mag.x;
  magY=hkPtr->mag.y;
  magZ=hkPtr->mag.z;
  memcpy(sbsTemp,hkPtr->sbs.temp,sizeof(Short_t)*4);
}

