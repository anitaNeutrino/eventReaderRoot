//////////////////////////////////////////////////////////////////////////////
/////  Adu5Pat.cxx        ANITA ADU5 PAT reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 PAT and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "Adu5Pat.h"
#include <iostream>
#include <fstream>

ClassImp(Adu5Pat);

Adu5Pat::Adu5Pat() 
{
   //Default Constructor
}

Adu5Pat::~Adu5Pat() {
   //Default Destructor
}



Adu5Pat::Adu5Pat(Int_t           trun,
		 UInt_t          trealTime,
		 GpsAdu5PatStruct_t *gpsStruct)
{

   run=trun;
   realTime=trealTime; //Need to do something with timeOfDay
   readTime=gpsStruct->unixTime;
   payloadTime=gpsStruct->unixTime;
   payloadTimeUs=gpsStruct->unixTimeUs;
   timeOfDay=gpsStruct->timeOfDay;
   latitude=gpsStruct->latitude;
   longitude=gpsStruct->longitude;
   altitude=gpsStruct->altitude;
   heading=gpsStruct->heading;
   pitch=gpsStruct->pitch;
   roll=gpsStruct->roll;
   mrms=gpsStruct->mrms;
   brms=gpsStruct->brms;
   attFlag=gpsStruct->attFlag;
   intFlag=0;
}
