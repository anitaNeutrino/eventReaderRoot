//////////////////////////////////////////////////////////////////////////////
/////  Adu5Sat.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "Adu5Sat.h"
#include <iostream>
#include <fstream>

ClassImp(Adu5Sat);

Adu5Sat::Adu5Sat() 
{
   //Default Constructor
}

Adu5Sat::~Adu5Sat() {
   //Default Destructor
}

Adu5Sat::Adu5Sat(Int_t           trun,
		 UInt_t          trealTime,
		 GpsAdu5SatStruct_t *gpsStruct)
{

   run=trun;
   realTime=trealTime;
   payloadTime=gpsStruct->unixTime;
   for(int ant=0;ant<4;ant++) {
     numSats[ant]=gpsStruct->numSats[ant];
     for(int i=0;i<MAX_SATS;i++) {
       prn[ant][i]=gpsStruct->sat[ant][i].prn;
       elevation[ant][i]=gpsStruct->sat[ant][i].elevation;
       snr[ant][i]=gpsStruct->sat[ant][i].snr;
       flag[ant][i]=gpsStruct->sat[ant][i].flag;
       azimuth[ant][i]=gpsStruct->sat[ant][i].azimuth;
     }
   }

}
