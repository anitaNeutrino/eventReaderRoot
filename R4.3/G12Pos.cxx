//////////////////////////////////////////////////////////////////////////////
/////  G12Pos.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "G12Pos.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(G12Pos);

G12Pos::G12Pos() 
{
   //Default Constructor
}

G12Pos::~G12Pos() {
   //Default Destructor
}

G12Pos::G12Pos(Int_t           trun,
	 UInt_t          trealTime,
	 GpsG12PosStruct_t *gpsStruct)
{

 if(gpsStruct->gHdr.code!=PACKET_GPS_G12_POS ||
     gpsStruct->gHdr.verId!=VER_G12_POS ||
     gpsStruct->gHdr.numBytes!=sizeof(GpsG12PosStruct_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << gpsStruct->gHdr.code << "\t" << PACKET_GPS_G12_POS 
	      << "\nversion:\t" << gpsStruct->gHdr.verId 
	      << "\t" << VER_G12_POS 
	      << "\nsize:\t" << gpsStruct->gHdr.numBytes << "\t"
	      << sizeof(GpsG12PosStruct_t) << std::endl;
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
   trueCourse=gpsStruct->trueCourse;
   verticalVelocity=gpsStruct->verticalVelocity;
   speedInKnots=gpsStruct->speedInKnots;
   pdop=gpsStruct->pdop;
   hdop=gpsStruct->hdop;
   vdop=gpsStruct->vdop;
   tdop=gpsStruct->tdop;
}
