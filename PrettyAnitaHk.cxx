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
#include <cstring>

ClassImp(PrettyAnitaHk);

PrettyAnitaHk::PrettyAnitaHk() 
{
   //Default Constructor
}

PrettyAnitaHk::~PrettyAnitaHk() {
   //Default Destructor
}

PrettyAnitaHk::PrettyAnitaHk(CalibratedHk *calHk, Int_t tintFlag)
{
  run=calHk->run;
  realTime=calHk->realTime;
  payloadTime=calHk->payloadTime;
  payloadTimeUs=calHk->payloadTimeUs;
  magnetometer[0]=calHk->magX;
  magnetometer[1]=calHk->magY;
  magnetometer[2]=calHk->magZ;

  for(int i=0;i<NUM_INT_TEMPS;i++) {
    intTemps[i]=calHk->getInternalTemp(i);
  }
  for(int i=0;i<NUM_SBS_TEMPS;i++) {
    intTemps[NUM_INT_TEMPS+i]=calHk->getSBSTemp(i);
  }
  for(int i=0;i<NUM_EXT_TEMPS;i++) {
    extTemps[i]=calHk->getExternalTemp(i);
  }
  for(int i=0;i<NUM_VOLTAGES;i++) {
    voltages[i]=calHk->getVoltage(i);
  }
  for(int i=0;i<NUM_CURRENTS;i++) {
    currents[i]=calHk->getCurrent(i);
  }
  for(int i=0;i<NUM_PRESSURES;i++) {
    pressures[i]=calHk->getPressure(i);
  }
  for(int i=0;i<NUM_ACCELEROMETERS;i++) {
    for(int j=0;j<4;j++) {
      accelerometer[i][j]=calHk->getAccelerometer(i,j);
    }
  }
  for(int i=0;i<NUM_SUNSENSORS;i++) {
    for(int j=0;j<5;j++) {
      rawSunSensor[i][j]=calHk->getRawSunsensor(i,j);
    }
    Float_t tempSS=0,tempSSX=0,tempSSY=0;
    calHk->getSSMagnitude(i,&tempSS,&tempSSX,&tempSSY);
    ssMag[i][0]=tempSSX;
    ssMag[i][1]=tempSSY;

    Float_t tempSSAz=0,tempSSAzRel=0,tempSSEl=0,tempSSPos[3]={0};
    ssGoodFlag[i]=calHk->getFancySS(i,tempSSPos,&tempSSAz,&tempSSEl,&tempSSAzRel);
    ssElevation[i]=tempSSEl;
    ssAzimuth[i]=tempSSAz;
    ssAzimuthAdu5[i]=tempSSAzRel;
  }
  intFlag=tintFlag;
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
