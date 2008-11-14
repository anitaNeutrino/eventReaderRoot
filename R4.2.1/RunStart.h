//////////////////////////////////////////////////////////////////////////////
/////  RunStart.h        Run Start                           /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Run Start structs in a tree   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RUNSTARTSTART_H
#define RUNSTARTSTART_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"

//!  RunStart -- The Run start data
/*!
  The ROOT implementation of the run start data
  \ingroup rootclasses
*/
class RunStart: public TObject
{
 public:
   RunStart(); ///<Default constructor
   ~RunStart(); ///< Destructor

   RunStart(Int_t run, Int_t trealTime, RunStart_t *startPtr); ///< Assignment constructor

  Int_t           run; ///< Run number from offline
  UInt_t          realTime; ///< Time in unixTime
  UInt_t          payloadTime; ///< Time in unixTime
  UInt_t          runReported; ///< Run number from payload
  UInt_t          eventNumber; ///< Starting event number

  ClassDef(RunStart,10);
};


#endif //RUNSTARTSTART_H
