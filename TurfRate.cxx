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



TurfRate::TurfRate(Int_t           trun,
		   UInt_t          trealTime,
		   UInt_t          tpayloadTime,
		   UInt_t          tpayloadTimeUs,
		   UShort_t        tl1Rates[8][4],
		   UChar_t         tupperL2Rates[16],
		   UChar_t         tlowerL2Rates[16],
		   UChar_t         tl3Rates[16],
		   Int_t           tintFlag)
{
   run=trun;
   realTime=trealTime;
   payloadTime=tpayloadTime;
   payloadTimeUs=tpayloadTimeUs;
   memcpy(l1Rates,tl1Rates,sizeof(UShort_t)*8*4);
   memcpy(upperL2Rates,tupperL2Rates,sizeof(UShort_t)*16);
   memcpy(lowerL2Rates,tlowerL2Rates,sizeof(UShort_t)*16);
   memcpy(l3Rates,tl3Rates,sizeof(UShort_t)*16);
   intFlag=tintFlag;

}
