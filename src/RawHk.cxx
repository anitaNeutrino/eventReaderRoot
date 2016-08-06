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
  simplePacketCheck(&(hkPtr->gHdr),PACKET_HKD);
 this->gHdr_verId=hkPtr->gHdr.verId;
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
  memcpy(sbsTemp,hkPtr->sbs.temp,sizeof(Short_t)*3);
  memcpy(ntuTemp,&hkPtr->sbs.temp[3],sizeof(Short_t)*3);

}

