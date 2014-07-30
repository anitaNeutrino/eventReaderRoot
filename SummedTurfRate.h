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

//!  SummedTurfRate -- The Summed Turf Rate data
/*!
  The ROOT implementation of the summed TURF rate data
  \ingroup rootclasses
*/
class SummedTurfRate: public TObject
{
 public:
   SummedTurfRate(); ///< Default constructor
   ~SummedTurfRate(); ///< Destructor

   SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStruct_t *ratePtr); ///< Assignment constructor
   SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStructVer16_t *ratePtr); ///< Version 16 constructor
   SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStructVer15_t *ratePtr); ///< Version 15 constructor
   SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStructVer14_t *ratePtr); ///< Version 14 constructor
   SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStructVer11_t *ratePtr); ///< Version 11 constructor
   SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStructVer10_t *ratePtr); ///< Version 10 constructor

   Int_t            run; ///< Run number, assigned offline
   UInt_t           realTime; ///< Time in unixTime
   UInt_t           payloadTime; ///< Time in unixTime
   UShort_t         numRates; ///< Number of rates in the sum
   UShort_t         deltaT; ///< Time span of data in seconds
   UInt_t           deadTime; ///< Total deadtime (/ by numRates*66535 to get fraction)
   UChar_t          bufferCount[4]; ///< Count of times each buffer is full
   UInt_t           l1Rates[PHI_SECTORS][2]; ///< Summed l1 rates (upper + lower)
   UShort_t         upperL2Rates[PHI_SECTORS]; ///< Summed l2 rates (upper)
   UShort_t         lowerL2Rates[PHI_SECTORS]; ///< Summed l2 rates (lower)
   UShort_t         l3Rates[PHI_SECTORS]; ///< Summed l3 rates
   UInt_t           nadirL1Rates[NADIR_ANTS]; ///< Summed l1 rates (nadir)
   UShort_t         nadirL2Rates[NADIR_ANTS]; ///< Summed l2 rates (nadir)
   UInt_t           antTrigMask; ///< Which (upper+lower) ants are excluded?
   UShort_t         phiTrigMask; ///< Which phi sectors are masked off?
   UShort_t         phiTrigMaskH; ///< Which phi sectors are masked off?
   UChar_t          nadirAntTrigMask; ///< Which nadir ants are excluded?
   UChar_t          errorFlag; ///< Error flag
   Int_t            intFlag; ///< Interpolation flag -- zero for raw data

   Int_t getL1Rate(int phi, int ring); ///< Returns the l1 rate for ring-phi

   Int_t getL2Rate(int phi, int ring); ///< Returns the l2 rate for ring-phi

   Int_t getL3Rate(int phi) ///< Returns the l3 rate for phi
      {return l3Rates[phi];}
   Int_t getNadirL12Rate(int phi); ///< Returns the nadir l1 + l2 rate fr phi
   Int_t isAntMasked(int phi, int ring); ///< Returns 1 if given ring-phi is maked off
   Int_t isPhiMasked(int phi); ///< Returns 1 if given phi is masked off
   
  ClassDef(SummedTurfRate,16);
};


#endif //SUMMEDTURFRATE_H
