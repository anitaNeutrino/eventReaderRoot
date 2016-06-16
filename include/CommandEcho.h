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


//!  CommandEcho -- The command echo data
/*!
  The ROOT implementation of the command echo data
  \ingroup rootclasses
*/
class CommandEcho: public TObject
{
 public:
   CommandEcho(); ///< Defualt constructor		 
   ~CommandEcho(); ///< Destructor

   CommandEcho(Int_t trun, Int_t trealTime, CommandEcho_t *echoPtr); ///< Assignment constructor

   Int_t           run; ///< Assigned offline
   UInt_t          realTime; ///< unixTime
   UInt_t          payloadTime; ///< unixTime
   UShort_t        goodFlag; ///< Command flag value (was the command successful?)
   UShort_t        numCmdBytes; ///< Number of command bytes
   UChar_t         cmd[MAX_CMD_LENGTH]; ///< List of command bytes
   Int_t           fromPayload; ///< Was the command auto-generated or sent from the ground?


   const char * getCommandAsString() ;

  ClassDef(CommandEcho,11);
};


#endif //COMMANDECHO_H
