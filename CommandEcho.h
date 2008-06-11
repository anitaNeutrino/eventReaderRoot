//////////////////////////////////////////////////////////////////////////////
/////  CommandEcho.h        Gpsd Start                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing CommandEcho structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDECHO_H
#define COMMANDECHO_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"


class CommandEcho: public TObject
{
 public:
  CommandEcho();		 
  ~CommandEcho();

  CommandEcho(Int_t trun, Int_t trealTime, CommandEcho_t *echoPtr);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime; 
   UShort_t        goodFlag;
   UShort_t        numCmdBytes;
   UChar_t         cmd[MAX_CMD_LENGTH];

  ClassDef(CommandEcho,10);
};


#endif //COMMANDECHO_H
