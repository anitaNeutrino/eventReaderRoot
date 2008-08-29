//////////////////////////////////////////////////////////////////////////////
/////  TimedAnitaHeader.h        Timed ANITA header class                /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing ANITA headers with fixed times      /////
/////     (from matching to the ADU5 data)                               /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef TIMEDANITAHEADER_H
#define TIMEDANITAHEADER_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "RawAnitaHeader.h"
#include "simpleStructs.h"

class TimedAnitaHeader: public RawAnitaHeader
{
 public:
  TimedAnitaHeader();
  TimedAnitaHeader(RawAnitaHeader &old, UInt_t newTriggerTime, Int_t tsecQualFlag);
  TimedAnitaHeader(AnitaEventHeader_t *hdPtr, Int_t run, UInt_t realTime,
		   UInt_t triggerTime, UInt_t triggerTimeNs,
		   Int_t tgoodTimeFlag, Int_t tsecQualFlag, UInt_t toldTriggerTime);
		 
  ~TimedAnitaHeader();

  Int_t secQualFlag;
  UInt_t oldTriggerTime;

   
  ClassDef(TimedAnitaHeader,1);
};


#endif //TIMEDANITAHEADER_H
