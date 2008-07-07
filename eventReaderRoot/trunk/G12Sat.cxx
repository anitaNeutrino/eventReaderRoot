//////////////////////////////////////////////////////////////////////////////
/////  G12Sat.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "G12Sat.h"
#include <iostream>
#include <fstream>

ClassImp(G12Sat);

G12Sat::G12Sat() 
{
   //Default Constructor
}

G12Sat::~G12Sat() {
   //Default Destructor
}

G12Sat::G12Sat(Int_t           trun,
		 UInt_t          trealTime,
		 GpsG12SatStruct_t *gpsStruct)
{

 if(gpsStruct->gHdr.code!=PACKET_GPS_G12_SAT ||
     gpsStruct->gHdr.verId!=VER_G12_SAT ||
     gpsStruct->gHdr.numBytes!=sizeof(GpsG12SatStruct_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << gpsStruct->gHdr.code << "\t" << PACKET_GPS_G12_SAT 
	      << "\nversion:\t" << gpsStruct->gHdr.verId 
	      << "\t" << VER_G12_SAT 
	      << "\nsize:\t" << gpsStruct->gHdr.numBytes << "\t"
	      << sizeof(GpsG12SatStruct_t) << std::endl;
  }
   run=trun;
   realTime=trealTime;
   payloadTime=gpsStruct->unixTime;
   numSats=gpsStruct->numSats;
   for(int i=0;i<MAX_SATS;i++) {
     prn[i]=gpsStruct->sat[i].prn;
     elevation[i]=gpsStruct->sat[i].elevation;
     snr[i]=gpsStruct->sat[i].snr;
     flag[i]=gpsStruct->sat[i].flag;
     azimuth[i]=gpsStruct->sat[i].azimuth;
   }

}
