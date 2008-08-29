//////////////////////////////////////////////////////////////////////////////
/////  PrettyAnitaHk.cxx        ANITA hk reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in pretty ANITA hks and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "PrettyAnitaHk.h"
#include <iostream>
#include <fstream>

ClassImp(PrettyAnitaHk);

PrettyAnitaHk::PrettyAnitaHk() 
{
   //Default Constructor
}

PrettyAnitaHk::~PrettyAnitaHk() {
   //Default Destructor
}



PrettyAnitaHk::PrettyAnitaHk(Int_t           trun,
			     UInt_t          trealTime,
			     UInt_t          tpayloadTime,
			     UInt_t          tpayloadTimeUs,
			     Float_t         *tintTemps,
			     Float_t         *textTemps,
			     Float_t         *tvoltages,
			     Float_t         *tcurrents,
			     Float_t         *tmagnetometer,
			     Float_t         *tpressures,
			     Float_t         *taccelerometer[],
			     Float_t         *trawSunSensor[],
			     Float_t         *tssMag[],
			     Float_t         *tssElevation,
			     Float_t         *tssAzimuth,
			     Float_t         *tssAzimuthAdu5,
			     Int_t           *tssGoodFlag,
			     Int_t           tintFlag)
{

   run=trun;
   realTime=trealTime;
   payloadTime=tpayloadTime;
   payloadTimeUs=tpayloadTimeUs;
   memcpy(intTemps,tintTemps,sizeof(Float_t)*19);
   memcpy(extTemps,textTemps,sizeof(Float_t)*25);
   memcpy(voltages,tvoltages,sizeof(Float_t)*11);
   memcpy(currents,tcurrents,sizeof(Float_t)*12);
   memcpy(magnetometer,tmagnetometer,sizeof(Float_t)*3);
   memcpy(pressures,tpressures,sizeof(Float_t)*2);
   memcpy(accelerometer,taccelerometer,sizeof(Float_t)*2*4);
   memcpy(rawSunSensor,trawSunSensor,sizeof(Float_t)*4*5);
   memcpy(ssMag,tssMag,sizeof(Float_t)*4*2);
   memcpy(ssElevation,tssElevation,sizeof(Float_t)*4);
   memcpy(ssAzimuth,tssAzimuth,sizeof(Float_t)*4);
   memcpy(ssAzimuthAdu5,tssAzimuthAdu5,sizeof(Float_t)*4);
   memcpy(ssGoodFlag,tssGoodFlag,sizeof(Float_t)*4);
   intFlag=tintFlag;
  
}
