//////////////////////////////////////////////////////////////////////////////
/////  AveragedSurfHk.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AveragedSurfHk.h"
#include "AnitaPacketUtil.h"
#include "AnitaGeomTool.h"
#include "AnitaEventCalibrator.h"
#include <iostream>
#include <fstream>
#include <cstring>

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
 if(surfPtr->gHdr.code!=PACKET_AVG_SURF_HK ||
     surfPtr->gHdr.verId!=VER_AVG_SURF_HK ||
     surfPtr->gHdr.numBytes!=sizeof(AveragedSurfHkStruct_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_AVG_SURF_HK) <<"\n" 
	      << "code:\t" << surfPtr->gHdr.code << "\t" << PACKET_AVG_SURF_HK 
	      << "\nversion:\t" << surfPtr->gHdr.verId 
	      << "\t" << VER_AVG_SURF_HK 
	      << "\nsize:\t" << surfPtr->gHdr.numBytes << "\t"
	      << sizeof(AveragedSurfHkStruct_t) << std::endl;
  }

  run=trun;
  realTime=trealTime;
  payloadTime=surfPtr->unixTime;
  numHks=surfPtr->numHks;
  deltaT=surfPtr->deltaT;
  hadError=surfPtr->hadError;
  globalThreshold=surfPtr->globalThreshold;
  reserved=surfPtr->reserved;
  memcpy(scalerGoals,surfPtr->scalerGoals,sizeof(UShort_t)*BANDS_PER_ANT);
  memcpy(scalerGoalsNadir,surfPtr->scalerGoalsNadir,sizeof(UShort_t)*BANDS_PER_ANT);
  memcpy(avgScaler,surfPtr->avgScaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(rmsScaler,surfPtr->rmsScaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(avgThresh,surfPtr->avgThresh,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(rmsThresh,surfPtr->rmsThresh,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(avgRFPower,surfPtr->avgRFPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(rmsRFPower,surfPtr->rmsRFPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(surfTrigBandMask,surfPtr->surfTrigBandMask,sizeof(UShort_t)*ACTIVE_SURFS);
  intFlag=0;
}


AveragedSurfHk::AveragedSurfHk(Int_t trun, Int_t trealTime, AveragedSurfHkStructVer14_t *surfPtr)
{
 if(surfPtr->gHdr.code!=PACKET_AVG_SURF_HK ||
     surfPtr->gHdr.verId!=14 ||
     surfPtr->gHdr.numBytes!=sizeof(AveragedSurfHkStructVer14_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_AVG_SURF_HK) << "\n" 
	      << "code:\t" << surfPtr->gHdr.code << "\t" << PACKET_AVG_SURF_HK 
	      << "\nversion:\t" << surfPtr->gHdr.verId 
	      << "\t" << 14 
	      << "\nsize:\t" << surfPtr->gHdr.numBytes << "\t"
	      << sizeof(AveragedSurfHkStructVer14_t) << std::endl;
  }

  run=trun;
  realTime=trealTime;
  payloadTime=surfPtr->unixTime;
  numHks=surfPtr->numHks;
  deltaT=surfPtr->deltaT;
  hadError=surfPtr->hadError;
  globalThreshold=surfPtr->globalThreshold;
  reserved=surfPtr->reserved;
  memcpy(scalerGoals,surfPtr->scalerGoals,sizeof(UShort_t)*BANDS_PER_ANT);
  memcpy(scalerGoalsNadir,surfPtr->scalerGoalsNadir,sizeof(UShort_t)*BANDS_PER_ANT);
  memcpy(avgScaler,surfPtr->avgScaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(rmsScaler,surfPtr->rmsScaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(avgThresh,surfPtr->avgThresh,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(rmsThresh,surfPtr->rmsThresh,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(avgRFPower,surfPtr->avgRFPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(rmsRFPower,surfPtr->rmsRFPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(surfTrigBandMask,surfPtr->surfTrigBandMask,sizeof(UShort_t)*ACTIVE_SURFS);
  intFlag=0;
}


AveragedSurfHk::AveragedSurfHk(Int_t trun, Int_t trealTime, AveragedSurfHkStructVer13_t *surfPtr)
{
 if(surfPtr->gHdr.code!=PACKET_AVG_SURF_HK ||
     surfPtr->gHdr.verId!=13 ||
     surfPtr->gHdr.numBytes!=sizeof(AveragedSurfHkStructVer13_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_AVG_SURF_HK) << "\n" 
	      << "code:\t" << surfPtr->gHdr.code << "\t" << PACKET_AVG_SURF_HK 
	      << "\nversion:\t" << surfPtr->gHdr.verId 
	      << "\t" << 13
	      << "\nsize:\t" << surfPtr->gHdr.numBytes << "\t"
	      << sizeof(AveragedSurfHkStructVer13_t) << std::endl;
  }

  run=trun;
  realTime=trealTime;
  payloadTime=surfPtr->unixTime;
  numHks=surfPtr->numHks;
  deltaT=surfPtr->deltaT;
  hadError=surfPtr->hadError;
  globalThreshold=surfPtr->globalThreshold;
  reserved=0;
  memcpy(scalerGoals,surfPtr->scalerGoals,sizeof(UShort_t)*BANDS_PER_ANT);
  for(int band=0;band<BANDS_PER_ANT;band++)
     scalerGoalsNadir[band]=scalerGoals[band];
  memcpy(avgScaler,surfPtr->avgScaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(rmsScaler,surfPtr->rmsScaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(avgThresh,surfPtr->avgThresh,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(rmsThresh,surfPtr->rmsThresh,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(avgRFPower,surfPtr->avgRFPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(rmsRFPower,surfPtr->rmsRFPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(surfTrigBandMask,surfPtr->surfTrigBandMask,sizeof(UShort_t)*ACTIVE_SURFS);
  intFlag=0;
}


AveragedSurfHk::AveragedSurfHk(Int_t trun, Int_t trealTime, AveragedSurfHkStructVer12_t *surfPtr)
{
 if(surfPtr->gHdr.code!=PACKET_AVG_SURF_HK ||
     surfPtr->gHdr.verId!=12 ||
     surfPtr->gHdr.numBytes!=sizeof(AveragedSurfHkStructVer12_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_AVG_SURF_HK) << "\n" 
	      << "code:\t" << surfPtr->gHdr.code << "\t" << PACKET_AVG_SURF_HK 
	      << "\nversion:\t" << surfPtr->gHdr.verId 
	      << "\t" << 12 
	      << "\nsize:\t" << surfPtr->gHdr.numBytes << "\t"
	      << sizeof(AveragedSurfHkStructVer12_t) << std::endl;
  }

  run=trun;
  realTime=trealTime;
  payloadTime=surfPtr->unixTime;
  numHks=surfPtr->numHks;
  deltaT=surfPtr->deltaT;
  hadError=surfPtr->hadError;
  globalThreshold=surfPtr->globalThreshold;
  reserved=0;
  for(int band=0;band<BANDS_PER_ANT;band++) {
     scalerGoals[band]=surfPtr->scalerGoal;
     scalerGoalsNadir[band]=surfPtr->scalerGoal;
  }
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


Int_t AveragedSurfHk::getScalerGoal(int surf, int scl)
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


Double_t AveragedSurfHk::getRFPowerInK(int surf, int chan)
{
  if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
  if(chan<0 || chan>=RFCHAN_PER_SURF)
    return -1;
  Int_t adc=avgRFPower[surf][chan];

  Double_t kelvin=AnitaEventCalibrator::Instance()->convertRfPowToKelvin(surf,chan,adc);
  return kelvin;

}


Double_t AveragedSurfHk::getRMSRFPowerInK(int surf, int chan)
{
  if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
  if(chan<0 || chan>=RFCHAN_PER_SURF)
    return -1;
  Int_t adcLow=avgRFPower[surf][chan]-rmsRFPower[surf][chan];
  Int_t adcHigh=avgRFPower[surf][chan]+rmsRFPower[surf][chan];
  Double_t kelvinHigh=AnitaEventCalibrator::Instance()->convertRfPowToKelvin(surf,chan,adcHigh);
  Double_t kelvinLow=AnitaEventCalibrator::Instance()->convertRfPowToKelvin(surf,chan,adcLow);  
  return (kelvinHigh-kelvinLow)/2;

}



Double_t AveragedSurfHk::getMeasuredRFPowerInK(int surf, int chan)
{
  if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
  if(chan<0 || chan>=RFCHAN_PER_SURF)
    return -1;
  Int_t adc=avgRFPower[surf][chan];

  Double_t kelvin=AnitaEventCalibrator::Instance()->convertRfPowToKelvinMeasured(surf,chan,adc);
  return kelvin;

}


Double_t AveragedSurfHk::getMeasuredRMSRFPowerInK(int surf, int chan)
{
  if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
  if(chan<0 || chan>=RFCHAN_PER_SURF)
    return -1;
  Int_t adcLow=avgRFPower[surf][chan]-rmsRFPower[surf][chan];
  Int_t adcHigh=avgRFPower[surf][chan]+rmsRFPower[surf][chan];
  Double_t kelvinHigh=AnitaEventCalibrator::Instance()->convertRfPowToKelvinMeasured(surf,chan,adcHigh);
  Double_t kelvinLow=AnitaEventCalibrator::Instance()->convertRfPowToKelvinMeasured(surf,chan,adcLow);  
  return (kelvinHigh-kelvinLow)/2;

}
