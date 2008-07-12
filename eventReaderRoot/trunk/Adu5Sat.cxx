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

#include "TPad.h"
#include "TEllipse.h"
#include "TMath.h"
#include "TMarker.h"

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
if(gpsStruct->gHdr.code!=PACKET_GPS_ADU5_SAT ||
     gpsStruct->gHdr.verId!=VER_ADU5_SAT ||
     gpsStruct->gHdr.numBytes!=sizeof(GpsAdu5SatStruct_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << gpsStruct->gHdr.code << "\t" << PACKET_GPS_ADU5_SAT 
	      << "\nversion:\t" << gpsStruct->gHdr.verId 
	      << "\t" << VER_ADU5_SAT 
	      << "\nsize:\t" << gpsStruct->gHdr.numBytes << "\t"
	      << sizeof(GpsAdu5SatStruct_t) << std::endl;
  }
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


void Adu5Sat::getCirclePlot(TPad *padSat)
{
  padSat->cd();
  TEllipse *lippy = new TEllipse();
  lippy->SetFillColor(0);
  lippy->SetFillStyle(0);
  lippy->DrawEllipse(0.5,0.5,0.45*TMath::Cos(TMath::DegToRad()*10),0,0,360,0);
  lippy->DrawEllipse(0.5,0.5,0.45*TMath::Cos(TMath::DegToRad()*20),0,0,360,0);
  lippy->DrawEllipse(0.5,0.5,0.45*TMath::Cos(TMath::DegToRad()*30),0,0,360,0);
  lippy->DrawEllipse(0.5,0.5,0.45*TMath::Cos(TMath::DegToRad()*40),0,0,360,0);
  lippy->DrawEllipse(0.5,0.5,0.45*TMath::Cos(TMath::DegToRad()*50),0,0,360,0);
  lippy->DrawEllipse(0.5,0.5,0.45*TMath::Cos(TMath::DegToRad()*60),0,0,360,0);
  lippy->DrawEllipse(0.5,0.5,0.45*TMath::Cos(TMath::DegToRad()*70),0,0,360,0);
  lippy->DrawEllipse(0.5,0.5,0.45*TMath::Cos(TMath::DegToRad()*80),0,0,360,0);
  lippy->DrawEllipse(0.5,0.5,0.45*TMath::Cos(TMath::DegToRad()*90),0,0,360,0);

  Int_t markers[4]={29,23,26,28};

  TMarker *satty = new TMarker();
  for(int ant=0;ant<4;ant++) {
    for(int i=0;i<numSats[ant];i++) {
      if(snr[ant][i]<30)
	satty->SetMarkerColor(kRed);
      else
	satty->SetMarkerColor(kGreen);
      satty->SetMarkerStyle(29);
      satty->SetMarkerSize(3);
      Double_t r=0.45*TMath::Cos(TMath::DegToRad()*elevation[ant][i]);
      Double_t x=0.5+r*TMath::Cos(TMath::DegToRad()*azimuth[ant][i]);
      Double_t y=0.5+r*TMath::Sin(TMath::DegToRad()*azimuth[ant][i]);
      std::cout << x << "\t" << y << std::endl;
      satty->DrawMarker(x,y);
    }
  }
}
