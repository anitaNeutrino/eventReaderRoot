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
  run=trun;
  realTime=trealTime;
  payloadTime=echoPtr->unixTime;
  goodFlag=echoPtr->goodFlag;
  numCmdBytes=echoPtr->numCmdBytes;
  memcpy(cmd,echoPtr->cmd,sizeof(UChar_t)*MAX_CMD_LENGTH);

}

