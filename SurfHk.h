//////////////////////////////////////////////////////////////////////////////
/////  SurfHk.h        SURF hk class                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing SURF Hk objects in a TTree          /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef SURFHK_H
#define SURFHK_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"


//!  SurfHk -- The raw SURF scaler+threshold data
/*!
  The ROOT implementation of the SURF scaler, threshold and RF power data
  \ingroup rootclasses
*/
class SurfHk: public TObject
{
 public:
   SurfHk(); ///< Default constructor
   ~SurfHk(); ///< Destructor

   SurfHk(Int_t trun, Int_t trealTime, FullSurfHkStruct_t *surfPtr); ///< Assignment constructor
   SurfHk(Int_t trun, Int_t trealTime, FullSurfHkStructVer14_t *surfPtr); ///< Version 14 constructor
   SurfHk(Int_t trun, Int_t trealTime, FullSurfHkStructVer13_t *surfPtr); ///< Version 13 constructor 
   SurfHk(Int_t trun, Int_t trealTime, FullSurfHkStructVer12_t *surfPtr); ///< Version 12 constructor

  SurfHk(Int_t           trun,
	 UInt_t          trealTime,
	 UInt_t          tpayloadTime,
	 UInt_t          tpayloadTimeUs,
	 UShort_t        tglobalThreshold,
	 UShort_t        terrorFlag,
	 UShort_t        tscalerGoals[BANDS_PER_ANT],
	 UShort_t        tscalerGoalsNadir[BANDS_PER_ANT],
	 UShort_t        tupperWords[ACTIVE_SURFS],
	 UShort_t        tscaler[ACTIVE_SURFS][SCALERS_PER_SURF],
	 UShort_t        tthreshold[ACTIVE_SURFS][SCALERS_PER_SURF],
	 UShort_t        tsetThreshold[ACTIVE_SURFS][SCALERS_PER_SURF],
	 UShort_t        trfPower[ACTIVE_SURFS][RFCHAN_PER_SURF],
	 UShort_t        tsurfTrigBandMask[ACTIVE_SURFS],
	 Int_t           tintFlag); ///< Old assignment constructor

  Int_t           run; ///< Run number from offline
  UInt_t          realTime; ///< Time in unixTime
  UInt_t          payloadTime; ///< Time in unixTime
  UInt_t          payloadTimeUs; ///< Subsecond time of readout in us
  UShort_t        globalThreshold; ///< Global threshold if set
  UShort_t        errorFlag; ///< Error flag
  UShort_t        scalerGoals[BANDS_PER_ANT]; ///< Scaler goal for each band
  UShort_t        scalerGoalsNadir[BANDS_PER_ANT]; ///< Scaler goal for each band of the nadir antennas
  UShort_t        upperWords[ACTIVE_SURFS]; ///< Upper words of each SURF... for debugging
  UShort_t        scaler[ACTIVE_SURFS][SCALERS_PER_SURF]; ///< Scaler values, multiple by 1000 to get Hz.
  UShort_t        threshold[ACTIVE_SURFS][SCALERS_PER_SURF]; ///< Threshold values in DAC counts
  UShort_t        setThreshold[ACTIVE_SURFS][SCALERS_PER_SURF]; ///< Threshold values intedned, should match threshold array exactly
  UShort_t        rfPower[ACTIVE_SURFS][RFCHAN_PER_SURF]; ///< RF power per input channel in ADC counts
  UShort_t        surfTrigBandMask[ACTIVE_SURFS]; ///< Which bands are masked off?
  Int_t           intFlag; ///< Interpolation flag, should be zero for raw data
   
   Int_t isBandMasked(int surf, int scl)
      { return (surfTrigBandMask[surf]&(1<<scl));} ///< Returns 1 if band is masked

   Int_t getScaler(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band); ///< Returns scaler value for given ring-phi-band
   Int_t getThreshold(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band) ;///< Returns readback threhsold value for given ring-phi-band
   Int_t getSetThreshold(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band); ///< Returns set threshold value for given ring-phi-band
   Int_t isBandMasked(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band); ///< Returns 1 if given phi-ring-band is masked
   Int_t getLogicalIndex(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band); ///< Returns logical band index
   Int_t getScalerGoal(int surf, int scl); ///< Returns scaler goal of surf-scaler
   Double_t getRFPowerInK(int surf, int chan); ///< Returns the pseudo-calibrated RF power in K.

  ClassDef(SurfHk,14);
};


#endif //SURFHK_H
