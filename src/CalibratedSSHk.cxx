//////////////////////////////////////////////////////////////////////////////
/////  CalibratedSSHk.h        Calibrated Hk Data                          /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing calibrated hk structs in a tree     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "CalibratedSSHk.h"
#include "RawSSHk.h"
#include <iostream>
#include <fstream>
#include <cstring>

#include <TMath.h>

ClassImp(CalibratedSSHk);

namespace SSConsts {
//Calibration constants, may need to be updated
float calSlope[40]=
  {1,1,100,1,1,1,1,1,1,1,1,100,1,1,1,100,100,100,100,100,1,1,1,1,100,1,1,1,100,1,1,1,1,1,1,100,100,100,100,100};

float calIntercept[40]=
  {0,0,-273.15,0,0,0,0,0,0,0,0,-273.15,0,0,0,-273.15,-273.15,-273.15,-273.15,-273.15,0,0,0,0,-273.15,0,0,0,-273.15,0,0,0,0,0,0,-273.15,-273.15,-273.15,-273.15,-273.15};

int calScheme[40]=
  {0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,1,1,1,1};


//Sunsensor Stuff
//int ssYVals[4][2]={{0,20},{22,3},{26,7},{9,29}};
//int ssXVals[4][2]={{1,21},{23,4},{27,8},{10,30}};

//int sunsensorChans[20]={0,1,20,21,2,22,23,3,4,24,26,27,7,8,28,9,10,29,30,11};
int ssXInds[4][2]={{20,0},{3,22},{7,26},{29,9}};
int ssYInds[4][2]={{21,1},{4,23},{8,27},{30,10}};
int ssTemp[4]={2,24,28,11};



//Sunsensor Stuff
float ssOffset[4][2]={{0.08,-0.1572},{-0.32940,-0.17477},
		      {0.05541,-0.08458},{-0.23773,-0.50356}};
float ssGain[4][2]={{5.0288,5.0},{4.8515,5.0},{5.0599,5.0},{5.0288,5.0}};
float ssSep[4]={3.704391,3.618574,3.512025,3.554451};
float ssAzRel[4]={-135,180,135,90};
float ssGammaFactor[4]={0.4,0.2,0.3,0.5};
const float globalGamma=67;
}

using namespace SSConsts;

CalibratedSSHk::CalibratedSSHk() 
{
   //Default Constructor
}

CalibratedSSHk::~CalibratedSSHk() {
   //Default Destructor
}


CalibratedSSHk::CalibratedSSHk(RawSSHk *hkPtr, RawSSHk *avzPtr, RawSSHk *calPtr)
{
  run=hkPtr->run;
  realTime=hkPtr->realTime;
  payloadTime=hkPtr->payloadTime;
  payloadTimeUs=hkPtr->payloadTimeUs;

  //Now calibrate voltage
  for(int chan=0;chan<40;chan++) {
    float avz= 2048;
    float cal = 4055;   
    if(avzPtr)
      avz = float((avzPtr->acromagData[chan])>>4);	    
    if(calPtr)
      cal = float((calPtr->acromagData[chan])>>4);      
    float slope=4.9/(cal-avz);
    float intercept=-1*slope*avz;
    voltage[chan]=slope*float((hkPtr->acromagData[chan])>>4)+intercept;      
    
    //      std::cout << board << "\t" << chan << "\t" << slope << "\t" << intercept << "\t" << hkPtr->acromagData[chan] << "\t" << voltage[chan] << std::endl;
  }
  

  //Now convert voltage to 'useful' quantity 
  for(int chan=0;chan<40;chan++) {
    switch(calScheme[chan]) {
    case 0:
      //Do nothing
      useful[chan]=voltage[chan];
      break;
    case 1:
      //y = mx +c
      useful[chan]=(calSlope[chan]*voltage[chan])+calIntercept[chan];
      break;
    default:
      useful[chan]=voltage[chan];
      break;
    }			          
  }
  
}


const char *CalibratedSSHk::getName(int ssInd)
{
  switch(ssInd) {
  case 0: return "SS-5";
  case 1: return "SS-6";
  case 2: return "SS-7";
  case 3: return "SS-8";
  default:
    return "n/c";
  }
}


void CalibratedSSHk::getSSMagnitude(int ssInd, Float_t *magnitude, Float_t *magX, Float_t *magY) {
  if(ssInd<0 || ssInd>4) return;
  
  *magnitude=
    TMath::Abs(this->useful[ssXInds[ssInd][0]])+
    TMath::Abs(this->useful[ssXInds[ssInd][1]])+
    TMath::Abs(this->useful[ssYInds[ssInd][0]])+
    TMath::Abs(this->useful[ssYInds[ssInd][1]]);
  
  *magX=TMath::Abs(this->useful[ssXInds[ssInd][0]]+this->useful[ssXInds[ssInd][1]]);
  *magY=TMath::Abs(this->useful[ssYInds[ssInd][0]]+this->useful[ssYInds[ssInd][1]]);  
}


