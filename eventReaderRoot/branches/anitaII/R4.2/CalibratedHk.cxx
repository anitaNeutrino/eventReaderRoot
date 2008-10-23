//////////////////////////////////////////////////////////////////////////////
/////  CalibratedHk.h        Calibrated Hk Data                          /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing calibrated hk structs in a tree     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "CalibratedHk.h"
#include "RawHk.h"
#include <iostream>
#include <fstream>

#include <TMath.h>

ClassImp(CalibratedHk);


//Calibration constants, may need to be updated
float calSlope[3][40]=
{
    {1,1,100,1,1,1,1,1,1,1,1,100,1,1,1,100,100,100,100,100,1,1,1,1,100,1,1,1,100,1,1,1,1,1,1,100,100,100,100,100},
    {2,1,2,-4,4,2,2,1,1,1,1,1,1,8,8,20,20,20,1,1,0.8,8,0.8,0.8,20,20,0.8,1,1,1,1,1,1,4,4,1,10.1377,18.252,1,1},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100}
};

float calIntercept[3][40]=
{
    {0,0,-273.15,0,0,0,0,0,0,0,0,-273.15,0,0,0,-273.15,-273.15,-273.15,-273.15,-273.15,0,0,0,0,-273.15,0,0,0,-273.15,0,0,0,0,0,0,-273.15,-273.15,-273.15,-273.15,-273.15},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15,-273.15}
};

