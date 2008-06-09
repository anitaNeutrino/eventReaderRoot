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


class RunStart: public TObject
{
 public:
  RunStart();		 
  ~RunStart();

  RunStart(Int_t run, Int_t trealTime, RunStart_t *startPtr);

  Int_t           run; //That I think
   UInt_t          realTime;
   UInt_t          payloadTime; 
   UInt_t          runReported;
   UInt_t         eventNumber;

  ClassDef(RunStart,10);
};


#endif //RUNSTARTSTART_H
