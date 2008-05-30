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
  TimedAnitaHeader(Int_t    trun,
		   UInt_t   trealTime,
		   UInt_t   tpayloadTime,
		   UInt_t   tpayloadTimeUs,
		   UInt_t   tgpsSubTime,
		   UInt_t   teventNumber,
		   UShort_t tcalibStatus,
		   UChar_t  tpriority,
		   UChar_t  tturfUpperWord,
		   UChar_t  totherFlag,
		   UChar_t  terrorFlag,
		   UChar_t  totherFlag3,
		   UChar_t  tnadirAntMask,
		   UInt_t   tantTrigMask,
		   UChar_t  ttrigType,
		   UChar_t  tl3Type1Count,
		   UShort_t ttrigNum,
		   UInt_t   ttrigTime,
		   UInt_t   tc3poNum,
		   UShort_t tppsNum,
		   UShort_t tdeadTime,
		   UChar_t  tbufferDepth,
		   UShort_t tupperL1TrigPattern,
		   UShort_t tlowerL1TrigPattern,
		   UShort_t tupperL2TrigPattern,
		   UShort_t tlowerL2TrigPattern,
		   UShort_t tl3TrigPattern,
		   UInt_t   ttriggerTime,
		   UInt_t   ttriggerTimeNs,
		   Int_t    tgoodTimeFlag,
		   Int_t    tsecQualFlag,
		   UInt_t   toldTriggerTime);		 
  ~TimedAnitaHeader();

  Int_t secQualFlag;
  UInt_t oldTriggerTime;

   
  ClassDef(TimedAnitaHeader,1);
};


#endif //TIMEDANITAHEADER_H
