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
   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer16_t *ratePtr); ///< Version 16 constructor
   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer15_t *ratePtr); ///< Version 15 constructor
   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer14_t *ratePtr); ///< Version 14 constructor
   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer13_t *ratePtr); ///< Version 13 constructor
   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer12_t *ratePtr); ///< Version 12 constructor
   TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer11_t *ratePtr); ///< Version 11 constructor

   Int_t           run; ///< Run number, assigned offline
   UInt_t          realTime; ///< Time in unixTime
   UInt_t          payloadTime; ///< Time in unixTime
   UShort_t        ppsNum; ///< ppsNum of data
   UShort_t        deadTime; ///< deadTime (66535 means buffers always full)
   UShort_t        l1Rates[PHI_SECTORS][2]; ///< l1 antenna rates
   UChar_t         upperL2Rates[PHI_SECTORS]; ///< upper l2 cluster rates
   UChar_t         lowerL2Rates[PHI_SECTORS]; ///< lower l2 cluster rates
   UChar_t         l3Rates[PHI_SECTORS]; ///< l3 rates
   UShort_t        nadirL1Rates[NADIR_ANTS]; ///< nadir l1 rates
   UChar_t         nadirL2Rates[NADIR_ANTS]; ///< nadir l2 rates
   UInt_t          antTrigMask; ///< Which upper+lower ring antennas are masked off?
   UChar_t         nadirAntTrigMask; ///< Which nadir antennas are masked off?
   UShort_t        phiTrigMask; ///< Which phi sectors are masked off?
   UChar_t         errorFlag; ///< Error flag (who knows)?
   Int_t           intFlag; ///< Interpolation flag, zero for raw data.

   Int_t getL1Rate(int phi, int ring); ///< Returns l1 rate in phi-ring
   Int_t getL2Rate(int phi, int ring); ///< Returns l2 rate in phi-ring
   Int_t getL3Rate(int phi)
      {return l3Rates[phi];} ///< Returns l3 rate in phi sector
   Int_t getNadirL12Rate(int phi); ///< Returns naidr l1+l2 rate (the L2 rates are the OR of the neighbouring antennas
   Int_t isAntMasked(int phi, int ring); ///< Returns 1 if given phi-ring antenna is masked
   Int_t isPhiMasked(int phi); ///< Returns 1 if given phi is masked
   Float_t getDeadTimeFrac() {return deadTime/65535.;} ///< Returns the deadtime as a fraction of a second (by dividing by 65535)
   
   
  ClassDef(TurfRate,16);
};


#endif //TURFRATE_H
