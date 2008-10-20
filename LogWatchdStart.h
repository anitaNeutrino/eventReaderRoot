//////////////////////////////////////////////////////////////////////////////
/////  LogWatchdStart.h        LogWatchd Start                           /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing LogWatchd Start structs in a tree   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef LOGWATCHDSTART_H
#define LOGWATCHDSTART_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"



//!  LogWatchdStart -- The start packet from LogWatchd
/*!
  The ROOT implementation of the start packet from LogWatchd
  \ingroup rootclasses
*/
class LogWatchdStart: public TObject
{
 public:
   LogWatchdStart(); ///< Default constructor
   ~LogWatchdStart(); ///< Destructor

   LogWatchdStart(Int_t trun, Int_t trealTime, LogWatchdStart_t *startPtr); ///< Assignment constructor

   Int_t           run; ///< Run number assigned offline
   UInt_t          realTime; ///< Time in unixTime
   UInt_t          payloadTime; ///< Time in unixTime
   UInt_t          runReported; ///< Run number from the payload
   Float_t         upTime; ///< CPU uptime
   Float_t         idleTime; ///< CPU idletime

  ClassDef(LogWatchdStart,10);
};


#endif //LOGWATCHDSTART_H
