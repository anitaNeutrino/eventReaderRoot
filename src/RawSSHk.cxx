//////////////////////////////////////////////////////////////////////////////
/////  RawSSHk.h        Raw Hk Data                                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Raw Hk     structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawSSHk.h"
#include "AnitaPacketUtil.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(RawSSHk);

RawSSHk::RawSSHk() 
{
   //Default Constructor
}

RawSSHk::~RawSSHk() {
   //Default Destructor
}


RawSSHk::RawSSHk(Int_t trun, Int_t trealTime, SSHkDataStruct_t *hkPtr)
{
  simplePacketCheck(&(hkPtr->gHdr),PACKET_HKD_SS);
  run=trun;
  realTime=trealTime;
  payloadTime=hkPtr->unixTime;
  payloadTimeUs=hkPtr->unixTimeUs;
  acromagCode=hkPtr->ip320.code;  
  memcpy(acromagData,hkPtr->ip320.board.data,sizeof(UShort_t)*CHANS_PER_IP320);
}

