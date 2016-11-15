//////////////////////////////////////////////////////////////////////////////
/////  TurfRate.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "TurfRate.h"
#include "AnitaPacketUtil.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(TurfRate);

TurfRate::TurfRate() 
{
   //Default Constructor
}

TurfRate::~TurfRate() {
   //Default Destructor
}

TurfRate::TurfRate(Int_t trun, Int_t trealTime, TurfRateStruct_t *turfPtr)
{
  simplePacketCheck(&(turfPtr->gHdr),PACKET_TURF_RATE);

   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   ppsNum=turfPtr->ppsNum;   
   deadTime=turfPtr->deadTime;
   c3poNum=turfPtr->c3poNum;
   //   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   //   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   for(int i=0;i<PHI_SECTORS;i++) {
     l3Rates[i]=turfPtr->l3Rates[i];
     l3RatesH[i]=0;
     l1Rates[i][0]=turfPtr->l1Rates[i];
     l1Rates[i][1]=0;
     l3RatesGated[i]=turfPtr->l3Rates[i];
   }
   rfScaler=turfPtr->rfScaler;
   refPulses=turfPtr->refPulses;
   l1TrigMask=turfPtr->l1TrigMask;
   l1TrigMaskH=0;
   phiTrigMask=turfPtr->phiTrigMask;
   phiTrigMaskH=0;
   errorFlag=turfPtr->errorFlag;
   intFlag=0;
}



TurfRate::TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer40_t *turfPtr)
{
  simplePacketCheck(&(turfPtr->gHdr),PACKET_TURF_RATE);

   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   ppsNum=turfPtr->ppsNum;   
   deadTime=turfPtr->deadTime;   
   //   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   //   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   for(int i=0;i<PHI_SECTORS;i++) {
     l3Rates[i]=turfPtr->l3Rates[i][0];
     l3RatesH[i]=turfPtr->l3Rates[i][1];
     l1Rates[i][0]=turfPtr->l1Rates[i][0];
     l1Rates[i][1]=turfPtr->l1Rates[i][1];
   }
   l1TrigMask=turfPtr->l1TrigMask;
   l1TrigMaskH=turfPtr->l1TrigMaskH;
   phiTrigMask=turfPtr->phiTrigMask;
   phiTrigMaskH=turfPtr->phiTrigMaskH;
   errorFlag=turfPtr->errorFlag;
   intFlag=0;
}


TurfRate::TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer34_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_TURF_RATE ||
     turfPtr->gHdr.verId!=VER_TURF_RATE ||
     turfPtr->gHdr.numBytes!=sizeof(TurfRateStruct_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_TURF_RATE) << "\n" 
	      << "code:\t" << (int)turfPtr->gHdr.code << "\t" << PACKET_TURF_RATE 
	      << "\nversion:\t" << (int)turfPtr->gHdr.verId 
	      << "\t" << VER_TURF_RATE 
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(TurfRateStruct_t) << std::endl;
  }

   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   ppsNum=turfPtr->ppsNum;   
   deadTime=turfPtr->deadTime;   
   //   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UShort_t)*PHI_SECTORS*2);
   //   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   //   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   for(int i=0;i<PHI_SECTORS;i++) {
     l3Rates[i]=turfPtr->l3Rates[i][0];
     l3RatesH[i]=turfPtr->l3Rates[i][1];
   }
   l1TrigMask=turfPtr->l1TrigMask;
   l1TrigMaskH=turfPtr->l1TrigMaskH;
   phiTrigMask=turfPtr->phiTrigMask;
   phiTrigMaskH=turfPtr->phiTrigMaskH;
   errorFlag=turfPtr->errorFlag;
   intFlag=0;

}


TurfRate::TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer16_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_TURF_RATE ||
     turfPtr->gHdr.verId!=16 ||
     turfPtr->gHdr.numBytes!=sizeof(TurfRateStructVer16_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_TURF_RATE) << "\n" 
	      << "code:\t" << (int)turfPtr->gHdr.code << "\t" << PACKET_TURF_RATE 
	      << "\nversion:\t" << (int)turfPtr->gHdr.verId 
	      << "\t" << 16
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(TurfRateStructVer16_t) << std::endl;
  }

   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   ppsNum=turfPtr->ppsNum;   
   deadTime=turfPtr->deadTime;   
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UShort_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(nadirL1Rates,turfPtr->nadirL1Rates,sizeof(UShort_t)*NADIR_ANTS);
   memcpy(nadirL2Rates,turfPtr->nadirL2Rates,sizeof(UChar_t)*NADIR_ANTS);
   antTrigMask=turfPtr->antTrigMask;
   nadirAntTrigMask=turfPtr->nadirAntTrigMask;
   phiTrigMask=turfPtr->phiTrigMask;
   phiTrigMaskH=0;
   errorFlag=turfPtr->errorFlag;
   intFlag=0;

}

