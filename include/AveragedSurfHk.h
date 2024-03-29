//////////////////////////////////////////////////////////////////////////////
/////  AveragedSurfHk.h        SURF hk class                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing SURF Hk objects in a TTree          /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef AVERAGEDSURFHK_H
#define AVERAGEDSURFHK_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"


//!  AveragedSurfHk -- The Averaged SURF Housekeeping data
/*!
  This is a class to hold the averaged SURF housekeeping data. It will be the main method by which SURF housekeeping data is telemetred during the flight.
  \ingroup rootclasses
*/
class AveragedSurfHk: public TObject
{
 public:
   AveragedSurfHk(); ///<Default constructor		 
   ~AveragedSurfHk(); ///<Default destructor

  AveragedSurfHk(Int_t trun, Int_t trealTime, AveragedSurfHkStruct_t *surfPtr); ///< Default assignment constructor
  AveragedSurfHk(Int_t trun, Int_t trealTime, AveragedSurfHkStructVer30_t *surfPtr); ///< Assignment constructor for version 30
  AveragedSurfHk(Int_t trun, Int_t trealTime, AveragedSurfHkStructVer14_t *surfPtr); ///< Assignment constructor for version 14
  AveragedSurfHk(Int_t trun, Int_t trealTime, AveragedSurfHkStructVer13_t *surfPtr); ///< Assignment constructor for version 13
  AveragedSurfHk(Int_t trun, Int_t trealTime, AveragedSurfHkStructVer12_t *surfPtr); ///< Assignment constructor for version 12

  Int_t           run; ///< Assigned offline
  UInt_t          realTime; ///<Basically unixTime
   UInt_t          payloadTime; ///<Basically unixTime
   UShort_t        numHks; ///< Number of SURF Hk reads that are included in average
   UShort_t        deltaT; ///< Time span of the averaging
   UInt_t          hadError; ///< Flag for errors
   UShort_t        globalThreshold; ///< Global trheshold (if in global threshold mode)
   UShort_t        reserved; ///< Reserved for future expansion
   UShort_t        scalerGoals[3]; ///< The scaler goals for the four bands
   UShort_t        upperWords[ACTIVE_SURFS]; ///< The upper words -- contains debugging info
   UShort_t        avgScaler[ACTIVE_SURFS][SCALERS_PER_SURF]; ///< The mean scaler value per channel
   UShort_t        rmsScaler[ACTIVE_SURFS][SCALERS_PER_SURF]; ///< The rms scaler value per channel
   UShort_t        avgL1[ACTIVE_SURFS][L1S_PER_SURF]; ///< The mean scaler value per channel
   UShort_t        rmsL1[ACTIVE_SURFS][L1S_PER_SURF]; ///< The rms scaler value per channel
   UShort_t        avgThresh[ACTIVE_SURFS][SCALERS_PER_SURF]; ///< The mean threshold value per channel
   UShort_t        rmsThresh[ACTIVE_SURFS][SCALERS_PER_SURF]; ///< The rms threhsold value per channel

   UShort_t        avgRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF]; ///< The mean RF power value per channel
   UShort_t        rmsRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF]; ///< The rms RF power value per channel
   UShort_t        surfTrigBandMask[ACTIVE_SURFS]; ///< The mask showing whcih bands were excluded from the trigger
   Int_t           intFlag; ///< Only used if this is interpolated data
   
   Int_t isBandMasked(int surf, int scl)
      { return (surfTrigBandMask[surf]&(1<<scl));} ///< Was the given band masked off (1 is band excluded from trigger, 0 is band included)
   Int_t getL1Scaler(int phi, AnitaPol::AnitaPol_t pol,  AnitaRing::AnitaRing_t ring); ///<Returns the L1 scaler value for given phi-ring-pol 
   Int_t getL1ScalerRMS(int phi, AnitaPol::AnitaPol_t pol, AnitaRing::AnitaRing_t ring); ///< Get L1 scaler for given phi-ring
   Int_t getScaler(int phi, AnitaRing::AnitaRing_t ring, AnitaTrigPol::AnitaTrigPol_t pol); ///< Get scaler for given phi-ring-pol
   Int_t getScalerRMS(int phi, AnitaRing::AnitaRing_t ring, AnitaTrigPol::AnitaTrigPol_t pol); ///< Get scaler RMS for given phi-ring-pol
   Int_t getThreshold(int phi, AnitaRing::AnitaRing_t ring,  AnitaTrigPol::AnitaTrigPol_t pol); ///< Get threhsold for given phi-ring-pol
   Int_t getThresholdRMS(int phi, AnitaRing::AnitaRing_t ring,  AnitaTrigPol::AnitaTrigPol_t pol); ///< Get threhsold RMS for given phi-ring-pol
   Int_t isBandMasked(int phi, AnitaRing::AnitaRing_t ring,  AnitaTrigPol::AnitaTrigPol_t pol); ///< Get band masking for given phi-ring-pol
   Int_t getLogicalIndex(int phi, AnitaRing::AnitaRing_t ring,  AnitaTrigPol::AnitaTrigPol_t pol); ///< Get logical index for phi-ring-pol
   int getSurfScaler(int phi, AnitaRing::AnitaRing_t ring,  AnitaTrigPol::AnitaTrigPol_t pol, Int_t &surf, Int_t &scl); ///< Get surf and scaler for  phi-ring-pol
   Int_t getScalerGoal(int surf, int scl); ///< Get scaler goal for given surf-scaler
   Double_t getRFPowerInK(int surf, int chan); ///< Returns the pseudo-calibrated RF power in K.
   Double_t getRMSRFPowerInK(int surf, int chan); ///< Returns the pseudo-calibrated RF power in K.
   //   Double_t getMeasuredRFPowerInK(int surf, int chan); ///< Returns the pseudo-calibrated RF power in K.
   //   Double_t getMeasuredRMSRFPowerInK(int surf, int chan); ///< Returns the pseudo-calibrated RF power in K.
  ClassDef(AveragedSurfHk,40);
};


#endif //AVERAGEDSURFHK_H
