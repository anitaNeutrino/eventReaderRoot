//////////////////////////////////////////////////////////////////////////////
/////  SummedTurfRate.h        Pretty ANITA hk class                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 VTG objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef SUMMEDTURFRATE_H
#define SUMMEDTURFRATE_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"


class SummedTurfRate: public TObject
{
 public:
  SummedTurfRate();		 
  ~SummedTurfRate();

  SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStruct_t *ratePtr);

   Int_t            run;
   UInt_t           realTime;
   UInt_t           payloadTime;
   UShort_t         numRates;
   UShort_t         deltaT;
   UInt_t           deadTime;
   UChar_t          bufferCount[4];
   UInt_t           l1Rates[PHI_SECTORS][2];
   UShort_t         upperL2Rates[PHI_SECTORS];
   UShort_t         lowerL2Rates[PHI_SECTORS];
   UShort_t         l3Rates[PHI_SECTORS];
   UInt_t           nadirL1Rates[NADIR_ANTS];
   UShort_t         nadirL2Rates[NADIR_ANTS];
   UInt_t           antTrigMask;
   UShort_t         phiTrigMask;
   UChar_t          nadirAntTrigMask;
   UChar_t          errorFlag;
   Int_t            intFlag;

   Int_t getL1Rate(int phi, int ring); 

   Int_t getL2Rate(int phi, int ring);

   Int_t getL3Rate(int phi)
      {return l3Rates[phi];}
   Int_t isAntMasked(int phi, int ring);
   Int_t isPhiMasked(int phi);
   
  ClassDef(SummedTurfRate,16);
};


#endif //SUMMEDTURFRATE_H
