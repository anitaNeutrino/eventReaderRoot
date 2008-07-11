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

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UShort_t        ppsNum;
   UShort_t        deadTime;
   UShort_t        l1Rates[PHI_SECTORS][2];
   UChar_t         upperL2Rates[PHI_SECTORS];
   UChar_t         lowerL2Rates[PHI_SECTORS];
   UChar_t         l3Rates[PHI_SECTORS];
   UShort_t        nadirL1Rates[NADIR_ANTS];
   UChar_t         nadirL2Rates[NADIR_ANTS];   
   UInt_t          antTrigMask;
   UChar_t         nadirAntTrigMask;
   UShort_t        phiTrigMask;
   UChar_t         errorFlag;
   Int_t           intFlag;

   Int_t getL1Rate(int phi, int ring); 
   Int_t getL2Rate(int phi, int ring);
   Int_t getL3Rate(int phi)
      {return l3Rates[phi];}
   Int_t getNadirL12Rate(int phi);
   Int_t isAntMasked(int phi, int ring);
   Int_t isPhiMasked(int phi);
   
   
  ClassDef(TurfRate,16);
};


#endif //TURFRATE_H
