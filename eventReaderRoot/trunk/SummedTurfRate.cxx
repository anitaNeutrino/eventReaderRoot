//////////////////////////////////////////////////////////////////////////////
/////  SummedTurfRate.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "SummedTurfRate.h"
#include <iostream>
#include <fstream>

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
   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   numRates=turfPtr->numRates;
   deltaT=turfPtr->deltaT;
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


Int_t SummedTurfRate::getL1Rate(int phi, int ring)
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

Int_t SummedTurfRate::getL2Rate(int phi, int ring)
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


Int_t SummedTurfRate::isPhiMasked(int phi) {
  if(phi<0 || phi>15) return -1;
  return (phiTrigMask & (1<<phi));
}

Int_t SummedTurfRate::isAntMasked(int phi, int ring)
{
  if(phi<0 || phi>15) return -1;
   switch(ring) {
   case AnitaRing::kUpperRing:
      return  (antTrigMask&(1<<(phi+16)));
   case AnitaRing::kLowerRing:
      return  (antTrigMask&(1<<phi));
   case AnitaRing::kNadirRing:
      phi/=2;
      return nadirAntTrigMask&(1<<phi);
   default:
      return -1;
   }      
}