Float_t CalibratedSSHk::getSSTemp(int ssInd) 
{
  if(ssInd<0 || ssInd>4) return -1;
  return this->useful[ssTemp[ssInd]];
}


Int_t CalibratedSSHk::getSSXRatio(int ssInd, Float_t *xRatio)
{
  *xRatio=(this->useful[ssXInds[ssInd][1]]-this->useful[ssXInds[ssInd][0]]);
  Float_t xDenom=(this->useful[ssXInds[ssInd][1]]+this->useful[ssXInds[ssInd][0]]);
  if(TMath::Abs(xDenom)>0.1*TMath::Abs(*xRatio)) {
    (*xRatio)/=xDenom;    
    return 1;
  }    
  else {
    *xRatio=0;
  }
  return 0;    
}

Int_t CalibratedSSHk::getSSYRatio(int ssInd, Float_t *yRatio) 
{
  
  *yRatio=(this->useful[ssYInds[ssInd][1]]-this->useful[ssYInds[ssInd][0]]);
  Float_t yDenom=(this->useful[ssYInds[ssInd][1]]+this->useful[ssYInds[ssInd][0]]);
  if(TMath::Abs(yDenom)>0.1*TMath::Abs(*yRatio)) {
    (*yRatio)/=yDenom;    
    return 1;
  }    
  else {
    *yRatio=0;
  }
  return 0;    
}


Float_t CalibratedSSHk::getSSAzimuth(int ssInd) 
{
  Float_t azimuth,elevation,relAzimuth;
  Float_t pos[3];
  getFancySS(ssInd,pos,&azimuth,&elevation,&relAzimuth);
  return azimuth;
}


Float_t CalibratedSSHk::getSSAzimuthAdu5(int ssInd)
{
  Float_t azimuth,elevation,relAzimuth;
  Float_t pos[3];
  getFancySS(ssInd,pos,&azimuth,&elevation,&relAzimuth);
  return relAzimuth;
}

Float_t CalibratedSSHk::getSSElevation(int ssInd) 
{

  Float_t azimuth,elevation,relAzimuth;
  Float_t pos[3];
  getFancySS(ssInd,pos,&azimuth,&elevation,&relAzimuth);
  return elevation;

}

  


Int_t CalibratedSSHk::getFancySS(int ssInd, Float_t pos[3], Float_t *azimuth,
			       Float_t *elevation, Float_t *relAzimuth) {
  
  //Currently 0 & 1 are broken
  if(ssInd<=1 || ssInd>=4) return 0;


  Float_t magnitude=0,magX=0,magY=0;
  this->getSSMagnitude(ssInd,&magnitude,&magX,&magY);
  
  Float_t xRatio=0;
  Float_t x0=0;    
  if(this->getSSXRatio(ssInd,&xRatio)) {
    x0=xRatio;
    x0*=ssGain[ssInd][0];
    x0-=ssOffset[ssInd][0];
  }    
  
  Float_t y0=0;
  Float_t yRatio=0;
  if(this->getSSYRatio(ssInd,&yRatio)) {
    y0=-1*yRatio;
    y0*=ssGain[ssInd][1];
    y0-=ssOffset[ssInd][1];
  }
  
  
  int goodVals=1;  
  if(magX<0.5) {
    goodVals=0;
    xRatio=0;
    x0=-1*ssOffset[ssInd][0];
  }
  
  if(magY<0.5) {
    goodVals=0;
    yRatio=0;
    y0=-1*ssOffset[ssInd][1];
  }
  
  Float_t gamma=(globalGamma-ssGammaFactor[ssInd])*TMath::DegToRad();		  
  pos[0]=(-1*x0*TMath::Cos(gamma))-(TMath::Sin(gamma)*ssSep[ssInd]);
  pos[1]=-1*y0;
  pos[2]=ssSep[ssInd]*TMath::Cos(gamma)-x0*TMath::Sin(gamma);
  
  *azimuth=TMath::ATan2(pos[1],-1*pos[0])*TMath::RadToDeg();
  Float_t posXY2=TMath::Sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
  *elevation=TMath::ATan2(pos[2],posXY2)*TMath::RadToDeg();
  *relAzimuth=(*azimuth)+ssAzRel[ssInd];
  if((*relAzimuth)<0) (*relAzimuth)+=360;
  
  return goodVals;
}



Float_t   CalibratedSSHk::getRawSunsensor(int ssInd, int type)
{
   if((ssInd>=0 && ssInd<4) && (type>=0 && type<5)) {
      switch(type) {
      case 0:
	 return useful[ssXInds[ssInd][0]%40];
      case 1:
	 return useful[ssXInds[ssInd][1]%40];
      case 2:
	 return useful[ssYInds[ssInd][0]%40];
      case 3:
	 return useful[ssYInds[ssInd][1]%40];
      case 4:
	 return useful[ssTemp[ssInd]%40];
      default:
	 return -1;

      }
   }
   return -1;

}
