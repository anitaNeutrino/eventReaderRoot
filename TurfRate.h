//////////////////////////////////////////////////////////////////////////////
/////  TurfRate.h        Pretty ANITA hk class                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 VTG objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef TURFRATE_H
#define TURFRATE_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"


class TurfRate: public TObject
{
 public:
  TurfRate();		 
  ~TurfRate();

  TurfRate(Int_t trun, Int_t trealTime, TurfRateStruct_t *ratePtr);

  TurfRate(Int_t           trun,
	   UInt_t          trealTime,
	   UInt_t          tpayloadTime,
	   UInt_t          tppsNum,
	   UShort_t        tl1Rates[PHI_SECTORS][2],
	   UChar_t         tupperL2Rates[PHI_SECTORS],
	   UChar_t         tlowerL2Rates[PHI_SECTORS],
	   UChar_t         tl3Rates[PHI_SECTORS],
	   UInt_t          tantTrigMask,
	   UChar_t         tnadirAntTrigMask,
	   Int_t           tintFlag);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UInt_t          ppsNum;
   UShort_t        l1Rates[PHI_SECTORS][2];
   UChar_t         upperL2Rates[PHI_SECTORS];
   UChar_t         lowerL2Rates[PHI_SECTORS];
   UChar_t         l3Rates[PHI_SECTORS];
   UInt_t          antTrigMask;
   UChar_t         nadirAntTrigMask;
   Int_t           intFlag;

   Int_t getL1Rate(int phi, int ring) 
      {return 16*l1Rates[phi][ring];}
   Int_t getL2Rate(int phi, int ring)
      { if(ring==0) return 64*upperL2Rates[phi];
      else if(ring==1) return 64*lowerL2Rates[phi];
      return -1;}
   Int_t getL3Rate(int phi)
      {return l3Rates[phi];}
   Int_t isAntMasked(int phi, int ring);
   
   
  ClassDef(TurfRate,13);
};


#endif //TURFRATE_H
