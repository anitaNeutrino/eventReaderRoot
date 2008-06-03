//////////////////////////////////////////////////////////////////////////////
/////  TurfRate.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "TurfRate.h"
#include <iostream>
#include <fstream>

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
   run=trun;
   realTime=trealTime;
   payloadTime=turfPtr->unixTime;
   ppsNum=turfPtr->ppsNum;
   memcpy(l1Rates,turfPtr->l1Rates,sizeof(UShort_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,turfPtr->upperL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,turfPtr->lowerL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(l3Rates,turfPtr->l3Rates,sizeof(UChar_t)*PHI_SECTORS);
   antTrigMask=turfPtr->antTrigMask;
   nadirAntTrigMask=turfPtr->nadirAntTrigMask;
   intFlag=0;

}



TurfRate::TurfRate(Int_t           trun,
		   UInt_t          trealTime,
		   UInt_t          tpayloadTime,
		   UInt_t          tppsNum,
		   UShort_t        tl1Rates[PHI_SECTORS][2],
		   UChar_t         tupperL2Rates[PHI_SECTORS],
		   UChar_t         tlowerL2Rates[PHI_SECTORS],
		   UChar_t         tl3Rates[PHI_SECTORS],
		   UInt_t          tantTrigMask,
		   UChar_t         tnadirAntTrigMask,
		   Int_t           tintFlag)
{
   run=trun;
   realTime=trealTime;
   payloadTime=tpayloadTime;
   ppsNum=tppsNum;
   memcpy(l1Rates,tl1Rates,sizeof(UShort_t)*PHI_SECTORS*2);
   memcpy(upperL2Rates,tupperL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(lowerL2Rates,tlowerL2Rates,sizeof(UChar_t)*PHI_SECTORS);
   memcpy(l3Rates,tl3Rates,sizeof(UChar_t)*PHI_SECTORS);
   antTrigMask=tantTrigMask;
   nadirAntTrigMask=tnadirAntTrigMask;
   intFlag=tintFlag;

}

Int_t TurfRate::isAntMasked(int phi, int ring)
{
   switch(ring) {
   case AnitaRing::kUpperRing:
      return  (antTrigMask&(1<<(phi+16)));
   case AnitaRing::kLowerRing:
      return  (antTrigMask&(1<<phi));
   case AnitaRing::kNadirRing:
      phi/=2;
      return nadirAntTrigMask&(1<<phi);
   default:
      return 0;
   }
      
}
