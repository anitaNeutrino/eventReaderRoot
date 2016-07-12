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

 if(hkPtr->gHdr.code!=PACKET_HKD_SS ||
     hkPtr->gHdr.verId!=VER_HK_SS ||
     hkPtr->gHdr.numBytes!=sizeof(SSHkDataStruct_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_HKD_SS) << "\n" 
	      << "code:\t" << hkPtr->gHdr.code << "\t" << PACKET_HKD_SS 

	      << "\nversion:\t" << hkPtr->gHdr.verId 
	      << "\t" << VER_HK_SS 
	      << "\nsize:\t" << hkPtr->gHdr.numBytes << "\t"
	      << sizeof(SSHkDataStruct_t) << std::endl;
  }
  run=trun;
  realTime=trealTime;
  payloadTime=hkPtr->unixTime;
  payloadTimeUs=hkPtr->unixTimeUs;
  acromagCode=hkPtr->ip320.code;  
  memcpy(acromagData,hkPtr->ip320.board.data,sizeof(UShort_t)*CHANS_PER_IP320);
}

