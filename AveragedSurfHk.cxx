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

  AnitaPol::AnitaPol_t thePol;
  Int_t ant;
  AnitaGeomTool::getAntPolFromSurfChan(surf,chan,ant,thePol);

  
Double_t rfPowPedestals[40][2]={{878.25,733.25},{916,747.5},{881.25,739.5},{885.75,760.5},{891.75,726},{837.5,732.5},{833.25,715.5},{907,760},{863.75,730.25},{899.25,728.25},{903.5,734.5},{896.5,745.25},{882.5,753.25},{898.5,721.25},{888.75,722.75},{897,724.5},{934,725.5},{843,727.5},{916.75,747.5},{902.25,728.25},{908,728},{907.75,724.75},{944.25,717.5},{926.25,742},{952.75,731.5},{936,725.5},{930.5,733.5},{910,735.5},{943.75,742.5},{946,775.75},{934.5,713.75},{933.25,785.5},{846.75,722.5},{917.5,695.25},{879.25,703.25},{857,691},{816,702.5},{863.75,790.75},{861,709.5},{929.75,732}};
    Double_t ped=rfPowPedestals[ant][thePol];  

    //  Int_t ped=736;
    //  if(thePol==AnitaPol::kHorizontal)
    //    ped=900;
  
  Double_t a=0.0439;
  Double_t DA=adc-ped;
  Double_t kelvin=290*TMath::Power(10,(a*DA/10.));
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

  AnitaPol::AnitaPol_t thePol;
  Int_t ant;
  AnitaGeomTool::getAntPolFromSurfChan(surf,chan,ant,thePol);

  
Double_t rfPowPedestals[40][2]={{878.25,733.25},{916,747.5},{881.25,739.5},{885.75,760.5},{891.75,726},{837.5,732.5},{833.25,715.5},{907,760},{863.75,730.25},{899.25,728.25},{903.5,734.5},{896.5,745.25},{882.5,753.25},{898.5,721.25},{888.75,722.75},{897,724.5},{934,725.5},{843,727.5},{916.75,747.5},{902.25,728.25},{908,728},{907.75,724.75},{944.25,717.5},{926.25,742},{952.75,731.5},{936,725.5},{930.5,733.5},{910,735.5},{943.75,742.5},{946,775.75},{934.5,713.75},{933.25,785.5},{846.75,722.5},{917.5,695.25},{879.25,703.25},{857,691},{816,702.5},{863.75,790.75},{861,709.5},{929.75,732}};
  Double_t ped=rfPowPedestals[ant][thePol];  

  //  Int_t ped=736;
  //  if(thePol==AnitaPol::kHorizontal)
  //    ped=900;
  
  Double_t a=0.0439;
  Double_t DAlow=adcLow-ped;
  Double_t kelvinLow=290*TMath::Power(10,(a*DAlow/10.));
  Double_t DAhigh=adcHigh-ped;
  Double_t kelvinHigh=290*TMath::Power(10,(a*DAhigh/10.));
  
  return (kelvinHigh-kelvinLow)/2;

}