TurfRate::TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer15_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_TURF_RATE ||
     turfPtr->gHdr.verId!=15 ||
     turfPtr->gHdr.numBytes!=sizeof(TurfRateStructVer15_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_TURF_RATE) << "\n" 
	      << "code:\t" << (int)turfPtr->gHdr.code << "\t" << PACKET_TURF_RATE 
	      << "\nversion:\t" << (int)turfPtr->gHdr.verId 
	      << "\t" << 15
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(TurfRateStructVer15_t) << std::endl;
  }

   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   ppsNum=turfPtr->ppsNum;   
   deadTime=0;
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UShort_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(nadirL1Rates,turfPtr->nadirL1Rates,sizeof(UShort_t)*NADIR_ANTS);
   memcpy(nadirL2Rates,turfPtr->nadirL2Rates,sizeof(UChar_t)*NADIR_ANTS);
   antTrigMask=turfPtr->antTrigMask;
   nadirAntTrigMask=turfPtr->nadirAntTrigMask;
   phiTrigMask=turfPtr->phiTrigMask;
   phiTrigMaskH=0;
   errorFlag=turfPtr->errorFlag;
   intFlag=0;
}


TurfRate::TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer14_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_TURF_RATE ||
     turfPtr->gHdr.verId!=14 ||
     turfPtr->gHdr.numBytes!=sizeof(TurfRateStructVer14_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_TURF_RATE) << "\n" 
	      << "code:\t" << (int)turfPtr->gHdr.code << "\t" << PACKET_TURF_RATE 
	      << "\nversion:\t" << (int)turfPtr->gHdr.verId 
	      << "\t" << 14
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(TurfRateStructVer14_t) << std::endl;
  }

   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   ppsNum=turfPtr->ppsNum;   
   deadTime=0;
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UShort_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(nadirL1Rates,turfPtr->nadirL1Rates,sizeof(UShort_t)*NADIR_ANTS);
   memcpy(nadirL2Rates,turfPtr->nadirL2Rates,sizeof(UChar_t)*NADIR_ANTS);
   antTrigMask=turfPtr->antTrigMask;
   nadirAntTrigMask=turfPtr->nadirAntTrigMask;
   phiTrigMask=0;
   phiTrigMaskH=0;
   errorFlag=0;
   intFlag=0;
}

TurfRate::TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer13_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_TURF_RATE ||
     turfPtr->gHdr.verId!=13 ||
     turfPtr->gHdr.numBytes!=sizeof(TurfRateStructVer13_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_TURF_RATE) << "\n" 
	      << "code:\t" << (int)turfPtr->gHdr.code << "\t" << PACKET_TURF_RATE 
	      << "\nversion:\t" << (int)turfPtr->gHdr.verId 
	      << "\t" << 13
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(TurfRateStructVer13_t) << std::endl;
  }

   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   ppsNum=turfPtr->ppsNum;   
   deadTime=0;
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UShort_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UChar_t)*PHI_SECTORS);
   memset(nadirL1Rates,0,sizeof(UShort_t)*NADIR_ANTS);
   memset(nadirL2Rates,0,sizeof(UChar_t)*NADIR_ANTS);
   antTrigMask=turfPtr->antTrigMask;
   nadirAntTrigMask=turfPtr->nadirAntTrigMask;
   phiTrigMask=0;
   phiTrigMaskH=0;
   errorFlag=0;
   intFlag=0;
}

