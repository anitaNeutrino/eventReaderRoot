//////////////////////////////////////////////////////////////////////////////
/////  SurfHk.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "SurfHk.h"
#include <iostream>
#include <fstream>

ClassImp(SurfHk);

SurfHk::SurfHk() 
{
   //Default Constructor
}

SurfHk::~SurfHk() {
   //Default Destructor
}


SurfHk::SurfHk(Int_t trun, Int_t trealTime, FullSurfHkStruct_t *surfPtr)
{
  if(surfPtr->gHdr.code!=PACKET_SURF_HK ||
     surfPtr->gHdr.verId!=VER_SURF_HK ||
     surfPtr->gHdr.numBytes!=sizeof(FullSurfHkStruct_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << surfPtr->gHdr.code << "\t" << PACKET_SURF_HK 
	      << "\nversion:\t" << surfPtr->gHdr.verId 
	      << "\t" << VER_SURF_HK 
	      << "\nsize:\t" << surfPtr->gHdr.numBytes << "\t"
	      << sizeof(FullSurfHkStruct_t) << std::endl;
  }
     

  run=trun;
  realTime=trealTime;
  payloadTime=surfPtr->unixTime;
  payloadTimeUs=surfPtr->unixTimeUs;
  globalThreshold=surfPtr->globalThreshold;
  errorFlag=surfPtr->errorFlag;
  memcpy(scalerGoals,surfPtr->scalerGoals,sizeof(UShort_t)*BANDS_PER_ANT);
  memcpy(scalerGoalsNadir,surfPtr->scalerGoalsNadir,sizeof(UShort_t)*BANDS_PER_ANT);
  memcpy(upperWords,surfPtr->upperWords,sizeof(UShort_t)*ACTIVE_SURFS);
  memcpy(scaler,surfPtr->scaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(threshold,surfPtr->threshold,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(setThreshold,surfPtr->setThreshold,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(rfPower,surfPtr->rfPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(surfTrigBandMask,surfPtr->surfTrigBandMask,sizeof(UShort_t)*ACTIVE_SURFS);
  intFlag=0;
}

SurfHk::SurfHk(Int_t           trun,
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
	       Int_t           tintFlag)
{

   run=trun;
   realTime=trealTime;
   payloadTime=tpayloadTime;
   payloadTimeUs=tpayloadTimeUs;
   globalThreshold=tglobalThreshold;
   errorFlag=terrorFlag;
   memcpy(scalerGoals,tscalerGoals,sizeof(UShort_t)*BANDS_PER_ANT);
   memcpy(scalerGoalsNadir,tscalerGoalsNadir,sizeof(UShort_t)*BANDS_PER_ANT);
   memcpy(upperWords,tupperWords,sizeof(UShort_t)*ACTIVE_SURFS);
   memcpy(scaler,tscaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
   memcpy(threshold,tthreshold,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
   memcpy(setThreshold,tsetThreshold,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
   memcpy(rfPower,trfPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
   memcpy(surfTrigBandMask,tsurfTrigBandMask,sizeof(UShort_t)*ACTIVE_SURFS);
   intFlag=tintFlag;

}

Int_t SurfHk::getScaler(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band)
{
   Int_t surf,scl;
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
   return scaler[surf][scl];
}

Int_t SurfHk::getThreshold(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band)
{
   Int_t surf,scl;
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
   return threshold[surf][scl];

}

Int_t SurfHk::getSetThreshold(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band)
{
 Int_t surf,scl;
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
   return setThreshold[surf][scl];
}

Int_t SurfHk::isBandMasked(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band)
{
Int_t surf,scl;
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
   return isBandMasked(surf,scl);

}

Int_t SurfHk::getLogicalIndex(int phi, AnitaRing::AnitaRing_t ring, AnitaBand::AnitaBand_t band)
{
   Int_t surf,scl;
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
   return scl +surf*SCALERS_PER_SURF;
}


Int_t SurfHk::getScalerGoal(int surf, int scl)
{
  int band=scl%4;
  if(surf<8) {
    //Upper or lower ring
    return scalerGoals[band];
  }
  else if(surf<ACTIVE_SURFS) {
    //Nadir Ring
    return scalerGoalsNadir[band];
  }
  return -1;
}
