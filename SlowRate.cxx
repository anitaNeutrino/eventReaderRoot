//////////////////////////////////////////////////////////////////////////////
/////  SlowRate.h        Slow Rate data structure                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing slow rate data                      /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////


#include "SlowRate.h"
#include "AnitaPacketUtil.h"
#include "AnitaEventCalibrator.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(SlowRate);

SlowRate::SlowRate() 
{
   //Default Constructor
}

SlowRate::~SlowRate() {
   //Default Destructor
}

SlowRate::SlowRate(Int_t trun, UInt_t trealTime, SlowRateFull_t *slowPtr)
{

 if(slowPtr->gHdr.code!=PACKET_SLOW_FULL ||
     slowPtr->gHdr.verId!=VER_SLOW_FULL ||
     slowPtr->gHdr.numBytes!=sizeof(SlowRateFull_t)) {
   std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_SLOW_FULL) << "\n" 
	     << "code:\t" << (int)slowPtr->gHdr.code << "\t" << PACKET_SLOW_FULL 
	     << "\nversion:\t" << (int)slowPtr->gHdr.verId 
	     << "\t" << VER_SLOW_FULL 
	     << "\nsize:\t" << slowPtr->gHdr.numBytes << "\t"
	     << sizeof(SlowRateFull_t) << std::endl;
  }

   run=trun;
   realTime=trealTime;
   payloadTime=slowPtr->unixTime;
   eventNumber=slowPtr->rf.eventNumber;

   eventRate1Min=slowPtr->rf.eventRate1Min;
   eventRate10Min=slowPtr->rf.eventRate10Min;

   latitude=slowPtr->hk.latitude;
   longitude=slowPtr->hk.longitude;
   altitude=slowPtr->hk.altitude;

   memcpy(rfPwrAvg,&(slowPtr->rf.rfPwrAvg[0][0]),sizeof(UChar_t)*ACTIVE_SURFS*RFCHAN_PER_SURF);
   memcpy(avgScalerRates,&(slowPtr->rf.avgScalerRates[0][0]),sizeof(UChar_t)*TRIGGER_SURFS*ANTS_PER_SURF);
   memcpy(rmsScalerRates,&(slowPtr->rf.rmsScalerRates[0][0]),sizeof(UChar_t)*TRIGGER_SURFS*ANTS_PER_SURF);
   memcpy(avgL1Rates,&(slowPtr->rf.avgL1Rates[0]),sizeof(UChar_t)*TRIGGER_SURFS);
   memcpy(avgL2Rates,&(slowPtr->rf.avgL2Rates[0]),sizeof(UChar_t)*PHI_SECTORS);
   memcpy(avgL3Rates,&(slowPtr->rf.avgL3Rates[0]),sizeof(UChar_t)*PHI_SECTORS);

   memcpy(temps,&(slowPtr->hk.temps[0]),sizeof(UChar_t)*4);
   memcpy(powers,&(slowPtr->hk.powers[0]),sizeof(UChar_t)*4);
}

Double_t SlowRate::getRFPowerInK(int surf, int chan)
{
  if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
  if(chan<0 || chan>=RFCHAN_PER_SURF)
    return -1;
  Int_t adc=rfPwrAvg[surf][chan];
  adc*=4;
  Double_t kelvin=AnitaEventCalibrator::Instance()->convertRfPowToKelvin(surf,chan,adc);
  return kelvin;
}


Double_t SlowRate::getMeasuredRFPowerInK(int surf, int chan)
{
  if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
  if(chan<0 || chan>=RFCHAN_PER_SURF)
    return -1;
  Int_t adc=rfPwrAvg[surf][chan];
  adc*=4;
  Double_t kelvin=AnitaEventCalibrator::Instance()->convertRfPowToKelvinMeasured(surf,chan,adc);
  return kelvin;
}

Int_t SlowRate::getAvgScaler(int surf, int ant)
{
  if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
  if(ant<0 || ant>=ANTS_PER_SURF) 
    return -1; 
  return Int_t(avgScalerRates[surf][ant])*128;

}

Int_t SlowRate::getRmsScaler(int surf, int ant)
{

 if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
 if(ant<0 || ant>=ANTS_PER_SURF) 
   return -1; 
 return Int_t(rmsScalerRates[surf][ant])*32;
}

Int_t SlowRate::getL1Rate(int surf)
{
  if(surf<0 || surf>=ACTIVE_SURFS)
    return -1;
  return Int_t(avgL1Rates[surf])*256*16;
}
 
Int_t SlowRate::getL2Rate(int phi)
{
  if(phi<0 || phi>=PHI_SECTORS)
    return -1;
  return Int_t(avgL2Rates[phi])*64;
}

Float_t SlowRate::getL3Rate(int phi)
{
  if(phi<0 || phi>=PHI_SECTORS)
    return -1;
  Float_t val=avgL3Rates[phi];
  val/=32;
  return val;
}

Float_t SlowRate::getAltitude()
{
  Float_t alt=altitude;
  if(altitude<-5000) {
    alt=65536-alt;
  }
  return alt;
}

Float_t SlowRate::getPower(int powerInd)
{
  float powerCal[4]={18.252,10.1377,20,20};
  if(powerInd<0 || powerInd>3)
    return 0;

  float adc=powers[powerInd];
  float reading=((10*adc)/255.)-5;
  float value=reading*powerCal[powerInd];
  return value;
}

Float_t SlowRate::getTemp(int tempInd)
{
  if(tempInd<0 || tempInd>3)
    return -273.15;
  float adc=temps[tempInd];
  float reading=((10*adc)/255.)-5;
  float value=(reading*100)-273.15;
  return value;
}

char *SlowRate::getPowerName(int powerInd)
{
  char *slowPowerNames[4]={"PV V","24 V","Batt I","24 I"};
  if(powerInd<0 || powerInd>3)
    return NULL;
  return slowPowerNames[powerInd];

}

char *SlowRate::getTempName(int tempInd) 
{
  char *slowTempNames[4]={"SBS","SURF","SHORT","Rad. Plate"};
  if(tempInd<0 || tempInd>3)
    return NULL;
  return slowTempNames[tempInd];
}