int calScheme[3][40]=
{
    {0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,1,1,1,1},
    {1,1,1,1,1,1,1,2,3,2,4,5,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,6,7,8,9,10,0,1,1,0,1,1,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


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
float ssAzRel[4]={45,-45,-135,135};
float ssGammaFactor[4]={0.4,0.2,0.3,0.5};
const float globalGamma=67;



CalibratedHk::CalibratedHk() 
{
   //Default Constructor
}

CalibratedHk::~CalibratedHk() {
   //Default Destructor
}


CalibratedHk::CalibratedHk(RawHk *hkPtr, RawHk *avzPtr, RawHk *calPtr)
{
  run=hkPtr->run;
  realTime=hkPtr->realTime;
  payloadTime=hkPtr->payloadTime;
  payloadTimeUs=hkPtr->payloadTimeUs;
  magX=hkPtr->magX;
  magY=hkPtr->magY;
  magZ=hkPtr->magZ;
  memcpy(sbsTemp,hkPtr->sbsTemp,sizeof(Short_t)*4);

  //Now calibrate voltage
  for(int board=0;board<3;board++) {
    for(int chan=0;chan<40;chan++) {
      float avz= 2048;
      float cal = 4055;   
      if(avzPtr)
	avz = float((avzPtr->acromagData[board][chan])>>4);	    
      if(calPtr)
	cal = float((calPtr->acromagData[board][chan])>>4);      
      float slope=4.9/(cal-avz);
      float intercept=-1*slope*avz;
      voltage[board][chan]=slope*float((hkPtr->acromagData[board][chan])>>4)+intercept;      
      
      //      std::cout << board << "\t" << chan << "\t" << slope << "\t" << intercept << "\t" << hkPtr->acromagData[board][chan] << "\t" << voltage[board][chan] << std::endl;
    }
  }

  //Now convert voltage to 'useful' quantity 
  float vs=2.*(voltage[1][6]); //Heck knows what this is
  for(int board=0;board<3;board++) {
    for(int chan=0;chan<40;chan++) {
      switch(calScheme[board][chan]) {
      case 0:
	//Do nothing
	useful[board][chan]=voltage[board][chan];
	break;
      case 1:
	//y = mx +c
	useful[board][chan]=(calSlope[board][chan]*voltage[board][chan])+
	  calIntercept[board][chan];
	break;
      case 2:
	//Accel T
	useful[board][chan]=44.4*((voltage[board][chan]*(5./vs)-1.375));
	break;
      case 3:
	//Accel 2-Y
	useful[board][chan]=(1./0.844)*((voltage[board][chan]*(5./vs)-2.523));
	break;
      case 4:
	//Accel 1-Y
	useful[board][chan]=(1./0.839)*((voltage[board][chan]*(5./vs)-2.512));
	break;
      case 5:
	//Low pressure
	useful[board][chan]=0.00215181+(voltage[board][chan]*4.0062)-(voltage[board][chan]*voltage[board][chan]*0.0027642); //PSI
	//		    useful[board][chan]*=6894.76; //Pascals
	useful[board][chan]*=68.046e-3; //Atmospheres
	break;
      case 6:
	//Accel 2-Z
	useful[board][chan]=(1./0.843)*((voltage[board][chan]*(5./vs)-2.523));
	break;
      case 7:
	//Accel 2-X
	useful[board][chan]=(1./0.827)*((voltage[board][chan]*(5./vs)-2.523));
	break;
      case 8:
	//Accel 1-Z
	useful[board][chan]=(1./0.842)*((voltage[board][chan]*(5./vs)-2.506));
	break;
      case 9:
	//Accel 1-X
	useful[board][chan]=(1./0.830)*((voltage[board][chan]*(5./vs)-2.493));
	break;
      case 10:
	//High Pressure
	useful[board][chan]=3.75*(voltage[board][chan]-0.25); //Torr
	useful[board][chan]*=1.3158e-3; //Atmospheres
	//		    useful[board][chan]*=133.322; //Pascals
	break;
      default:
	useful[board][chan]=voltage[board][chan];
	break;
      }			          
    }
  }
}


Float_t   CalibratedHk::getInternalTemp(int index)
{
  //  int internalTempChans[NUM_INT_TEMPS]={0,20,1,21,2,22,3,23,4,24,5,25,6,26,7};
  int internalTempChans[NUM_INT_TEMPS]={80,100,81,101,82,102,83,103,84,104,85,105,86,106,87};


  if(index>=0 && index<NUM_INT_TEMPS)
    return useful[internalTempChans[index]/40][internalTempChans[index]%40];
  return -1;
}

Float_t   CalibratedHk::getExternalTemp(int index)
{  
  int externalTempChans[NUM_EXT_TEMPS]={112,93,113,94,114,95,115,96,116,97,117,98,118,99,119,39,19,38,18,37,17,36,16,35,15};
  //  int externalTempChans[NUM_EXT_TEMPS]={32,13,33,14,34,15,35,16,36,17,37,18,38,19,39};
  if(index>=0 && index<NUM_EXT_TEMPS)
    return useful[externalTempChans[index]/40][externalTempChans[index]%40];
  return -273;
}

Float_t   CalibratedHk::getSBSTemp(int index)
{
  Int_t sbsMagicAdd[NUM_SBS_TEMPS]={0,0,15,15};
  if(index>=0 && index<NUM_SBS_TEMPS)
    return (25e-3*sbsTemp[index]) + sbsMagicAdd[index];
  return -273;
}


Float_t CalibratedHk::getVoltage(int index) {
  int powerVoltChans[NUM_VOLTAGES]={74,73,44,76,77,46,42,45,41,43,40};
  if(index>=0 && index<NUM_VOLTAGES) {
    return useful[powerVoltChans[index]/40][powerVoltChans[index]%40];
  }
  return -1;
}

Float_t CalibratedHk::getCurrent(int index) {  
  int powerAmpChans[NUM_CURRENTS]={54,53,56,57,61,64,60,63,66,62,65,55};
  if(index>=0 && index<NUM_CURRENTS) {
        return useful[powerAmpChans[index]/40][powerAmpChans[index]%40];
  }
  return -1;
}

Float_t CalibratedHk::getPower(int index) {  
  int powerWattChans[NUM_POWERS][2]={{74,54},{73,53},{76,56},{77,57},{42,61},{76,55}};
  if(index>=0 && index<NUM_POWERS) {
    float volts=useful[powerWattChans[index][0]/40][powerWattChans[index][0]%40];
    float amps=useful[powerWattChans[index][1]/40][powerWattChans[index][1]%40];
    return volts*amps;
  }
  return 0;
}

Float_t CalibratedHk::getAttitude(int index) {
  int attitudeChans[10]={70,50,69,49,68,58,67,57,51,71};
  if(index>=0 && index<10) {
    return useful[attitudeChans[index]/40][attitudeChans[index]%40];
  }
  else if(index==10)
    return magX;
  else if(index==11)
    return magY;
  else if(index==12)
    return magZ;
  return -1;
}


char *CalibratedHk::getAttitudeName(int index) {
  char *attChanNames[NUM_ATTITUDE]={"Ac1-X","Ac1-Y","Ac1-Z","Ac1-T","Ac2-X","Ac2-Y","Ac2-Z","Ac2-T","Pres","Pres","Mag-X","Mag-Y","Mag-Z"};
  if(index>=0 && index<NUM_ATTITUDE)
    return attChanNames[index];
  return "None";
}

char *CalibratedHk::getPowerName(int index) {
  char *powerNames[NUM_POWERS]={"RF1","RF2","+24","PV","+5S","Batt"};
  if(index>=0 && index<NUM_POWERS) 
    return powerNames[index];
  return "None";
}


char *CalibratedHk::getCurrentName(int index)
{
//char *currentNames[NUM_CURRENTS]={"RF1","RF2","Batt","+24","PV","+1.5","+5S","-12","+12","+3.3","+5","-5"};
char *currentNames[NUM_CURRENTS]={"RF1","RF2","+24","PV","+5S","+3.3","+1.5","+12","+5NB","-12","+5","Batt"};
 if(index>=0 && index<NUM_CURRENTS) 
    return currentNames[index];
  return "None";


}

char *CalibratedHk::getVoltageName(int index)
{

char *voltageNames[NUM_VOLTAGES]={"RF1","RF2","+12","+24","PV","+5","+5S","+3.3","+1.5","-12","-5"};
 if(index>=0 && index<NUM_VOLTAGES) 
    return voltageNames[index];
  return "None";

}
char *CalibratedHk::getExternalTempName(int index)
{

char *extTempNames[NUM_EXT_TEMPS]={"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25"};
 if(index>=0 && index<NUM_EXT_TEMPS) 
    return extTempNames[index];
  return "None";
}

char *CalibratedHk::getInternalTempName(int index)
{
char *intTempNames[NUM_INT_TEMPS]={"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15"}; 
 if(index>=0 && index<NUM_INT_TEMPS) 
    return intTempNames[index];
 return "None";
}

char *CalibratedHk::getSBSTempName(int index)
{
  char *sbsTempNames[NUM_SBS_TEMPS]={"CPU","CPU","Core1","Core2"};
  if(index>=0 && index<NUM_SBS_TEMPS) 
   return sbsTempNames[index];
  return "None";
}




void CalibratedHk::getSSMagnitude(int ssInd, Float_t *magnitude, Float_t *magX, Float_t *magY) {
  if(ssInd<0 || ssInd>4) return;
  
  *magnitude=
    TMath::Abs(this->useful[0][ssXInds[ssInd][0]])+
    TMath::Abs(this->useful[0][ssXInds[ssInd][1]])+
    TMath::Abs(this->useful[0][ssYInds[ssInd][0]])+
    TMath::Abs(this->useful[0][ssYInds[ssInd][1]]);
  
  *magX=TMath::Abs(this->useful[0][ssXInds[ssInd][0]]+this->useful[0][ssXInds[ssInd][1]]);
  *magY=TMath::Abs(this->useful[0][ssYInds[ssInd][0]]+this->useful[0][ssYInds[ssInd][1]]);  
}


Float_t CalibratedHk::getSSTemp(int ssInd) 
{
  if(ssInd<0 || ssInd>4) return -1;
  return this->useful[0][ssTemp[ssInd]];
}


Int_t CalibratedHk::getSSXRatio(int ssInd, Float_t *xRatio)
{
  *xRatio=(this->useful[0][ssXInds[ssInd][1]]-this->useful[0][ssXInds[ssInd][0]]);
  Float_t xDenom=(this->useful[0][ssXInds[ssInd][1]]+this->useful[0][ssXInds[ssInd][0]]);
  if(TMath::Abs(xDenom)>0.1*TMath::Abs(*xRatio)) {
    (*xRatio)/=xDenom;    
    return 1;
  }    
  else {
    *xRatio=0;
  }
  return 0;    
}

Int_t CalibratedHk::getSSYRatio(int ssInd, Float_t *yRatio) 
{
  
  *yRatio=(this->useful[0][ssYInds[ssInd][1]]-this->useful[0][ssYInds[ssInd][0]]);
  Float_t yDenom=(this->useful[0][ssYInds[ssInd][1]]+this->useful[0][ssYInds[ssInd][0]]);
  if(TMath::Abs(yDenom)>0.1*TMath::Abs(*yRatio)) {
    (*yRatio)/=yDenom;    
    return 1;
  }    
  else {
    *yRatio=0;
  }
  return 0;    
}
  


Int_t CalibratedHk::getFancySS(int ssInd, Float_t pos[3], Float_t *azimuth,
			       Float_t *elevation, Float_t *relAzimuth) {
  
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


Float_t   CalibratedHk::getPressure(int index)
{
   int presChans[2]={51,71};
   if(index>=0 && index<2) {
      return useful[presChans[index]/40][presChans[index]%40];
   }
   return -1;
}

Float_t   CalibratedHk::getAccelerometer(int acInd, int type)
{
   int accelChans[2][4]={{70,50,69,49},{68,58,67,57}};
   if((acInd>=0 && acInd<2) && (type>=0 && type<4)) {
      return useful[accelChans[acInd][type]/40][accelChans[acInd][type]%40];
   }
   return -1;

}

Float_t   CalibratedHk::getRawSunsensor(int ssInd, int type)
{
   if((ssInd>=0 && ssInd<4) && (type>=0 && type<5)) {
      switch(type) {
      case 0:
	 return useful[ssXInds[ssInd][0]/40][ssXInds[ssInd][0]%40];
      case 1:
	 return useful[ssXInds[ssInd][1]/40][ssXInds[ssInd][1]%40];
      case 2:
	 return useful[ssYInds[ssInd][0]/40][ssYInds[ssInd][0]%40];
      case 3:
	 return useful[ssYInds[ssInd][1]/40][ssYInds[ssInd][1]%40];
      case 4:
	 return useful[ssTemp[ssInd]/40][ssTemp[ssInd]%40];
      default:
	 return -1;

      }
   }
   return -1;

}
