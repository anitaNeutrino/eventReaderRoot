//////////////////////////////////////////////////////////////////////////////
/////  AveragedSurfHk.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AveragedSurfHk.h"
#include <iostream>
#include <fstream>

ClassImp(AveragedSurfHk);

AveragedSurfHk::AveragedSurfHk() 
{
   //Default Constructor
}

AveragedSurfHk::~AveragedSurfHk() {
   //Default Destructor
}


AveragedSurfHk::AveragedSurfHk(Int_t trun, Int_t trealTime, AveragedSurfHkStruct_t *surfPtr)
{
  run=trun;
  realTime=trealTime;
  payloadTime=surfPtr->unixTime;
  numHks=surfPtr->numHks;
  deltaT=surfPtr->deltaT;
  hadError=surfPtr->hadError;
  globalThreshold=surfPtr->globalThreshold;
  reserved=surfPtr->reserved;
  memcpy(scalerGoals,surfPtr->scalerGoals,sizeof(UShort_t)*BANDS_PER_ANT);
  memcpy(avgScaler,surfPtr->avgScaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(rmsScaler,surfPtr->rmsScaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(avgThresh,surfPtr->avgThresh,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(rmsThresh,surfPtr->rmsThresh,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(avgRFPower,surfPtr->avgRFPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(rmsRFPower,surfPtr->rmsRFPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(surfTrigBandMask,surfPtr->surfTrigBandMask,sizeof(UShort_t)*ACTIVE_SURFS);
  intFlag=0;
}


int AveragedSurfHk::getSurfScaler(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band, Int_t &surf, Int_t &scl) {
  if(phi<0 || phi>16 || ring<0 || ring>2 || band<0 || band>3)
    return -1;
  if(ring==AnitaRing::kUpperRing|| ring==AnitaRing::kLowerRing) {
    surf=phi/2; //SURF 1 has Phi 1 & 2
    scl=8*(phi%2) + 4*ring + band;
  }
  else {
    surf=8;
    if(phi>=8) {
      surf=9;
      phi-=8;
    }
    scl=4*(phi/2) + band;
  }      
  return 0;
}

Int_t AveragedSurfHk::getScaler(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band)
{
   Int_t surf,scl;
   if(getSurfScaler(phi,ring,band,surf,scl)<0)
     return -1;
   return avgScaler[surf][scl];
   
}

Int_t AveragedSurfHk::getThreshold(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band)
{
   Int_t surf,scl;
   if(getSurfScaler(phi,ring,band,surf,scl)<0)
     return -1;
   return avgThresh[surf][scl];

}

Int_t AveragedSurfHk::getScalerRMS(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band)
{
   Int_t surf,scl;
   if(getSurfScaler(phi,ring,band,surf,scl)<0)
     return -1;
   return rmsScaler[surf][scl];
}

Int_t AveragedSurfHk::getThresholdRMS(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band)
{
   Int_t surf,scl;
   if(getSurfScaler(phi,ring,band,surf,scl)<0)
     return -1;
   return rmsThresh[surf][scl];

}


Int_t AveragedSurfHk::isBandMasked(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band)
{
  
  Int_t surf,scl;
  if(getSurfScaler(phi,ring,band,surf,scl)<0)
    return -1;
  return isBandMasked(surf,scl);
  
}

Int_t AveragedSurfHk::getLogicalIndex(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band)
{
  Int_t surf,scl;
  if(getSurfScaler(phi,ring,band,surf,scl)<0)
    return -1;
  return scl +surf*SCALERS_PER_SURF;
}
