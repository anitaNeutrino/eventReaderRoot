//////////////////////////////////////////////////////////////////////////////
/////  SurfHk.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "SurfHk.h"
#include "AnitaPacketUtil.h"
#include "AnitaGeomTool.h"
#include "AnitaEventCalibrator.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include "AnitaVersion.h" 

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
  simplePacketCheck(&(surfPtr->gHdr),PACKET_SURF_HK);
     

  run=trun;
  realTime=trealTime;
  payloadTime=surfPtr->unixTime;
  payloadTimeUs=surfPtr->unixTimeUs;
  globalThreshold=surfPtr->globalThreshold;
  errorFlag=surfPtr->errorFlag;
  memcpy(scalerGoals,surfPtr->scalerGoals,sizeof(UShort_t)*NUM_ANTENNA_RINGS);
  memcpy(upperWords,surfPtr->upperWords,sizeof(UShort_t)*ACTIVE_SURFS);

  //At some point will fix this to do it properly
  int rawSurfToTrigSurf[ACTIVE_SURFS]={-1,-1,0,1,2,3,4,5,6,7,-1,-1};
  
  
  for(int surf=0;surf<ACTIVE_SURFS;surf++) {
    if(rawSurfToTrigSurf[surf]>=0) {
      surfTrigBandMask[surf]=surfPtr->surfTrigBandMask[rawSurfToTrigSurf[surf]];
      for(int l1=0;l1<L1S_PER_SURF;l1++) {
	l1Scaler[surf][l1]=surfPtr->l1Scaler[rawSurfToTrigSurf[surf]][l1];
      }
      for(int l2=0;l2<L2S_PER_SURF;l2++) {
	l2Scaler[surf][l2]=surfPtr->l2Scaler[rawSurfToTrigSurf[surf]][l2];
      }
      for(int i=0;i<12;i++) {
	scaler[surf][i]=surfPtr->scaler[rawSurfToTrigSurf[surf]][i];
	threshold[surf][i]=surfPtr->threshold[rawSurfToTrigSurf[surf]][i];
	setThreshold[surf][i]=surfPtr->setThreshold[rawSurfToTrigSurf[surf]][i];
      }
    }
    else {
      surfTrigBandMask[surf]=0;
      for(int l1=0;l1<L1S_PER_SURF;l1++) {
	l1Scaler[surf][l1]=0;
      }
      for(int l2=0;l2<L2S_PER_SURF;l2++) {
	l2Scaler[surf][l2]=0;
      }
      for(int i=0;i<12;i++) {
	scaler[surf][i]=0;
	threshold[surf][i]=0;
	setThreshold[surf][i]=0;
      }
    }          
  }
  memcpy(rfPower,surfPtr->rfPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  intFlag=0;
}



