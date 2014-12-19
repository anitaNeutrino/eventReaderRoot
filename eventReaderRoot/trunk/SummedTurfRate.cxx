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
   for(int phi=0;phi<PHI_SECTORS;phi++) {
     l3Rates[phi]=turfPtr->l3Rates[phi][0];
     l3RatesH[phi]=turfPtr->l3Rates[phi][1];
   }
   phiTrigMask=turfPtr->phiTrigMask;   
   phiTrigMaskH=turfPtr->phiTrigMaskH;   
   l1TrigMask=turfPtr->l1TrigMask;   
   l1TrigMaskH=turfPtr->l1TrigMaskH;   
   errorFlag=turfPtr->errorFlag;
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
    return ((phiTrigMask & (1<<phi))?1:0);
  return ((phiTrigMaskH & (1<<phi))?1:0);
}


Int_t SummedTurfRate::isL1Masked(int phi, AnitaPol::AnitaPol_t pol) {
  if(phi<0 || phi>15) return -1;
  if(pol==AnitaPol::kVertical)
    return ((l1TrigMask & (1<<phi))?1:0);
  return ((l1TrigMaskH & (1<<phi))?1:0);
}
