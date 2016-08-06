//////////////////////////////////////////////////////////////////////////////
/////  G12Sat.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "G12Sat.h"
#include "AnitaPacketUtil.h"
#include <iostream>
#include <fstream>
#include <cstring>

#include "TPad.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TLatex.h"
#include "TMath.h"
#include "TMarker.h"

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
  simplePacketCheck(&(gpsStruct->gHdr),PACKET_GPS_G12_SAT);
  
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

void G12Sat::getCirclePlot(TPad *padSat)
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

  TLine *liney = new TLine();
  liney->SetLineStyle(1);
  liney->DrawLineNDC(0.03,0.5,0.97,0.5);
  liney->DrawLineNDC(0.5,0.03,0.5,0.97);

  TLatex *texy = new TLatex();
  texy->SetTextSize(0.08);
  texy->DrawTextNDC(0.02,0.93,"G12");

  TMarker *satty = new TMarker();
  for(int i=0;i<(int)numSats;i++) {
    if(snr[i]<30)
       satty->SetMarkerColor(kRed);
    else if(snr[i]<40)
       satty->SetMarkerColor(kOrange);
    else
       satty->SetMarkerColor(kGreen);
    satty->SetMarkerStyle(29);
    satty->SetMarkerSize(2.5);
    Double_t r=0.45*TMath::Cos(TMath::DegToRad()*elevation[i]);
    Double_t x=0.5+r*TMath::Cos(TMath::DegToRad()*azimuth[i]);
    Double_t y=0.5+r*TMath::Sin(TMath::DegToRad()*azimuth[i]);
    //    std::cout << x << "\t" << y << std::endl;
    satty->DrawMarker(x,y);
  }
  
}