TurfRate::TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer12_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_TURF_RATE ||
     turfPtr->gHdr.verId!=12 ||
     turfPtr->gHdr.numBytes!=sizeof(TurfRateStructVer12_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_TURF_RATE) << "\n" 
	      << "code:\t" << (int)turfPtr->gHdr.code << "\t" << PACKET_TURF_RATE 
	      << "\nversion:\t" << (int)turfPtr->gHdr.verId 
	      << "\t" << 12
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(TurfRateStructVer12_t) << std::endl;
  }

   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   ppsNum=turfPtr->ppsNum;   
   deadTime=0;
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UShort_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UChar_t)*PHI_SECTORS);
   memset(nadirL1Rates,0,sizeof(UShort_t)*NADIR_ANTS);
   memset(nadirL2Rates,0,sizeof(UChar_t)*NADIR_ANTS);
   antTrigMask=0;
   nadirAntTrigMask=0;
   phiTrigMask=0;
   phiTrigMaskH=0;
   errorFlag=0;
   intFlag=0;
}

TurfRate::TurfRate(Int_t trun, Int_t trealTime, TurfRateStructVer11_t *turfPtr)
{
   std::cout << "L1 Rates will be all jumbled up\n";
 if(turfPtr->gHdr.code!=PACKET_TURF_RATE ||
     turfPtr->gHdr.verId!=11 ||
     turfPtr->gHdr.numBytes!=sizeof(TurfRateStructVer11_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_TURF_RATE) << "\n" 
	      << "code:\t" << (int)turfPtr->gHdr.code << "\t" << PACKET_TURF_RATE 
	      << "\nversion:\t" << (int)turfPtr->gHdr.verId 
	      << "\t" << 11
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(TurfRateStructVer11_t) << std::endl;
  }

   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   ppsNum=turfPtr->ppsNum;   
   deadTime=0;
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UShort_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UChar_t)*PHI_SECTORS);
   memset(nadirL1Rates,0,sizeof(UShort_t)*NADIR_ANTS);
   memset(nadirL2Rates,0,sizeof(UChar_t)*NADIR_ANTS);
   antTrigMask=0;
   nadirAntTrigMask=0;
   phiTrigMask=0;
   phiTrigMaskH=0;
   errorFlag=0;
   intFlag=0;
}


Int_t TurfRate::getL1Rate(int phi, int ring)
{
  if(phi<0 || phi>15) return -1;
  switch(ring) {
  case AnitaRing::kUpperRing:
  case AnitaRing::kLowerRing:
    return 16*l1Rates[phi][ring];
  case AnitaRing::kNadirRing:
    if(phi%2==0)
      return 16*nadirL1Rates[phi/2]; //Might need to change handling of nadirs 
  default:
    return -1;
  }
  return -1;
}

Int_t TurfRate::getL2Rate(int phi, int ring)
{ 
  if(phi<0 || phi>15) return -1;
  switch(ring) {
  case AnitaRing::kUpperRing:
    return 64*upperL2Rates[phi];
  case AnitaRing::kLowerRing:
    return 64*lowerL2Rates[phi];
  case AnitaRing::kNadirRing:
    if(phi%2==0)
      return 64*nadirL2Rates[phi/2];
  default:
    return -1;
  }
  return -1;
}

Int_t TurfRate::getNadirL12Rate(int phi) {
  if(phi<0 || phi>15) return -1;
  if(phi%2==0)
    return 16*nadirL1Rates[phi/2];
  else
    return 64*nadirL2Rates[phi/2];
}

//((l3TrigPattern&(1<<(phi))) ? 1 :0);

Int_t TurfRate::isPhiMasked(int phi) {
  if(phi<0 || phi>15) return -1;
  return ((phiTrigMask & (1<<phi)) ? 1 :0);
}
Int_t TurfRate::isPhiMaskedHPol(int phi) {
  if(phi<0 || phi>15) return -1;
  return ((phiTrigMaskH & (1<<phi)) ? 1:0);
}


Int_t TurfRate::isL1Masked(int phi) {
  if(phi<0 || phi>15) return -1;
  return ((l1TrigMask & (1<<phi)) ? 1 :0);
}
Int_t TurfRate::isL1MaskedHPol(int phi) {
  if(phi<0 || phi>15) return -1;
  return ((l1TrigMaskH & (1<<phi)) ? 1:0);
}

Int_t TurfRate::isAntMasked(int phi, int ring)
{
  if(phi<0 || phi>15) return -1;
   switch(ring) {
   case AnitaRing::kUpperRing:
     return  ((antTrigMask&(1<<(phi)))?1:0);
   case AnitaRing::kLowerRing:
     return  ((antTrigMask&(1<<(phi+16)))?1:0);
   case AnitaRing::kNadirRing:
      phi/=2;
      return nadirAntTrigMask&(1<<phi);
   default:
      return -1;
   }      
}
