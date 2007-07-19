//////////////////////////////////////////////////////////////////////////////
/////  SurfHk.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "SurfHk.h"
#include <iostream>
#include <fstream>

ClassImp(SurfHk);

SurfHk::SurfHk() 
{
   //Default Constructor
}

SurfHk::~SurfHk() {
   //Default Destructor
}

SurfHk::SurfHk(Int_t           trun,
	       UInt_t          trealTime,
	       UInt_t          tpayloadTime,
	       UInt_t          tpayloadTimeUs,
	       UShort_t        tglobalThreshold,
	       UShort_t        terrorFlag,
	       UShort_t        tscalerGoal,
	       UShort_t        tupperWords[9],
	       UShort_t        tscaler[9][32],
	       UShort_t        tthreshold[9][32],
	       UShort_t        tsetThreshold[9][32],
	       UShort_t        trfPower[9][8],
	       UShort_t        tsurfTrigBandMask[9][2],
	       Int_t           tintFlag)
{

   run=trun;
   realTime=trealTime;
   payloadTime=tpayloadTime;
   payloadTimeUs=tpayloadTimeUs;
   globalThreshold=tglobalThreshold;
   errorFlag=terrorFlag;
   scalerGoal=tscalerGoal;
   memcpy(upperWords,tupperWords,sizeof(UShort_t)*9);
   memcpy(scaler,tscaler,sizeof(UShort_t)*9*32);
   memcpy(threshold,tthreshold,sizeof(UShort_t)*9*32);
   memcpy(setThreshold,tsetThreshold,sizeof(UShort_t)*9*32);
   memcpy(rfPower,trfPower,sizeof(UShort_t)*9*8);
   memcpy(surfTrigBandMask,tsurfTrigBandMask,sizeof(UShort_t)*9*2);
   intFlag=tintFlag;

}
