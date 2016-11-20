//////////////////////////////////////////////////////////////////////////////
/////  Adu5Sat.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "Adu5Sat.h"
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
  simplePacketCheck(&(gpsStruct->gHdr),PACKET_GPS_ADU5_SAT);
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


void Adu5Sat::getCirclePlot(TPad *padSat, char *title)
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
  if(title)
    texy->DrawTextNDC(0.02,0.93,title);
  else
    texy->DrawTextNDC(0.02,0.93,"ADU5");

  Int_t markers[4]={29,23,26,28};

  TMarker *satty = new TMarker();
  for(int ant=0;ant<4;ant++) {
    for(int i=0;i<(int)numSats[ant];i++) {
      if(snr[ant][i]<30)
	satty->SetMarkerColor(kRed);
      else if(snr[ant][i]<40)
	satty->SetMarkerColor(kOrange);
      else
	satty->SetMarkerColor(kGreen);
      satty->SetMarkerStyle(markers[ant]);
      satty->SetMarkerSize(2.5);
      Double_t r=0.45*TMath::Cos(TMath::DegToRad()*elevation[ant][i]);
      Double_t x=0.5+r*TMath::Cos(TMath::DegToRad()*azimuth[ant][i]);
      Double_t y=0.5+r*TMath::Sin(TMath::DegToRad()*azimuth[ant][i]);
      //      std::cout << x << "\t" << y << std::endl;
      satty->DrawMarker(x,y);
    }
  }
}



int Adu5Sat::getNumSats(int whichAnt)
{
  if(whichAnt>=0 && whichAnt<4)
    return numSats[whichAnt];
  return 0;
}

int Adu5Sat::getPRN(int whichAnt,int whichSat)
{
  if(whichAnt>=0 && whichAnt<4) 
    if(whichSat<numSats[whichAnt])
      return prn[whichAnt][whichSat];
  return 0;

}

int Adu5Sat::getSNR(int whichAnt,int whichSat)
{

  if(whichAnt>=0 && whichAnt<4) 
    if(whichSat<numSats[whichAnt])
      return snr[whichAnt][whichSat];
  return 0;
}

int Adu5Sat::getElevation(int whichAnt,int whichSat)
{
  if(whichAnt>=0 && whichAnt<4) 
    if(whichSat<numSats[whichAnt])
      return elevation[whichAnt][whichSat];
  return 0;
}

int Adu5Sat::getAzimuth(int whichAnt,int whichSat)
{
  if(whichAnt>=0 && whichAnt<4) 
    if(whichSat<numSats[whichAnt])
      return azimuth[whichAnt][whichSat];
  return 0;
}

int Adu5Sat::getFlag(int whichAnt,int whichSat)
{
  if(whichAnt>=0 && whichAnt<4) 
    if(whichSat<numSats[whichAnt])
      return flag[whichAnt][whichSat];
  return 0;
}