SurfHk::SurfHk(Int_t trun, Int_t trealTime, FullSurfHkStructVer40_t *surfPtr)
{
  if(surfPtr->gHdr.code!=PACKET_SURF_HK ||
     surfPtr->gHdr.verId!=VER_SURF_HK ||
     surfPtr->gHdr.numBytes!=sizeof(FullSurfHkStruct_t)) {
     std::cerr << "Mismatched packet\t" << packetCodeAsString(PACKET_SURF_HK)
	 
	       << "\ncode:\t" << (int)surfPtr->gHdr.code << "\t" << PACKET_SURF_HK 
	       << "\nversion:\t" << (int)surfPtr->gHdr.verId 
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
  memcpy(scalerGoals,surfPtr->scalerGoals,sizeof(UShort_t)*NUM_ANTENNA_RINGS);
  memcpy(upperWords,surfPtr->upperWords,sizeof(UShort_t)*ACTIVE_SURFS);
  
  for(int surf=0;surf<ACTIVE_SURFS;surf++) {
    for(int l1=0;l1<L1S_PER_SURF;l1++) {
      l1Scaler[surf][l1]=surfPtr->l1Scaler[surf][l1];
    }
    for(int i=0;i<12;i++) {
      scaler[surf][i]=surfPtr->scaler[surf][i];
      threshold[surf][i]=surfPtr->threshold[surf][i];
      setThreshold[surf][i]=surfPtr->setThreshold[surf][i];
    }
  }
  memcpy(rfPower,surfPtr->rfPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(surfTrigBandMask,surfPtr->surfTrigBandMask,sizeof(UShort_t)*ACTIVE_SURFS);
  intFlag=0;
}



SurfHk::SurfHk(Int_t trun, Int_t trealTime, FullSurfHkStructVer30_t *surfPtr)
{
  if(surfPtr->gHdr.code!=PACKET_SURF_HK ||
     surfPtr->gHdr.verId!=VER_SURF_HK ||
     surfPtr->gHdr.numBytes!=sizeof(FullSurfHkStruct_t)) {
     std::cerr << "Mismatched packet\t" << packetCodeAsString(PACKET_SURF_HK)
	 
	       << "\ncode:\t" << (int)surfPtr->gHdr.code << "\t" << PACKET_SURF_HK 
	       << "\nversion:\t" << (int)surfPtr->gHdr.verId 
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
  memcpy(scaler,surfPtr->scaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF_V30);
  memcpy(threshold,surfPtr->threshold,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF_V30);
  memcpy(setThreshold,surfPtr->setThreshold,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF_V30);
  memcpy(rfPower,surfPtr->rfPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(surfTrigBandMask,surfPtr->surfTrigBandMask,sizeof(UShort_t)*ACTIVE_SURFS);
  intFlag=0;
}


SurfHk::SurfHk(Int_t trun, Int_t trealTime, FullSurfHkStructVer14_t *surfPtr)
{
  if(surfPtr->gHdr.code!=PACKET_SURF_HK ||
     surfPtr->gHdr.verId!=14 ||
     surfPtr->gHdr.numBytes!=sizeof(FullSurfHkStructVer14_t)) {
     std::cerr << "Mismatched packet\t" << packetCodeAsString(PACKET_SURF_HK)
	 
	       << "\ncode:\t" << (int)surfPtr->gHdr.code << "\t" << PACKET_SURF_HK 
	       << "\nversion:\t" << (int)surfPtr->gHdr.verId 
	       << "\t" << 14
	       << "\nsize:\t" << surfPtr->gHdr.numBytes << "\t"
	       << sizeof(FullSurfHkStructVer14_t) << std::endl;
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

SurfHk::SurfHk(Int_t trun, Int_t trealTime, FullSurfHkStructVer13_t *surfPtr)
{
  if(surfPtr->gHdr.code!=PACKET_SURF_HK ||
     surfPtr->gHdr.verId!=13 ||
     surfPtr->gHdr.numBytes!=sizeof(FullSurfHkStructVer13_t)) {
     std::cerr << "Mismatched packet\t" << packetCodeAsString(PACKET_SURF_HK)
	 
	       << "\ncode:\t" << (int)surfPtr->gHdr.code << "\t" << PACKET_SURF_HK 
	       << "\nversion:\t" << (int)surfPtr->gHdr.verId 
	       << "\t" << 13
	       << "\nsize:\t" << surfPtr->gHdr.numBytes << "\t"
	       << sizeof(FullSurfHkStructVer13_t) << std::endl;
  }
     

  run=trun;
  realTime=trealTime;
  payloadTime=surfPtr->unixTime;
  payloadTimeUs=surfPtr->unixTimeUs;
  globalThreshold=surfPtr->globalThreshold;
  errorFlag=surfPtr->errorFlag;
  memcpy(scalerGoals,surfPtr->scalerGoals,sizeof(UShort_t)*BANDS_PER_ANT);
  for(int band=0;band<BANDS_PER_ANT;band++)
     scalerGoalsNadir[band]=scalerGoals[band];
  memcpy(upperWords,surfPtr->upperWords,sizeof(UShort_t)*ACTIVE_SURFS);
  memcpy(scaler,surfPtr->scaler,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(threshold,surfPtr->threshold,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(setThreshold,surfPtr->setThreshold,sizeof(UShort_t)*ACTIVE_SURFS*SCALERS_PER_SURF);
  memcpy(rfPower,surfPtr->rfPower,sizeof(UShort_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
  memcpy(surfTrigBandMask,surfPtr->surfTrigBandMask,sizeof(UShort_t)*ACTIVE_SURFS);
  intFlag=0;
}

SurfHk::SurfHk(Int_t trun, Int_t trealTime, FullSurfHkStructVer12_t *surfPtr)
{
  if(surfPtr->gHdr.code!=PACKET_SURF_HK ||
     surfPtr->gHdr.verId!=12 ||
     surfPtr->gHdr.numBytes!=sizeof(FullSurfHkStructVer12_t)) {
     std::cerr << "Mismatched packet\t" << packetCodeAsString(PACKET_SURF_HK)
	 
	       << "\ncode:\t" << (int)surfPtr->gHdr.code << "\t" << PACKET_SURF_HK 
	       << "\nversion:\t" << (int)surfPtr->gHdr.verId 
	       << "\t" << 12 
	       << "\nsize:\t" << surfPtr->gHdr.numBytes << "\t"
	       << sizeof(FullSurfHkStructVer12_t) << std::endl;
  }
     

  run=trun;
  realTime=trealTime;
  payloadTime=surfPtr->unixTime;
  payloadTimeUs=surfPtr->unixTimeUs;
  globalThreshold=surfPtr->globalThreshold;
  errorFlag=surfPtr->errorFlag;
  for(int band=0;band<BANDS_PER_ANT;band++) {
     scalerGoalsNadir[band]=surfPtr->scalerGoal;
     scalerGoals[band]=surfPtr->scalerGoal;
  }
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

Int_t SurfHk::getL1Scaler(int phi, AnitaPol::AnitaPol_t pol, AnitaRing::AnitaRing_t ring) 
{
  Int_t surf, l1Scl;

  if (AnitaVersion::get() == 4) 
    AnitaGeomTool::getSurfL1TriggerChanFromPhiRing(phi,ring,surf,l1Scl);
  else 
    AnitaGeomTool::getSurfL1TriggerChanFromPhiPol(phi,pol,surf,l1Scl);

  if((surf>=0 && surf<ACTIVE_SURFS) && (l1Scl>=0 && l1Scl<L1S_PER_SURF)) {
    return l1Scaler[surf][l1Scl];
  }
  return -1;
}


Int_t SurfHk::getL2Scaler(int phi) 
{
  if (AnitaVersion::get() == 3) return -1; 

  Int_t surf, l2Scl;
  AnitaGeomTool::getSurfL2TriggerChanFromPhi(phi,surf,l2Scl);
  if((surf>=0 && surf<ACTIVE_SURFS) && (l2Scl>=0 && l2Scl<L2S_PER_SURF)) {
    return l2Scaler[surf][l2Scl];
  }
  return -1;
}



Int_t SurfHk::getScaler(int phi, AnitaRing::AnitaRing_t ring, AnitaTrigPol::AnitaTrigPol_t pol)
{
   Int_t surf,scl;
   AnitaGeomTool::getSurfChanTriggerFromPhiRingPol(phi,ring,pol,surf,scl);
   if((surf>=0 && surf<ACTIVE_SURFS) && (scl>=0 && scl<SCALERS_PER_SURF))
     return scaler[surf][scl];
   return -1;
}

Int_t SurfHk::getThreshold(int phi, AnitaRing::AnitaRing_t ring, AnitaTrigPol::AnitaTrigPol_t pol)
{
  Int_t surf,scl;
  AnitaGeomTool::getSurfChanTriggerFromPhiRingPol(phi,ring,pol,surf,scl);
  if((surf>=0 && surf<ACTIVE_SURFS) && (scl>=0 && scl<SCALERS_PER_SURF))
    return threshold[surf][scl];
  return -1;

}

Int_t SurfHk::getSetThreshold(int phi, AnitaRing::AnitaRing_t ring, AnitaTrigPol::AnitaTrigPol_t pol)
{
  Int_t surf,scl;
  AnitaGeomTool::getSurfChanTriggerFromPhiRingPol(phi,ring,pol,surf,scl);
  if((surf>=0 && surf<ACTIVE_SURFS) && (scl>=0 && scl<SCALERS_PER_SURF))
    return setThreshold[surf][scl];
  return -1;
}


Int_t SurfHk::isMasked(int phi, AnitaRing::AnitaRing_t ring, AnitaTrigPol::AnitaTrigPol_t pol)
{
  Int_t surf,scl;
  AnitaGeomTool::getSurfChanTriggerFromPhiRingPol(phi,ring,pol,surf,scl);
  if((surf>=0 && surf<ACTIVE_SURFS) && (scl>=0 && scl<SCALERS_PER_SURF))
    return isBandMasked(surf,scl);
  return -1;
}

Int_t SurfHk::getLogicalIndex(int phi, AnitaRing::AnitaRing_t ring, AnitaTrigPol::AnitaTrigPol_t pol)
{
   Int_t surf,scl;
   AnitaGeomTool::getSurfChanTriggerFromPhiRingPol(phi,ring,pol,surf,scl);
   return scl +surf*SCALERS_PER_SURF;
}

Int_t SurfHk::getScalerGoalRing(AnitaRing::AnitaRing_t ring)
{
  return scalerGoals[ring];
}

Int_t SurfHk::getScalerGoal(int surf, int scl)
{
  Int_t phi=-1;
  AnitaRing::AnitaRing_t ring=AnitaRing::kTopRing;
  AnitaTrigPol::AnitaTrigPol_t pol=AnitaTrigPol::kLCP;
  
  AnitaGeomTool::getPhiRingPolFromSurfChanTrigger(surf,scl,phi,ring,pol);  
  return getScalerGoalRing(ring);
}

Double_t SurfHk::getRFPowerInK(int surf, int chan)
{
  if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
  if(chan<0 || chan>=RFCHAN_PER_SURF)
    return -1;
  Int_t adc=rfPower[surf][chan]&0x7fff
;
  if(adc==0) return 290;

  Double_t kelvin=AnitaEventCalibrator::Instance()->convertRfPowToKelvin(surf,chan,adc);
  if(TMath::IsNaN(kelvin)) return 290;
  if(kelvin<0) return 290;
  if(kelvin>1e6) return 290;
  return kelvin;
}


// Double_t SurfHk::getMeasuredRFPowerInK(int surf, int chan)
// {
//   if(surf<0 || surf>=ACTIVE_SURFS)
//     return -1;
//   if(chan<0 || chan>=RFCHAN_PER_SURF)
//     return -1;
//   Int_t adc=rfPower[surf][chan];
//   Double_t kelvin=AnitaEventCalibrator::Instance()->convertRfPowToKelvinMeasured(surf,chan,adc);
//   return kelvin;
// }

Double_t SurfHk::getRawRFPower(int surf, int chan)
{
  if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
  if(chan<0 || chan>=RFCHAN_PER_SURF)
    return -1;
  UShort_t adc=rfPower[surf][chan]&0x7FFF; //mask off top bit
  return adc;
}
  
UInt_t SurfHk::getRFPowerTimeOffset(int surf, int chan)
{

  if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
  if(chan<0 || chan>=RFCHAN_PER_SURF)
    return -1;

  int zeroPhaseChan = 0;
  UInt_t startPhase = rfPower[surf][0]&0x8000;
  for (int chani =0; chani<=RFCHAN_PER_SURF; chani++) {
    if ((rfPower[surf][chani]&0x8000) != startPhase) {
      zeroPhaseChan = chani;
      break;
    }
  }
  UInt_t PhaseOffset = 0;
  if (chan < zeroPhaseChan) {
    PhaseOffset = zeroPhaseChan - chan; }
  if (chan >= zeroPhaseChan) {
    PhaseOffset = (zeroPhaseChan+8) - chan; }
    
  return PhaseOffset;

}

UInt_t SurfHk::getRFPowerPayloadTime(UInt_t surf, UInt_t chan)
{
  if( surf>=ACTIVE_SURFS)
    return -1;
  if( chan>=RFCHAN_PER_SURF)
    return -1;

  UInt_t zeroPhaseChan = 0;
  UInt_t startPhase = rfPower[surf][0]&0x8000;
  for (UInt_t chani =0; chani<=RFCHAN_PER_SURF; chani++) {
    if ((rfPower[surf][chani]&0x8000) != startPhase) {
      zeroPhaseChan = chani;
      break;
    }
  }
  UInt_t PhaseOffset = 0;
  if (chan < zeroPhaseChan) {
    PhaseOffset = zeroPhaseChan - chan; }
  if (chan >= zeroPhaseChan) {
    PhaseOffset = (zeroPhaseChan+8) - chan;}
    
  if (payloadTimeUs < 12500*PhaseOffset) {
    return payloadTime - 1;}
  else {
    return payloadTime;}
  
}

UInt_t SurfHk::getRFPowerPayloadTimeUs(UInt_t surf, UInt_t chan)
{
  if(surf>=ACTIVE_SURFS)
    return -1;
  if(chan>=RFCHAN_PER_SURF)
    return -1;

  UInt_t zeroPhaseChan = 0;
  UInt_t startPhase = rfPower[surf][0]&0x8000;
  for (UInt_t chani=0; chani<=RFCHAN_PER_SURF; chani++) {
    if ((rfPower[surf][chani]&0x8000) != startPhase) {
      zeroPhaseChan = chani;
      break;
    }
  }
  UInt_t PhaseOffset = 0;
  if (chan < zeroPhaseChan) {
    PhaseOffset = zeroPhaseChan - chan; }
  if (chan >= zeroPhaseChan) {
    PhaseOffset = (zeroPhaseChan+8) - chan;}
    
  if (payloadTimeUs < 12500*PhaseOffset) {
    return (1e6-12500*PhaseOffset)+payloadTimeUs; }
  else {
    return payloadTimeUs+PhaseOffset; }

  return 0;
}
