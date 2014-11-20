//////////////////////////////////////////////////////////////////////////////
/////  SummedTurfRate.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "SummedTurfRate.h"
#include "AnitaPacketUtil.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(SummedTurfRate);

SummedTurfRate::SummedTurfRate() 
{
   //Default Constructor
}

SummedTurfRate::~SummedTurfRate() {
   //Default Destructor
}

SummedTurfRate::SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStruct_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_SUM_TURF_RATE ||
     turfPtr->gHdr.verId!=VER_SUM_TURF_RATE ||
     turfPtr->gHdr.numBytes!=sizeof(SummedTurfRateStruct_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << turfPtr->gHdr.code << "\t" << PACKET_SUM_TURF_RATE 
	      << "\nversion:\t" << turfPtr->gHdr.verId 
	      << "\t" << VER_SUM_TURF_RATE 
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(SummedTurfRateStruct_t) << std::endl;
  }
   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   numRates=turfPtr->numRates;
   deltaT=turfPtr->deltaT;
   deadTime=turfPtr->deadTime;
   memcpy(bufferCount,turfPtr->bufferCount,sizeof(UChar_t)*4);
   //   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UInt_t)*PHI_SECTORS*2);
   //   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   //   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   for(int phi=0;phi<PHI_SECTORS;phi++) {
     l3Rates[phi]=turfPtr->l3Rates[phi][0];
     l3RatesH[phi]=turfPtr->l3Rates[phi][1];
   }
   //   memcpy(nadirL1Rates,turfPtr->nadirL1Rates,sizeof(UInt_t)*NADIR_ANTS);
   //   memcpy(nadirL2Rates,turfPtr->nadirL2Rates,sizeof(UShort_t)*NADIR_ANTS);
   //   antTrigMask=turfPtr->antTrigMask;
   phiTrigMask=turfPtr->phiTrigMask;   
   phiTrigMaskH=turfPtr->phiTrigMaskH;   
   //   nadirAntTrigMask=turfPtr->nadirAntTrigMask;
   errorFlag=turfPtr->errorFlag;
   intFlag=0;

}

SummedTurfRate::SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStructVer16_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_SUM_TURF_RATE ||
     turfPtr->gHdr.verId!=16 ||
     turfPtr->gHdr.numBytes!=sizeof(SummedTurfRateStructVer16_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_SUM_TURF_RATE) << "\n" 
	      << "code:\t" << turfPtr->gHdr.code << "\t" << PACKET_SUM_TURF_RATE 
	      << "\nversion:\t" << turfPtr->gHdr.verId 
	      << "\t" << 16
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(SummedTurfRateStructVer16_t) << std::endl;
  }
   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   numRates=turfPtr->numRates;
   deltaT=turfPtr->deltaT;
   deadTime=turfPtr->deadTime;
   memcpy(bufferCount,turfPtr->bufferCount,sizeof(UChar_t)*4);
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UInt_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(nadirL1Rates,turfPtr->nadirL1Rates,sizeof(UInt_t)*NADIR_ANTS);
   memcpy(nadirL2Rates,turfPtr->nadirL2Rates,sizeof(UShort_t)*NADIR_ANTS);
   antTrigMask=turfPtr->antTrigMask;
   phiTrigMask=turfPtr->phiTrigMask;   
   nadirAntTrigMask=turfPtr->nadirAntTrigMask;
   errorFlag=turfPtr->errorFlag;
   intFlag=0;

}

SummedTurfRate::SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStructVer15_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_SUM_TURF_RATE ||
     turfPtr->gHdr.verId!=15 ||
    turfPtr->gHdr.numBytes!=sizeof(SummedTurfRateStructVer15_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_SUM_TURF_RATE) << "\n" 
	      << "code:\t" << turfPtr->gHdr.code << "\t" << PACKET_SUM_TURF_RATE 
	      << "\nversion:\t" << turfPtr->gHdr.verId 
	      << "\t" << 15
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(SummedTurfRateStructVer15_t) << std::endl;
  }
   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   numRates=turfPtr->numRates;
   deltaT=turfPtr->deltaT;
   deadTime=0;
   memset(bufferCount,0,sizeof(UChar_t)*4);
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UInt_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(nadirL1Rates,turfPtr->nadirL1Rates,sizeof(UInt_t)*NADIR_ANTS);
   memcpy(nadirL2Rates,turfPtr->nadirL2Rates,sizeof(UShort_t)*NADIR_ANTS);
   antTrigMask=turfPtr->antTrigMask;
   phiTrigMask=turfPtr->phiTrigMask;   
   nadirAntTrigMask=turfPtr->nadirAntTrigMask;
   errorFlag=turfPtr->errorFlag;
   intFlag=0;

}


