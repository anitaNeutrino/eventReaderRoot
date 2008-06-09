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


class LogWatchdStart: public TObject
{
 public:
  LogWatchdStart();		 
  ~LogWatchdStart();

  LogWatchdStart(Int_t trun, Int_t trealTime, LogWatchdStart_t *startPtr);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime; 
   UInt_t          runReported;
   Float_t         upTime;
   Float_t         idleTime;

  ClassDef(LogWatchdStart,10);
};


#endif //LOGWATCHDSTART_H
