//////////////////////////////////////////////////////////////////////////////
/////  G12Pos.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "G12Pos.h"
#include "AnitaPacketUtil.h"
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
  simplePacketCheck(&(gpsStruct->gHdr),PACKET_GPS_G12_POS);
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
