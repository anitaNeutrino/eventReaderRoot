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

ClassImp(CalibratedHk);


//Calibration constants, may need to be updated
float calSlope[3][40]=
{
    {1,1,100,1,1,1,1,1,1,1,1,100,1,1,1,100,100,100,100,100,1,1,1,1,100,1,1,1,100,1,1,1,1,1,1,100,100,100,100,100},
    {-2,1,2,-4,4,1,2,1,1,1,1,1,1,8,8,20,20,20,1,1,0.8,12,0.8,0.8,8,20,0.8,1,1,1,1,1,1,4,4,1,10.1377,18.252,1,1},
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
      avz = float((avzPtr->acromagData[board][chan])>>4);	    
      cal = float((calPtr->acromagData[board][chan])>>4);      
      float slope=4.9/(cal-avz);
      float intercept=-1*slope*avz;
      voltage[board][chan]=slope*float((hkPtr->acromagData[board][chan])>>4)+intercept;      
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

