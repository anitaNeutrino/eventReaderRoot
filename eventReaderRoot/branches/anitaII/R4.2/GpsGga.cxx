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

 if(gpsStruct->gHdr.code&BASE_PACKET_MASK!=PACKET_GPS_GGA ||
     gpsStruct->gHdr.verId!=VER_GPS_GGA ||
     gpsStruct->gHdr.numBytes!=sizeof(GpsGgaStruct_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << gpsStruct->gHdr.code << "\t" << PACKET_GPS_GGA 
	      << "\nversion:\t" << gpsStruct->gHdr.verId 
	      << "\t" << VER_GPS_GGA 
	      << "\nsize:\t" << gpsStruct->gHdr.numBytes << "\t"
	      << sizeof(GpsGgaStruct_t) << std::endl;
  }
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
