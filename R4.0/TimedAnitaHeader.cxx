//////////////////////////////////////////////////////////////////////////////
/////  TimedAnitaHeader.cxx        ANITA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing ANITA headers with fixed times      /////
/////     (from matching to the ADU5 data)                               /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "TimedAnitaHeader.h"
#include <iostream>
#include <fstream>

ClassImp(TimedAnitaHeader);

TimedAnitaHeader::TimedAnitaHeader() 
{
   //Default Constructor
}

TimedAnitaHeader::~TimedAnitaHeader() {
   //Default Destructor
}


TimedAnitaHeader::TimedAnitaHeader(AnitaEventHeader_t *hdPtr, Int_t trun, UInt_t trealTime, UInt_t ttriggerTime, UInt_t ttriggerTimeNs, Int_t tgoodTimeFlag, Int_t tsecQualFlag, UInt_t toldTriggerTime)
  : RawAnitaHeader(hdPtr,trun,trealTime,ttriggerTime,ttriggerTimeNs,tgoodTimeFlag)
{
  secQualFlag=tsecQualFlag;
  oldTriggerTime=toldTriggerTime;
}

TimedAnitaHeader::TimedAnitaHeader(Int_t    trun,
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
				   Int_t tsecQualFlag, 
				   UInt_t toldTriggerTime)
  : RawAnitaHeader(trun,
		   trealTime,
		   tpayloadTime,
		   tpayloadTimeUs,
		   tgpsSubTime,
		   teventNumber,
		   tcalibStatus,
		   tpriority,
		   tturfUpperWord,
		   totherFlag,
		   terrorFlag,
		   totherFlag3,
		   tnadirAntMask,
		   tantTrigMask,
		   ttrigType,
		   tl3Type1Count,
		   ttrigNum,
		   ttrigTime,
		   tc3poNum,
		   tppsNum,
		   tdeadTime,
		   tbufferDepth,
		   tupperL1TrigPattern,
		   tlowerL1TrigPattern,
		   tupperL2TrigPattern,
		   tlowerL2TrigPattern,
		   tl3TrigPattern,
		   ttriggerTime,
		   ttriggerTimeNs,
		   tgoodTimeFlag)
{  
  secQualFlag=tsecQualFlag;
  oldTriggerTime=toldTriggerTime;
} 

TimedAnitaHeader::TimedAnitaHeader(RawAnitaHeader &old, UInt_t newTriggerTime, Int_t tsecQualFlag)
  :RawAnitaHeader(old)
{
  oldTriggerTime=triggerTime;
  triggerTime=newTriggerTime;
  secQualFlag=tsecQualFlag;
}
