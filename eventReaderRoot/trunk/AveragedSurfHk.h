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


class AveragedSurfHk: public TObject
{
 public:
  AveragedSurfHk();		 
  ~AveragedSurfHk();

  AveragedSurfHk(Int_t trun, Int_t trealTime, AveragedSurfHkStruct_t *surfPtr);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UShort_t        numHks;
   UShort_t        deltaT;   
   UInt_t          hadError;
   UShort_t        globalThreshold;
   UShort_t        reserved;
   UShort_t        scalerGoals[BANDS_PER_ANT];
   UShort_t        scalerGoalsNadir[BANDS_PER_ANT];
   UShort_t        upperWords[ACTIVE_SURFS];
   UShort_t        avgScaler[ACTIVE_SURFS][SCALERS_PER_SURF];
   UShort_t        rmsScaler[ACTIVE_SURFS][SCALERS_PER_SURF];
   UShort_t        avgThresh[ACTIVE_SURFS][SCALERS_PER_SURF];
   UShort_t        rmsThresh[ACTIVE_SURFS][SCALERS_PER_SURF];
   UShort_t        avgRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
   UShort_t        rmsRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
   UShort_t        surfTrigBandMask[ACTIVE_SURFS];
   Int_t           intFlag;
   
   Int_t isBandMasked(int surf, int scl)
      { return (surfTrigBandMask[surf]&(1<<scl));}

   Int_t getScaler(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band);
   Int_t getScalerRMS(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band);
   Int_t getThreshold(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band);
   Int_t getThresholdRMS(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band);
   Int_t isBandMasked(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band);
   Int_t getLogicalIndex(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band);
   int getSurfScaler(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band, Int_t &surf, Int_t &scl);
   Int_t getScalerGoal(int surf, int scl);
  ClassDef(AveragedSurfHk,13);
};


#endif //AVERAGEDSURFHK_H
