//////////////////////////////////////////////////////////////////////////////
/////  CommandEcho.h        CommandEcho                                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing command echoes in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "CommandEcho.h"
#include <iostream>
#include <fstream>

ClassImp(CommandEcho);

CommandEcho::CommandEcho() 
{
   //Default Constructor
}

CommandEcho::~CommandEcho() {
   //Default Destructor
}


CommandEcho::CommandEcho(Int_t trun, Int_t trealTime, CommandEcho_t *echoPtr)
{

  if((echoPtr->gHdr.code&BASE_PACKET_MASK)!=PACKET_CMD_ECHO ||
     echoPtr->gHdr.verId!=VER_CMD_ECHO ||
     echoPtr->gHdr.numBytes!=sizeof(CommandEcho_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << echoPtr->gHdr.code << "\t" << PACKET_CMD_ECHO 
	      << "\nversion:\t" << echoPtr->gHdr.verId 
	      << "\t" << VER_CMD_ECHO 
	      << "\nsize:\t" << echoPtr->gHdr.numBytes << "\t"
	      << sizeof(CommandEcho_t) << std::endl;
  }
  run=trun;
  realTime=trealTime;
  payloadTime=echoPtr->unixTime;
  goodFlag=echoPtr->goodFlag;
  numCmdBytes=echoPtr->numCmdBytes;
  memcpy(cmd,echoPtr->cmd,sizeof(UChar_t)*MAX_CMD_LENGTH);
  if(echoPtr->gHdr.code & CMD_FROM_PAYLOAD) {
    fromPayload=1;
  }
  else {
    fromPayload=0;
  }
}

