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
  ~RawAnitaHeader();

  Int_t           run;
  UInt_t          realTime;
  UInt_t          payloadTime;
  UInt_t          payloadTimeUs;
  UInt_t          gpsSubTime;
  UInt_t          turfEventId;
  UInt_t          eventNumber;
  UShort_t        calibStatus;
  UChar_t         priority;
  UChar_t         turfUpperWord;
  UChar_t         otherFlag;
  UChar_t         errorFlag;
  UChar_t         surfSlipFlag;
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
  UChar_t         reserved;
  UShort_t        upperL1TrigPattern;
  UShort_t        lowerL1TrigPattern;
  UShort_t        upperL2TrigPattern;
  UShort_t        lowerL2TrigPattern;
  UShort_t        l3TrigPattern;
  UShort_t        otherTrigPattern[3];
  UChar_t         nadirL1TrigPattern;
  UChar_t         nadirL2TrigPattern;
  UInt_t          triggerTime;
  UInt_t          triggerTimeNs;
  Int_t           goodTimeFlag;
   
  char *trigTypeAsString();
   
  ClassDef(RawAnitaHeader,12);
};


#endif //RAWANITAHEADER_H
