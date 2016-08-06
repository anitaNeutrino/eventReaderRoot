//////////////////////////////////////////////////////////////////////////////
/////  Adu5Vtg.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "Adu5Vtg.h"
#include "AnitaPacketUtil.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(Adu5Vtg);

Adu5Vtg::Adu5Vtg() 
{
   //Default Constructor
}

Adu5Vtg::~Adu5Vtg() {
   //Default Destructor
}

Adu5Vtg::Adu5Vtg(Int_t           trun,
		 UInt_t          trealTime,
		 GpsAdu5VtgStruct_t *gpsStruct)
{
  simplePacketCheck(&(gpsStruct->gHdr),PACKET_GPS_ADU5_VTG);
  run=trun;
  realTime=trealTime;
  payloadTime=gpsStruct->unixTime;
  payloadTimeUs=gpsStruct->unixTimeUs;
  trueCourse=gpsStruct->trueCourse;
  magneticCourse=gpsStruct->magneticCourse;
  speedInKnots=gpsStruct->speedInKnots;
  speedInKPH=gpsStruct->speedInKPH;
  intFlag=0;
}
