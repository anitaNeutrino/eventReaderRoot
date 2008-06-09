//////////////////////////////////////////////////////////////////////////////
/////  GpsGga.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "GpsGga.h"
#include <iostream>
#include <fstream>

ClassImp(GpsGga);

GpsGga::GpsGga() 
{
   //Default Constructor
}

GpsGga::~GpsGga() {
   //Default Destructor
}

GpsGga::GpsGga(Int_t           trun,
	 UInt_t          trealTime,
	 GpsGgaStruct_t *gpsStruct)
{

   run=trun;
   realTime=trealTime;
   intFlag=0;
   payloadTime=gpsStruct->unixTime;
   payloadTimeUs=gpsStruct->unixTimeUs;
   timeOfDay=gpsStruct->timeOfDay;
   numSats=gpsStruct->numSats;
   latitude=gpsStruct->latitude;
   longitude=gpsStruct->longitude;
   altitude=gpsStruct->altitude;
   hdop=gpsStruct->hdop;
   geoidSeparation=gpsStruct->geoidSeparation;
   ageOfCalc=gpsStruct->ageOfCalc;
   posFixType=gpsStruct->posFixType;
   baseStationId=gpsStruct->baseStationId;
}
