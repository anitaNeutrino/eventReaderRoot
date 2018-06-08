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

//!  TurfRate -- The Turf Rate data
/*!
  The ROOT implementation of the TURF rate data
  \ingroup rootclasses
*/
class TurfRate: public TObject
{
 public:
   TurfRate(); ///< Default constructor
   ~TurfRate(); ///< Destructor

   TurfRate(Int_t trun, Int_t trealTime, TurfRateStruct_t *ratePtr); ///< Assignment constructor
   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer41_t *ratePtr); ///< Version 41 constructor
   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer40_t *ratePtr); ///< Version 40 constructor
   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer35_t *ratePtr); ///< Version 34 constructor
   //   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer16_t *ratePtr); ///< Version 16 constructor
   //   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer15_t *ratePtr); ///< Version 15 constructor
   //   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer14_t *ratePtr); ///< Version 14 constructor
   //   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer13_t *ratePtr); ///< Version 13 constructor
   //   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer12_t *ratePtr); ///< Version 12 constructor
   //   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer11_t *ratePtr); ///< Version 11 constructor

   Int_t           run; ///< Run number, assigned offline
   UInt_t          realTime; ///< Time in unixTime
   UInt_t          payloadTime; ///< Time in unixTime
   UShort_t        ppsNum; ///< ppsNum of data
  
  //!  Dead Time
  /*!
   *  The deadtime counter uses 16.384 us ticks ( 256 ns * 2^6) to keep track of the deadtime within a second. Maximum deadtime would then be 61305 in a second. 
   *
    Users are encouraged to use the getDeadTimeFrac function where available. A differential dead time number is available in the RawAnitaHeader, which only includes the fraction of the current second before the event trigger in the count.
  */ 
   UShort_t        deadTime; 
   UShort_t        l2Rates[PHI_SECTORS]; ///< L2 rates (or VPol A3 rates) 
   UShort_t        l2RatesH[PHI_SECTORS]; ///< HPol l2 rates (called l1rates in A3) for A3 compatibility. 0 for A4. 
   UChar_t         l3Rates[PHI_SECTORS]; ///< l3 rates (or VPol A3 rates) 
   UChar_t         l3RatesH[PHI_SECTORS]; ///< HPol l3 rates, for A3 compatibility. 0 for A4. 
   UShort_t        l2TrigMask; ///< Which L2 were masked off in A4 (VPol only in A3) 
   UShort_t        l2TrigMaskH; ///< Which HPol L2 were masked off in A3 (== l2TrigMask in A4)
   UShort_t        phiTrigMask; ///< Which phi sectors are masked off (VPol only in A3)?
   UShort_t        phiTrigMaskH; ///< Which phi sectors are masked off (Hpol, == phiTrigMask in A4)?
   UChar_t         errorFlag; ///< Error flag (who knows)?
   UInt_t          c3poNum; ///< Number of clock cycles per second
   Int_t           intFlag; ///< Interpolation flag, zero for raw data.

   UChar_t         l3RatesGated[PHI_SECTORS]; ///< l3 gated rates
   UShort_t        rfScaler;
   UChar_t         refPulses;
   UChar_t         reserved[3]; ///< Reserved???
   

   
   //Deprecated
   //   UInt_t          antTrigMask; ///< Which upper+lower ring antennas are masked off?
   //   UChar_t         nadirAntTrigMask; ///< Which nadir antennas are masked off?


   Int_t isPhiMasked(int phi); ///< Is the Phi Sector masked
   Int_t isL2Masked(int phi); ///< Returns 1 if given phi is masked   
   Float_t getDeadTimeFrac() {return deadTime/61305.;} ///< Returns the deadtime as a fraction of a second (by dividing by 61305)
   Int_t getL2Rate(int phi)
   { return l2Rates[phi]; }
   Int_t getL3Rate(int phi)
      {return l3Rates[phi];} ///< Returns l3 rate in phi sector

   
   Int_t getL1Rate(int phi, int ring); ///< Deprecated
   Int_t getL2Rate(int phi, int ring); ///< Deprecated
   Int_t getNadirL12Rate(int phi); ///< Deprecated
   Int_t isAntMasked(int phi, int ring); ///< Deprecated
   Int_t isPhiMaskedHPol(int phi); ///< Deprecated
   Int_t isL1Masked(int phi); ///< Deprecated
   Int_t isL1MaskedHPol(int phi); ///< Deprecated
   
   
  ClassDef(TurfRate,43);

};


#endif //TURFRATE_H