SummedTurfRate::SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStructVer14_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_SUM_TURF_RATE ||
     turfPtr->gHdr.verId!=14 ||
    turfPtr->gHdr.numBytes!=sizeof(SummedTurfRateStructVer14_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_SUM_TURF_RATE) << "\n" 
	      << "code:\t" << turfPtr->gHdr.code << "\t" << PACKET_SUM_TURF_RATE 
	      << "\nversion:\t" << turfPtr->gHdr.verId 
	      << "\t" << 14
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(SummedTurfRateStructVer14_t) << std::endl;
  }
   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   numRates=turfPtr->numRates;
   deltaT=turfPtr->deltaT;
   deadTime=0;
   memset(bufferCount,0,sizeof(UChar_t)*4);
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UInt_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UShort_t)*PHI_SECTORS);
   memset(nadirL1Rates,0,sizeof(UInt_t)*NADIR_ANTS);
   memset(nadirL2Rates,0,sizeof(UShort_t)*NADIR_ANTS);
   antTrigMask=turfPtr->antTrigMask;
   phiTrigMask=0;
   nadirAntTrigMask=turfPtr->nadirAntTrigMask;
   errorFlag=0;
   intFlag=0;

}


SummedTurfRate::SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStructVer11_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_SUM_TURF_RATE ||
     turfPtr->gHdr.verId!=11 ||
    turfPtr->gHdr.numBytes!=sizeof(SummedTurfRateStructVer11_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_SUM_TURF_RATE) << "\n" 
	      << "code:\t" << turfPtr->gHdr.code << "\t" << PACKET_SUM_TURF_RATE 
	      << "\nversion:\t" << turfPtr->gHdr.verId 
	      << "\t" << 11
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(SummedTurfRateStructVer11_t) << std::endl;
  }
   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   numRates=turfPtr->numRates;
   deltaT=turfPtr->deltaT;
   deadTime=0;
   memset(bufferCount,0,sizeof(UChar_t)*4);
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UInt_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UShort_t)*PHI_SECTORS);
   memset(nadirL1Rates,0,sizeof(UInt_t)*NADIR_ANTS);
   memset(nadirL2Rates,0,sizeof(UShort_t)*NADIR_ANTS);
   antTrigMask=turfPtr->antTrigMask;
   phiTrigMask=0;
   nadirAntTrigMask=turfPtr->nadirAntTrigMask;
   errorFlag=0;
   intFlag=0;
}


SummedTurfRate::SummedTurfRate(Int_t trun, Int_t trealTime, SummedTurfRateStructVer10_t *turfPtr)
{

 if(turfPtr->gHdr.code!=PACKET_SUM_TURF_RATE ||
     turfPtr->gHdr.verId!=10 ||
    turfPtr->gHdr.numBytes!=sizeof(SummedTurfRateStructVer10_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_SUM_TURF_RATE) << "\n" 
	      << "code:\t" << turfPtr->gHdr.code << "\t" << PACKET_SUM_TURF_RATE 
	      << "\nversion:\t" << turfPtr->gHdr.verId 
	      << "\t" << 10
	      << "\nsize:\t" << turfPtr->gHdr.numBytes << "\t"
	      << sizeof(SummedTurfRateStructVer10_t) << std::endl;
  }
   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   numRates=turfPtr->numRates;
   deltaT=turfPtr->deltaT;
   deadTime=0;
   memset(bufferCount,0,sizeof(UChar_t)*4);
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UInt_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UShort_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UShort_t)*PHI_SECTORS);
   memset(nadirL1Rates,0,sizeof(UInt_t)*NADIR_ANTS);
   memset(nadirL2Rates,0,sizeof(UShort_t)*NADIR_ANTS);
   antTrigMask=0;
   phiTrigMask=0;
   nadirAntTrigMask=0;
   errorFlag=0;
   intFlag=0;
}


// Int_t SummedTurfRate::getL1Rate(int phi, int ring)
// {
//   if(phi<0 || phi>15) return -1;
//   switch(ring) {
//   case AnitaRing::kUpperRing:
//   case AnitaRing::kLowerRing:
//     return 16*l1Rates[phi][ring];
//   case AnitaRing::kNadirRing:
//     if(phi%2==0)
//       return 16*nadirL1Rates[phi/2]; //Might need to change handling of nadirs 
//   default:
//     return -1;
//   }
//   return -1;
// }

// Int_t SummedTurfRate::getL2Rate(int phi, int ring)
// { 
//   if(phi<0 || phi>15) return -1;
//   switch(ring) {
//   case AnitaRing::kUpperRing:
//     return 64*upperL2Rates[phi];
//   case AnitaRing::kLowerRing:
//     return 64*lowerL2Rates[phi];
//   case AnitaRing::kNadirRing:
//     if(phi%2==0)
//       return 64*nadirL2Rates[phi/2];
//   default:
//     return -1;
//   }
//   return -1;
// }

// Int_t SummedTurfRate::getNadirL12Rate(int phi) {
//   if(phi<0 || phi>15) return -1;
//   if(phi%2==0)
//     return 16*nadirL1Rates[phi/2];
//   else
//     return 64*nadirL2Rates[phi/2];
// }

Int_t SummedTurfRate::isPhiMasked(int phi, AnitaPol::AnitaPol_t pol) {
  if(phi<0 || phi>15) return -1;
  if(pol==AnitaPol::kVertical)
    return (phiTrigMask & (1<<phi));
  return (phiTrigMaskH & (1<<phi));
}

Int_t SummedTurfRate::isAntMasked(int phi, int ring)
{
  if(phi<0 || phi>15) return -1;
  return 0;
}
