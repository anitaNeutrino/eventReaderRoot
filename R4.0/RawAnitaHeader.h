//////////////////////////////////////////////////////////////////////////////
/////  RawAnitaHeader.h        Raw ANITA header class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ANITA headers in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWANITAHEADER_H
#define RAWANITAHEADER_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"

class RawAnitaHeader: public TObject
{
 public:
  RawAnitaHeader();
  RawAnitaHeader(AnitaEventHeader_t *hdPtr, Int_t run, UInt_t realTime,
		 UInt_t triggerTime, UInt_t triggerTimeNs,Int_t tgoodTimeFlag);
  RawAnitaHeader(Int_t    trun,
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
		 Int_t    tgoodTimeFlag);		 
  ~RawAnitaHeader();

  Int_t           run;
  UInt_t          realTime;
  UInt_t          payloadTime;
  UInt_t          payloadTimeUs;
  UInt_t          gpsSubTime;
  UInt_t          eventNumber;
  UShort_t        calibStatus;
  UChar_t         priority;
  UChar_t         turfUpperWord;
  UChar_t         otherFlag;
  UChar_t         errorFlag;
  UChar_t         otherFlag3;
  UChar_t         nadirAntTrigMask;
  UInt_t          antTrigMask;
  UChar_t         trigType;
  UChar_t         l3Type1Count;
  UShort_t        trigNum;
  UInt_t          trigTime;
  UInt_t          c3poNum;
  UShort_t        ppsNum;
  UShort_t        deadTime;
  UChar_t         bufferDepth;
  UShort_t        upperL1TrigPattern;
  UShort_t        lowerL1TrigPattern;
  UShort_t        upperL2TrigPattern;
  UShort_t        lowerL2TrigPattern;
  UShort_t        l3TrigPattern;
  UInt_t          triggerTime;
  UInt_t          triggerTimeNs;
  Int_t           goodTimeFlag;
   
  char *trigTypeAsString();
   
  ClassDef(RawAnitaHeader,11);
};


#endif //RAWANITAHEADER_H
