//////////////////////////////////////////////////////////////////////////////
/////  UsefulAnitaEvent.cxx        ANITA event reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in useful ANITA events and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "UsefulAnitaEvent.h"
#include "CalibratedAnitaEvent.h"
#include "AnitaGeomTool.h"
#include "PrettyAnitaHk.h"
#include "RawAnitaHeader.h"
#include "AnitaEventCalibrator.h"
#include <TGraph.h>
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(UsefulAnitaEvent);


#define MAGIC_DELTAT 8 ///<This is the supposed clock period

Double_t getZeroCrossingPoint(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
  Double_t m=(y1-y2)/(x1-x2);
  Double_t c=y1-m*x1;
  return (-1*c/m);
}


UsefulAnitaEvent::UsefulAnitaEvent() 
  : RawAnitaEvent()
{
  gotCalibTemp=0;
  fLastEventGuessed=0;
  fC3poNum=0;
  fFromCalibratedAnitaEvent=0;
  //Default Constructor
}


UsefulAnitaEvent::UsefulAnitaEvent(CalibratedAnitaEvent *calibratedPtr, WaveCalType::WaveCalType_t calType) 
  : RawAnitaEvent(*calibratedPtr)
{
  fFromCalibratedAnitaEvent=1;
  fC3poNum=calibratedPtr->fC3poNum;
  fLastEventGuessed=calibratedPtr->eventNumber;
  gotCalibTemp=0;
  fTempFactorGuess=calibratedPtr->fTempFactorGuess;
  for(int surf=0;surf<10;surf++) {
    fRcoArray[surf]=calibratedPtr->fRcoArray[surf];
    fClockPhiArray[surf]=calibratedPtr->fClockPhiArray[surf];    
  }
  fCalType=calType;
  calibrateEvent(fCalType);
}

UsefulAnitaEvent::UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, PrettyAnitaHk *theHk) 
  : RawAnitaEvent(*eventPtr)
{
  fC3poNum=0;
  fLastEventGuessed=0;
  if(theHk) {
    gotCalibTemp=1;
    calibTemp=theHk->intTemps[2];
  }
  else 
    gotCalibTemp=0;
   
  //Default Constructor
  calibrateEvent(calType);
}

UsefulAnitaEvent::UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, RawAnitaHeader *theHd) 
  : RawAnitaEvent(*eventPtr)
{
  fFromCalibratedAnitaEvent=0;
  fC3poNum=theHd->c3poNum;
  fLastEventGuessed=0;
  gotCalibTemp=0;
   
  //Default Constructor
  calibrateEvent(calType);
}

UsefulAnitaEvent::UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, Double_t surfTemp) 
  : RawAnitaEvent(*eventPtr)
{
  fFromCalibratedAnitaEvent=0;
  fC3poNum=0;
  gotCalibTemp=1;
  calibTemp=surfTemp;
     
  //Default Constructor
  calibrateEvent(calType);
}

UsefulAnitaEvent::~UsefulAnitaEvent() {
  //Default Destructor
}

int UsefulAnitaEvent::calibrateEvent(WaveCalType::WaveCalType_t calType)
{
  //   std::cout << "UsefulAnitaEvent::calibrateEvent" << std::endl;
  fCalibrator=AnitaEventCalibrator::Instance();
  fCalibrator->calibrateUsefulEvent(this,calType);
  
  fCalType=calType;
  //Fill the calibrated variables  
  switch(calType) {
  case WaveCalType::kJustUnwrap:
    //      std::cout << "WaveCalType::kJustUnwrap" << std::endl;
    //Fill struct with unwrapped raw values;      
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan); 
	//	    std::cout << "chanIndex: " << chanIndex << std::endl;

	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	//	    std::cout << "chanIndex: " << chanIndex << std::endl;
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	//	    std::cout << "chanIndex: " << chanIndex << std::endl;
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->unwrappedArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=samp;
	}	 
	//	    std::cout << "chanIndex: " << chanIndex << std::endl;

      }
    }
      
    break;
  case WaveCalType::kJustTimeNoUnwrap:
    //      std::cout << "WaveCalType::kJustUnwrap" << std::endl;
    //Fill struct with unwrapped raw values;      
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan); 
	//	    std::cout << "chanIndex: " << chanIndex << std::endl;

	this->fNumPoints[chanIndex]=NUM_SAMP;
	//	    std::cout << "chanIndex: " << chanIndex << std::endl;
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	//	    std::cout << "chanIndex: " << chanIndex << std::endl;
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->rawArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->timeArray[surf][chan][samp];
	}	 
	//	    std::cout << "chanIndex: " << chanIndex << std::endl;

      }
    }      
    break;
  case WaveCalType::kVoltageTime:
    //Just use the nominal 1mv per count, and 2.6GSamps/sec numbers     
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->unwrappedArray[surf][chan][samp]*2; //*2 for the bit shifting
	  this->fTimes[chanIndex][samp]=double(samp)/2.6;
	}
      }
    }
    break;
  case WaveCalType::kVTLabRG:
  case WaveCalType::kVTLabAG:
    //All Abby's lovely numbers    
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->mvArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->surfTimeArray[surf][samp];
	  this->fCapacitorNum[chanIndex][samp]=fCalibrator->scaArray[surf][chan][samp];
	}
      }
    }
      
    break;
  case WaveCalType::kVTFullRG:
    //All my lovely numbers + cable delay
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->mvArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->surfTimeArray[surf][samp]+
	    fCalibrator->groupDelayCalib[surf][chan];

	}
      }
    }
    break;
  case WaveCalType::kVTLabJW:
    //All Jiwoo's lovely numbers    
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->unwrappedArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->surfTimeArray[surf][samp];
	}
      }
    }
    break;
  case WaveCalType::kVTFullJW:
    //All Jiwoo's lovely numbers + cable delay
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->unwrappedArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->surfTimeArray[surf][samp]+
	    fCalibrator->groupDelayCalib[surf][chan];
	  //		if(chan==0) {
	  //		   std::cout << surf << "\t" << chan << "\t" << samp << "\t"
	  //			     << fCalibrator->surfTimeArray[surf][samp] << std::endl;
	  //		}
	}
      }
    }
    break;
  case WaveCalType::kVTLabJWPlus:
    //All Jiwoo's lovely numbers    
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->mvArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->surfTimeArray[surf][samp];
	}
      }
    }
    break;
  case WaveCalType::kVTFullJWPlus:
  case WaveCalType::kVTFullJWPlusFudge:
    //All Jiwoo's lovely numbers + cable delay
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  //	       std::cout << fCalibrator->surfTimeArray[surf][samp] << "\t" 
	  //		    << fCalibrator->mvArray[surf][chan][samp] << std::endl;
	  this->fVolts[chanIndex][samp]=fCalibrator->mvArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->surfTimeArray[surf][samp]+
	    fCalibrator->groupDelayCalib[surf][chan];
	}
      }
    }
    break;
       
  case WaveCalType::kVTLabAGFastClock:
  case WaveCalType::kVTLabAGCrossCorClock:
  case WaveCalType::kVTLabClockRG:
  case WaveCalType::kVTLabClockZeroRG:
    // kVTLabRG + Clock Jitter Correction (+ Zero Mean)
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	//	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	//	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->mvArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->timeArray[surf][chan][samp];
	  this->fCapacitorNum[chanIndex][samp]=fCalibrator->scaArray[surf][chan][samp];
	}
	for(int samp=this->fNumPoints[chanIndex];samp<NUM_SAMP;samp++) {
	   this->fVolts[chanIndex][samp]=0;
	   this->fTimes[chanIndex][samp]=0;
	}
      }
    }      
    break;

  case WaveCalType::kVTFullAGFastClock:
  case WaveCalType::kVTFullAGCrossCorClock:
  case WaveCalType::kVTCalFilePlusSimon:
    // kVTLabAG + Clock Jitter Correction (+ Zero Mean)
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	//	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	//	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->mvArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->timeArray[surf][chan][samp]-fCalibrator->chipByChipDeltats[surf][chan][getLabChip(chanIndex)];	  
	  if(calType==WaveCalType::kVTCalFilePlusSimon) {
	    this->fTimes[chanIndex][samp]-=fCalibrator->simonsDeltaT[surf][chan];
	  }
	  //	  std::cout << surf << "\t" << chan << "\t" << fCalibrator->chipByChipDeltats[surf][chan][getLabChip(chanIndex)] <<"\n";
	  this->fCapacitorNum[chanIndex][samp]=fCalibrator->scaArray[surf][chan][samp];
	}
	for(int samp=this->fNumPoints[chanIndex];samp<NUM_SAMP;samp++) {
	   this->fVolts[chanIndex][samp]=0;
	   this->fTimes[chanIndex][samp]=0;
	}
      }
    }      
    break;

  case WaveCalType::kVTFullClockRG:
  case WaveCalType::kVTFullClockZeroRG:
    // kVTFullRG + Clock Jitter Correction (+ Zero Mean)
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->mvArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->timeArray[surf][chan][samp]+
	    fCalibrator->groupDelayCalib[surf][chan];
	}
      }
    }      
    break;
       
  case WaveCalType::kVTLabJWPlusClock:
  case WaveCalType::kVTLabJWPlusClockZero: 
  case WaveCalType::kVTLabJWPlusFastClockZero:
    // kVTLabJWPlus + Clock Jitter Correction (+ Zero Mean) 
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->mvArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->timeArray[surf][chan][samp];
	}
      }
    }
    break;

      

  case WaveCalType::kVTFullJWPlusClock:              
  case WaveCalType::kVTFullJWPlusClockZero:  
  case WaveCalType::kVTFullJWPlusFastClockZero:
  case WaveCalType::kVTFullJWPlusFancyClockZero:        
    // kVTFullJWPlus + Clock Jitter Correction (+ Zero Mean)
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->mvArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=fCalibrator->timeArray[surf][chan][samp]+
	    fCalibrator->groupDelayCalib[surf][chan];
	}
      }
    }      
    break;
       

  case WaveCalType::kNoCalib:
  case WaveCalType::kADC:
  default: 
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {
	int chanIndex=getChanIndex(surf,chan);
	this->fNumPoints[chanIndex]=NUM_SAMP;
	for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	  this->fVolts[chanIndex][samp]=fCalibrator->rawArray[surf][chan][samp];
	  this->fTimes[chanIndex][samp]=samp;
	}
      }
    }	    	    
    break;
  }
  //   std::cout << "UsefulAnitaEvent::calibrateEvent -- end" << std::endl;
  
  return 0;
}


TGraph *UsefulAnitaEvent::getGraphFromSurfAndChan(int surf, int chan) 
{
  return getGraph(getChanIndex(surf,chan));
}

TGraph *UsefulAnitaEvent::getGraph(int chanIndex)
{
  if(chanIndex<0 || chanIndex>=NUM_DIGITZED_CHANNELS)
    return NULL;
  //   std::cout << chanIndex << std::endl;
   
  TGraph *grPtr=new TGraph(fNumPoints[chanIndex],
			   fTimes[chanIndex],
			   fVolts[chanIndex]);
  return grPtr;
}

TGraph *UsefulAnitaEvent::getGraph(int ant, AnitaPol::AnitaPol_t pol)
{
  //Antenna numbers count from 0
  return getGraph(AnitaGeomTool::getChanIndexFromAntPol(ant,pol));

}

TGraph *UsefulAnitaEvent::getGraph(AnitaRing::AnitaRing_t ring,
				   int phi,
				   AnitaPol::AnitaPol_t pol)
{
  //Phi numbers count from 0
  return getGraph(AnitaGeomTool::getChanIndexFromRingPhiPol(ring,phi,pol));

}


Int_t UsefulAnitaEvent::guessRco(int chanIndex)
{
 
  if(fLastEventGuessed!=this->eventNumber) 
    analyseClocksForGuesses();
  return fRcoArray[chanIndex/9];
}

Double_t UsefulAnitaEvent::getTempCorrectionFactor()
{
  if(gotCalibTemp) {
    double tempScale=29.938/(31.7225-0.054*calibTemp);
    return tempScale;
  }
  else {
    if(fLastEventGuessed!=this->eventNumber) 
      analyseClocksForGuesses();
    return fTempFactorGuess;
  }
}

void UsefulAnitaEvent::analyseClocksForGuesses() 
{
  AnitaEventCalibrator *myCally = AnitaEventCalibrator::Instance();
  Double_t timeVals[NUM_SURF][NUM_SAMP][2]={{{0}}};
  Int_t labChip=this->getLabChip(8);
  Double_t clockPeriod=MAGIC_DELTAT;
  if(fC3poNum>120e6 && fC3poNum<130e6) {
    clockPeriod=1e9/Double_t(fC3poNum);
  }
  Double_t tempFactor=1;
  if(gotCalibTemp)
    tempFactor=getTempCorrectionFactor();
  //Somehow are going to try and fudge things so that we can guess the
  //temperature correction
    
  Double_t deltaTFirstRco[NUM_SURF][2]={{0}};
  Int_t numDeltaTFirstRco[NUM_SURF][2]={{0}};
  Double_t deltaTSecondRco[NUM_SURF][2]={{0}};
  Int_t numDeltaTSecondRco[NUM_SURF][2]={{0}};

  Int_t skipSurf[NUM_SURF]={0};
      
  for(int surf=0;surf<NUM_SURF;surf++) {
    //Fill in timebase
    
    Int_t countZeros=0;
    Double_t timeRco[2]={0};
    Int_t clockIndex=9*surf + 8;
    for(int samp=0;samp<NUM_SAMP;samp++) {
      if(this->data[clockIndex][samp]==0) 
	countZeros++;
      for(int rco=0;rco<2;rco++) {
	timeVals[surf][samp][rco]=timeRco[rco];
	timeRco[rco]+=myCally->justBinByBin[surf][labChip][rco][samp]*tempFactor;
      }
    }
    if(countZeros>200) {
      skipSurf[surf]=1;
      continue;
    }

    //Now we can try and find the zero crossings and periods.
    Int_t earliestSample=this->getEarliestSample(clockIndex);
    Int_t latestSample=this->getLatestSample(clockIndex);
    Double_t offset=TMath::Mean(NUM_SAMP,this->data[clockIndex]);
	 
    //    std::cout << latestSample << "\t" << earliestSample << "\t" << clockIndex << "\n";
    for(int rcoGuess=0;rcoGuess<2;rcoGuess++) {
      //	   rcoGuess==0  means first rco is 1
      //      rcoGuess==1 means first rco is 0
      if(latestSample<earliestSample) {
	//We have two RCOs
	{
	  //Here is the first RCO
	  Double_t posZcUp[100]={0};
	  Double_t posZcDown[100]={0};
	  Int_t numZcUp=0;
	  Int_t numZcDown=0;	    
	  for(int samp=earliestSample;samp<259;samp++) {
	      
	    Double_t firstVal=this->data[clockIndex][samp]-offset;
	    Double_t secondVal=this->data[clockIndex][samp+1]-offset;
	    //	    std::cout << surf << "\t" << clockIndex << "\t" << samp << "\t" 
	    //		      << firstVal << "\t" << secondVal << "\n";

	    if((firstVal>=0 && secondVal<=0)) {  
	      //We have a ZC down
	      posZcDown[numZcDown]=getZeroCrossingPoint(timeVals[surf][samp][1-rcoGuess],firstVal,timeVals[surf][samp+1][1-rcoGuess],secondVal);
	      numZcDown++;
	    }
	    if((firstVal<=0 && secondVal>=0)) {  
	      //We have a ZC up
	      posZcUp[numZcUp]=getZeroCrossingPoint(timeVals[surf][samp][1-rcoGuess],firstVal,timeVals[surf][samp+1][1-rcoGuess],secondVal);
	      numZcUp++;
	    }
	  }
	  //	  std::cout << numZcDown << "\t" << numZcUp << "\n";
	       
	  deltaTFirstRco[surf][rcoGuess]=0;
	  if(numZcDown>1) {	       
	    for(int i=1;i<numZcDown;i++) {
	      //	      std::cout << (posZcDown[i]-posZcDown[i-1]) << "\t" << clockPeriod << std::endl;
	      if((posZcDown[i]-posZcDown[i-1])>(clockPeriod-0.5) && (posZcDown[i]-posZcDown[i-1])<(clockPeriod+0.5)) {
		deltaTFirstRco[surf][rcoGuess]+=(posZcDown[i]-posZcDown[i-1]);
		numDeltaTFirstRco[surf][rcoGuess]++;
	      }
	    }
	  }
	  if(numZcUp>1) {	       
	    for(int i=1;i<numZcUp;i++) {
	      if((posZcUp[i]-posZcUp[i-1])>(clockPeriod-0.5) && (posZcUp[i]-posZcUp[i-1])<(clockPeriod+0.5)) {
		deltaTFirstRco[surf][rcoGuess]+=(posZcUp[i]-posZcUp[i-1]);
		numDeltaTFirstRco[surf][rcoGuess]++;
	      }
	    }
	  }
	  if(numDeltaTFirstRco[surf][rcoGuess]>0) {
	    deltaTFirstRco[surf][rcoGuess]/=numDeltaTFirstRco[surf][rcoGuess];
	  }
	  else {
	    deltaTFirstRco[surf][rcoGuess]=0;
	  }
	}
	//	std::cout << surf << "\t" << rcoGuess << "\t" << numDeltaTFirstRco[surf][rcoGuess] << "\t" << deltaTFirstRco[surf][rcoGuess] << "\n";
	{
	  //Now lets do the second RCO
	       
	  Double_t posZcUp[100]={0};
	  Double_t posZcDown[100]={0};
	  Int_t numZcUp=0;
	  Int_t numZcDown=0;
	  if(latestSample>0) {
	    for(int samp=0;samp<latestSample;samp++) {
	      Double_t firstVal=this->data[clockIndex][samp]-offset;
	      Double_t secondVal=this->data[clockIndex][samp+1]-offset;
	      if((firstVal>=0 && secondVal<=0)) {  
		//We have a ZC down
		posZcDown[numZcDown]=getZeroCrossingPoint(timeVals[surf][samp][rcoGuess],firstVal,timeVals[surf][samp+1][rcoGuess],secondVal);
		numZcDown++;
	      }
	      if((firstVal<=0 && secondVal>=0)) {  
		//We have a ZC up
		posZcUp[numZcUp]=getZeroCrossingPoint(timeVals[surf][samp][rcoGuess],firstVal,timeVals[surf][samp+1][rcoGuess],secondVal);
		numZcUp++;
	      }
	    }

	    deltaTSecondRco[surf][rcoGuess]=0;
	    numDeltaTSecondRco[surf][rcoGuess]=0;
	    if(numZcDown>1) {	       
	      for(int i=1;i<numZcDown;i++) {
		if((posZcDown[i]-posZcDown[i-1])>(clockPeriod-0.5) && (posZcDown[i]-posZcDown[i-1])<(clockPeriod+0.5)) {
		  deltaTSecondRco[surf][rcoGuess]+=(posZcDown[i]-posZcDown[i-1]);
		  numDeltaTSecondRco[surf][rcoGuess]++;
		}
	      }
	    }
	    if(numZcUp>1) {	       
	      for(int i=1;i<numZcUp;i++) {
		if((posZcUp[i]-posZcUp[i-1])>(clockPeriod-0.5) && (posZcUp[i]-posZcUp[i-1])<(clockPeriod+0.5)) {
		  deltaTSecondRco[surf][rcoGuess]+=(posZcUp[i]-posZcUp[i-1]);
		  numDeltaTSecondRco[surf][rcoGuess]++;
		}
	      }
	    }
	    if(numDeltaTSecondRco[surf][rcoGuess]>0) {
	      deltaTSecondRco[surf][rcoGuess]/=numDeltaTSecondRco[surf][rcoGuess];
	    }
	    else {
	      deltaTSecondRco[surf][rcoGuess]=0;
	    }		  		  
	  }
	}
      }
      else {
	//Only one RCO
	//	   In this case rcoGuess==0 means that the rco for the whole segment is zero
	//	   In this case rcoGuess==1 means that the rco for the whole segment is one
	Double_t posZcUp[100]={0};
	Double_t posZcDown[100]={0};
	Int_t numZcUp=0;
	Int_t numZcDown=0;
	for(int samp=earliestSample;samp<latestSample;samp++) {
	  Double_t firstVal=this->data[clockIndex][samp]-offset;
	  Double_t secondVal=this->data[clockIndex][samp+1]-offset;
	  if((firstVal>=0 && secondVal<=0)) {  
	    //We have a ZC down
	    posZcDown[numZcDown]=getZeroCrossingPoint(timeVals[surf][samp][rcoGuess],firstVal,timeVals[surf][samp+1][rcoGuess],secondVal);
	    numZcDown++;
	  }
	  if((firstVal<=0 && secondVal>=0)) {  
	    //We have a ZC up
	    posZcUp[numZcUp]=getZeroCrossingPoint(timeVals[surf][samp][rcoGuess],firstVal,timeVals[surf][samp+1][rcoGuess],secondVal);
	    numZcUp++;
	  }
	}

	deltaTFirstRco[surf][rcoGuess]=0;
	if(numZcDown>1) {	       
	  for(int i=1;i<numZcDown;i++) {
	    if((posZcDown[i]-posZcDown[i-1])>(clockPeriod-0.5) && (posZcDown[i]-posZcDown[i-1])<(clockPeriod+0.5)) {
	      deltaTFirstRco[surf][rcoGuess]+=(posZcDown[i]-posZcDown[i-1]);
	      numDeltaTFirstRco[surf][rcoGuess]++;
	    }
	  }
	}
	if(numZcUp>1) {	       
	  for(int i=1;i<numZcUp;i++) {
	    if((posZcUp[i]-posZcUp[i-1])>(clockPeriod-0.5) && (posZcUp[i]-posZcUp[i-1])<(clockPeriod+0.5)) {
	      deltaTFirstRco[surf][rcoGuess]+=(posZcUp[i]-posZcUp[i-1]);
	      numDeltaTFirstRco[surf][rcoGuess]++;
	    }
	  }
	}
	if(numDeltaTFirstRco[surf][rcoGuess]>0) {
	  deltaTFirstRco[surf][rcoGuess]/=numDeltaTFirstRco[surf][rcoGuess];
	}
	else {
	  deltaTFirstRco[surf][rcoGuess]=0;
	}
	    
      }
    }
  }

  if(!gotCalibTemp) {
    //Somehow need to guess at the temperature calibration
    //Found deltaT's now need to think about what to do
    Double_t avgDelta[2]={0};
    Int_t numDeltaT[2]={0};
    for(int surf=0;surf<NUM_SURF;surf++) {
      if(skipSurf[surf]) continue;
      for(int rco=0;rco<2;rco++) {
	avgDelta[rco]+=deltaTFirstRco[surf][rco]*numDeltaTFirstRco[surf][rco];
	numDeltaT[rco]+=numDeltaTFirstRco[surf][rco];
	avgDelta[rco]+=deltaTSecondRco[surf][rco]*numDeltaTSecondRco[surf][rco];
	numDeltaT[rco]+=numDeltaTSecondRco[surf][rco];
      }
    }
    avgDelta[0]/=numDeltaT[0];
    avgDelta[1]/=numDeltaT[1];
    //    std::cout << avgDelta[0] << "\t" << avgDelta[1] << "\t" 
    //	      << 0.5*(avgDelta[0]+avgDelta[1]) << "\n";
    Double_t fullAvgDt=0.5*(avgDelta[0]+avgDelta[1]);
    tempFactor=clockPeriod/fullAvgDt;
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int rco=0;rco<2;rco++) {
	deltaTFirstRco[surf][rco]*=tempFactor;
	deltaTSecondRco[surf][rco]*=tempFactor;
      }
    }
  }

  Double_t finalDt=0;   
  Int_t numFinalDt=0;
  for(int surf=0;surf<NUM_SURF;surf++) {
    if(skipSurf[surf]) continue;
    if(numDeltaTFirstRco[surf][0]>0 && numDeltaTFirstRco[surf][1]>0 && numDeltaTSecondRco[surf][0]>0 && numDeltaTSecondRco[surf][1]>0) {
      //Okay now we should just be able to compare the values;
      if(TMath::Abs(deltaTFirstRco[surf][0]-deltaTSecondRco[surf][0])<
	 TMath::Abs(deltaTFirstRco[surf][1]-deltaTSecondRco[surf][1])) {
	fRcoArray[surf]=0;
      }
      else {
	fRcoArray[surf]=1;
      }

      //Okay now lets check we haven't messed up
      if(numDeltaTFirstRco[surf][0]>15 && numDeltaTFirstRco[surf][0]>15 && numDeltaTSecondRco[surf][0]<3 && numDeltaTSecondRco[surf][1]<3) {
	if(
	   (numDeltaTFirstRco[surf][1-fRcoArray[surf]]*TMath::Abs(deltaTFirstRco[surf][1-fRcoArray[surf]]-clockPeriod)+numDeltaTSecondRco[surf][1-fRcoArray[surf]]*TMath::Abs(deltaTSecondRco[surf][1-fRcoArray[surf]]-clockPeriod))/(numDeltaTFirstRco[surf][1-fRcoArray[surf]]+numDeltaTSecondRco[surf][1-fRcoArray[surf]]) 
	   <
	   (numDeltaTFirstRco[surf][fRcoArray[surf]]*TMath::Abs(deltaTFirstRco[surf][fRcoArray[surf]]-clockPeriod)+numDeltaTSecondRco[surf][fRcoArray[surf]]*TMath::Abs(deltaTSecondRco[surf][fRcoArray[surf]]-clockPeriod))/(numDeltaTFirstRco[surf][fRcoArray[surf]]+numDeltaTSecondRco[surf][fRcoArray[surf]])
	   ) { 
	  //Oops	      
	  fRcoArray[surf]=1-fRcoArray[surf];
	}
      }

      //Okay now lets check we haven't messed up
      if(numDeltaTSecondRco[surf][0]>15 && numDeltaTSecondRco[surf][0]>15 && numDeltaTFirstRco[surf][0]<3 && numDeltaTFirstRco[surf][1]<3) {
	if(
	   (numDeltaTFirstRco[surf][1-fRcoArray[surf]]*TMath::Abs(deltaTFirstRco[surf][1-fRcoArray[surf]]-clockPeriod)+numDeltaTSecondRco[surf][1-fRcoArray[surf]]*TMath::Abs(deltaTSecondRco[surf][1-fRcoArray[surf]]-clockPeriod))/(numDeltaTFirstRco[surf][1-fRcoArray[surf]]+numDeltaTSecondRco[surf][1-fRcoArray[surf]]) 
	   <
	   (numDeltaTFirstRco[surf][fRcoArray[surf]]*TMath::Abs(deltaTFirstRco[surf][fRcoArray[surf]]-clockPeriod)+numDeltaTSecondRco[surf][fRcoArray[surf]]*TMath::Abs(deltaTSecondRco[surf][fRcoArray[surf]]-clockPeriod))/(numDeltaTFirstRco[surf][fRcoArray[surf]]+numDeltaTSecondRco[surf][fRcoArray[surf]])
	   ) {
	  //Oops	      
	  fRcoArray[surf]=1-fRcoArray[surf];
	}
      }

    }
    else if(numDeltaTFirstRco[surf][0]>0 && numDeltaTFirstRco[surf][1]>0) {
      //We only have one trace segment so we'll compare the period to clockPeriod
      if(TMath::Abs(deltaTFirstRco[surf][0]-clockPeriod)<
	 TMath::Abs(deltaTFirstRco[surf][1]-clockPeriod)) {
	fRcoArray[surf]=0;
      }
      else {
	fRcoArray[surf]=1;
      }	
    }
    else if(numDeltaTSecondRco[surf][0]>0 && numDeltaTSecondRco[surf][1]>0) {
      //We only have one trace segment so we'll compare the period to clockPeriod
      if(TMath::Abs(deltaTSecondRco[surf][0]-clockPeriod)<
	 TMath::Abs(deltaTSecondRco[surf][1]-clockPeriod)) {
	fRcoArray[surf]=0;
      }
      else {
	fRcoArray[surf]=1;
      }
	
    }
    else {	   
      fRcoArray[surf]=0;
      //We are right up the smelly stuff
      std::cerr << "Event " << this->eventNumber << " has broken RCO for SURF " << surf << "\n";
      std::cerr << "Event " << this->eventNumber << " has broken RCO for SURF " << surf << "\t" << numDeltaTFirstRco[surf][fRcoArray[surf]] << "\t" << numDeltaTSecondRco[surf][fRcoArray[surf]] << "\t" << deltaTFirstRco[surf][fRcoArray[surf]] << "\t" << deltaTSecondRco[surf][fRcoArray[surf]] << "\n";
    }  

    finalDt+=deltaTFirstRco[surf][fRcoArray[surf]]*numDeltaTFirstRco[surf][fRcoArray[surf]]/tempFactor;
    numFinalDt+=numDeltaTFirstRco[surf][fRcoArray[surf]];
    finalDt+=deltaTSecondRco[surf][fRcoArray[surf]]*numDeltaTSecondRco[surf][fRcoArray[surf]]/tempFactor;
    numFinalDt+=numDeltaTSecondRco[surf][fRcoArray[surf]];
  }
  finalDt/=numFinalDt;
  fTempFactorGuess=clockPeriod/finalDt;
  //    std::cout << fTempFactorGuess << "\n";

  if(!gotCalibTemp) {
    //Now we'll have another stab at the RCO's just for fun.  
    finalDt=0;
    numFinalDt=0;
    for(int surf=0;surf<NUM_SURF;surf++) {
      if(skipSurf[surf]) continue;
      for(int rco=0;rco<2;rco++) {
	deltaTFirstRco[surf][rco]*=fTempFactorGuess/tempFactor;
	deltaTSecondRco[surf][rco]*=fTempFactorGuess/tempFactor;
      }


	  
      if(numDeltaTFirstRco[surf][0]>0 && numDeltaTFirstRco[surf][1]>0 && numDeltaTSecondRco[surf][0]>0 && numDeltaTSecondRco[surf][1]>0) {
	//Okay now we should just be able to compare the values;
	if(TMath::Abs(deltaTFirstRco[surf][0]-deltaTSecondRco[surf][0])<
	   TMath::Abs(deltaTFirstRco[surf][1]-deltaTSecondRco[surf][1])) {
	  fRcoArray[surf]=0;
	}
	else {
	  fRcoArray[surf]=1;
	}

	//Okay now lets check we haven't messed up
	if(numDeltaTFirstRco[surf][0]>15 && numDeltaTFirstRco[surf][0]>15 && numDeltaTSecondRco[surf][0]<3 && numDeltaTSecondRco[surf][1]<3) {
	  if(
	     (numDeltaTFirstRco[surf][1-fRcoArray[surf]]*TMath::Abs(deltaTFirstRco[surf][1-fRcoArray[surf]]-clockPeriod)+numDeltaTSecondRco[surf][1-fRcoArray[surf]]*TMath::Abs(deltaTSecondRco[surf][1-fRcoArray[surf]]-clockPeriod))/(numDeltaTFirstRco[surf][1-fRcoArray[surf]]+numDeltaTSecondRco[surf][1-fRcoArray[surf]]) 
	     <
	     (numDeltaTFirstRco[surf][fRcoArray[surf]]*TMath::Abs(deltaTFirstRco[surf][fRcoArray[surf]]-clockPeriod)+numDeltaTSecondRco[surf][fRcoArray[surf]]*TMath::Abs(deltaTSecondRco[surf][fRcoArray[surf]]-clockPeriod))/(numDeltaTFirstRco[surf][fRcoArray[surf]]+numDeltaTSecondRco[surf][fRcoArray[surf]])
	     ) { 
	    //Oops	      
	    fRcoArray[surf]=1-fRcoArray[surf];
	  }
	}

	//Okay now lets check we haven't messed up
	if(numDeltaTSecondRco[surf][0]>15 && numDeltaTSecondRco[surf][0]>15 && numDeltaTFirstRco[surf][0]<3 && numDeltaTFirstRco[surf][1]<3) {
	  if(
	     (numDeltaTFirstRco[surf][1-fRcoArray[surf]]*TMath::Abs(deltaTFirstRco[surf][1-fRcoArray[surf]]-clockPeriod)+numDeltaTSecondRco[surf][1-fRcoArray[surf]]*TMath::Abs(deltaTSecondRco[surf][1-fRcoArray[surf]]-clockPeriod))/(numDeltaTFirstRco[surf][1-fRcoArray[surf]]+numDeltaTSecondRco[surf][1-fRcoArray[surf]]) 
	     <
	     (numDeltaTFirstRco[surf][fRcoArray[surf]]*TMath::Abs(deltaTFirstRco[surf][fRcoArray[surf]]-clockPeriod)+numDeltaTSecondRco[surf][fRcoArray[surf]]*TMath::Abs(deltaTSecondRco[surf][fRcoArray[surf]]-clockPeriod))/(numDeltaTFirstRco[surf][fRcoArray[surf]]+numDeltaTSecondRco[surf][fRcoArray[surf]])
	     ) {
	    //Oops	      
	    fRcoArray[surf]=1-fRcoArray[surf];
	  }
	}

      }
      else if(numDeltaTFirstRco[surf][0]>0 && numDeltaTFirstRco[surf][1]>0) {
	//We only have one trace segment so we'll compare the period to clockPeriod
	if(TMath::Abs(deltaTFirstRco[surf][0]-clockPeriod)<
	   TMath::Abs(deltaTFirstRco[surf][1]-clockPeriod)) {
	  fRcoArray[surf]=0;
	}
	else {
	  fRcoArray[surf]=1;
	}	
      }
      else if(numDeltaTSecondRco[surf][0]>0 && numDeltaTSecondRco[surf][1]>0) {
	//We only have one trace segment so we'll compare the period to clockPeriod
	if(TMath::Abs(deltaTSecondRco[surf][0]-clockPeriod)<
	   TMath::Abs(deltaTSecondRco[surf][1]-clockPeriod)) {
	  fRcoArray[surf]=0;
	}
	else {
	  fRcoArray[surf]=1;
	}
	
      }
      else {	   
	fRcoArray[surf]=0;
	//We are right up the smelly stuff
	std::cerr << "Event " << this->eventNumber << " has broken RCO for SURF " << surf << "\n";
	//	std::cerr << "Event " << this->eventNumber << " has broken RCO for SURF " << surf << "\t" << numDeltaTFirstRco << "\t" << numDeltaTSecondRco << "\t" << deltaTFirstRco << "\t" << deltaTSecondRco << "\n";
	std::cerr << "Event " << this->eventNumber << " has broken RCO for SURF " << surf << "\t" << numDeltaTFirstRco[surf][fRcoArray[surf]] << "\t" << numDeltaTSecondRco[surf][fRcoArray[surf]] << "\t" << deltaTFirstRco[surf][fRcoArray[surf]] << "\t" << deltaTSecondRco[surf][fRcoArray[surf]] << "\n";
      }  

      finalDt+=deltaTFirstRco[surf][fRcoArray[surf]]*numDeltaTFirstRco[surf][fRcoArray[surf]]/fTempFactorGuess;
      numFinalDt+=numDeltaTFirstRco[surf][fRcoArray[surf]];
      finalDt+=deltaTSecondRco[surf][fRcoArray[surf]]*numDeltaTSecondRco[surf][fRcoArray[surf]]/fTempFactorGuess;
      numFinalDt+=numDeltaTSecondRco[surf][fRcoArray[surf]];
	  

    }

    finalDt/=numFinalDt;
    fTempFactorGuess=clockPeriod/finalDt;
  }
       

  fLastEventGuessed=this->eventNumber;
}


// Int_t UsefulAnitaEvent::guessRcoRun3871Old(int chanIndex)
// {
// #define START_EVENT 31853801
//   static UInt_t lastEvent=0;
//   static Int_t fRcoArray[NUM_SURF];   
//   static TF1 *myFakeTemp=0;
//   if(lastEvent!=this->eventNumber) {
//     AnitaEventCalibrator *myCally = AnitaEventCalibrator::Instance();
//     Double_t timeVals[NUM_SURF][NUM_SAMP]={0};
//     Int_t labChip=this->getLabChip(chanIndex);
      
//     if(!myFakeTemp) {	 
//       myFakeTemp= new TF1("myFakeTemp","[0] + [1]*exp(-x*[2])",0,100000);
//       myFakeTemp->SetParameters(8.07,0.13,1./30000);
//     }
      
//     Double_t tempFactor=myFakeTemp->Eval(100000)/myFakeTemp->Eval(this->eventNumber-START_EVENT);
//     tempFactor=1;

//     for(int surf=0;surf<NUM_SURF;surf++) {
//       //Fill in timebase
//       Double_t timeRco=0;
//       for(int samp=0;samp<NUM_SAMP;samp++) {

// 	timeVals[surf][samp]=timeRco;
// 	timeRco+=myCally->justBinByBin[surf][labChip][0][samp]*tempFactor;

//       }

//       //Now we can try and find the zero crossings and periods.
//       Int_t clockIndex=9*surf + 8;
//       Int_t earliestSample=this->getEarliestSample(clockIndex);
//       Int_t latestSample=this->getLatestSample(clockIndex);
//       Double_t offset=TMath::Mean(NUM_SAMP,this->data[clockIndex]);
	 
//       Double_t deltaTFirstRco=0;
//       Int_t numDeltaTFirstRco=0;
//       Double_t deltaTSecondRco=0;
//       Int_t numDeltaTSecondRco=0;

//       if(latestSample<earliestSample) {
// 	//We have two RCOs
// 	{
// 	  //Here is the first RCO
// 	  Double_t posZcUp[100]={0};
// 	  Double_t posZcDown[100]={0};
// 	  Int_t numZcUp=0;
// 	  Int_t numZcDown=0;	    
// 	  for(int samp=earliestSample;samp<259;samp++) {
// 	    Double_t firstVal=this->data[clockIndex][samp]-offset;
// 	    Double_t secondVal=this->data[clockIndex][samp+1]-offset;
// 	    if((firstVal>=0 && secondVal<=0)) {  
// 	      //We have a ZC down
// 	      posZcDown[numZcDown]=getZeroCrossingPoint(timeVals[surf][samp],firstVal,timeVals[surf][samp+1],secondVal);
// 	      numZcDown++;
// 	    }
// 	    if((firstVal<=0 && secondVal>=0)) {  
// 	      //We have a ZC up
// 	      posZcUp[numZcUp]=getZeroCrossingPoint(timeVals[surf][samp],firstVal,timeVals[surf][samp+1],secondVal);
// 	      numZcUp++;
// 	    }
// 	  }
	    
// 	  deltaTFirstRco=0;
// 	  if(numZcDown>1) {	       
// 	    for(int i=1;i<numZcDown;i++) {
// 	      if((posZcDown[i]-posZcDown[i-1])>(clockPeriod-0.5) && (posZcDown[i]-posZcDown[i-1])<(clockPeriod+0.5)) {
// 		deltaTFirstRco+=(posZcDown[i]-posZcDown[i-1]);
// 		numDeltaTFirstRco++;
// 	      }
// 	    }
// 	  }
// 	  if(numZcUp>1) {	       
// 	    for(int i=1;i<numZcUp;i++) {
// 	      if((posZcUp[i]-posZcUp[i-1])>(clockPeriod-0.5) && (posZcUp[i]-posZcUp[i-1])<(clockPeriod+0.5)) {
// 		deltaTFirstRco+=(posZcUp[i]-posZcUp[i-1]);
// 		numDeltaTFirstRco++;
// 	      }
// 	    }
// 	  }
// 	  if(numDeltaTFirstRco>0) {
// 	    deltaTFirstRco/=numDeltaTFirstRco;
// 	  }
// 	  else {
// 	    deltaTFirstRco=0;
// 	  }
// 	}

// 	{
// 	  //Now lets do the second RCO
	       
// 	  Double_t posZcUp[100]={0};
// 	  Double_t posZcDown[100]={0};
// 	  Int_t numZcUp=0;
// 	  Int_t numZcDown=0;
// 	  if(latestSample>0) {
// 	    for(int samp=0;samp<latestSample;samp++) {
// 	      Double_t firstVal=this->data[clockIndex][samp]-offset;
// 	      Double_t secondVal=this->data[clockIndex][samp+1]-offset;
// 	      if((firstVal>=0 && secondVal<=0)) {  
// 		//We have a ZC down
// 		posZcDown[numZcDown]=getZeroCrossingPoint(timeVals[surf][samp],firstVal,timeVals[surf][samp+1],secondVal);
// 		numZcDown++;
// 	      }
// 	      if((firstVal<=0 && secondVal>=0)) {  
// 		//We have a ZC up
// 		posZcUp[numZcUp]=getZeroCrossingPoint(timeVals[surf][samp],firstVal,timeVals[surf][samp+1],secondVal);
// 		numZcUp++;
// 	      }
// 	    }

// 	    deltaTSecondRco=0;
// 	    if(numZcDown>1) {	       
// 	      for(int i=1;i<numZcDown;i++) {
// 		if((posZcDown[i]-posZcDown[i-1])>(clockPeriod-0.5) && (posZcDown[i]-posZcDown[i-1])<(clockPeriod+0.5)) {
// 		  deltaTSecondRco+=(posZcDown[i]-posZcDown[i-1]);
// 		  numDeltaTSecondRco++;
// 		}
// 	      }
// 	    }
// 	    if(numZcUp>1) {	       
// 	      for(int i=1;i<numZcUp;i++) {
// 		if((posZcUp[i]-posZcUp[i-1])>(clockPeriod-0.5) && (posZcUp[i]-posZcUp[i-1])<(clockPeriod+0.5)) {
// 		  deltaTSecondRco+=(posZcUp[i]-posZcUp[i-1]);
// 		  numDeltaTSecondRco++;
// 		}
// 	      }
// 	    }
// 	    if(numDeltaTSecondRco>0) {
// 	      deltaTSecondRco/=numDeltaTSecondRco;
// 	    }
// 	    else {
// 	      deltaTSecondRco=0;
// 	    }		  		  
// 	  }
// 	}
//       }
//       else {
// 	//Only one RCO

// 	Double_t posZcUp[100]={0};
// 	Double_t posZcDown[100]={0};
// 	Int_t numZcUp=0;
// 	Int_t numZcDown=0;
// 	for(int samp=earliestSample;samp<latestSample;samp++) {
// 	  Double_t firstVal=this->data[clockIndex][samp]-offset;
// 	  Double_t secondVal=this->data[clockIndex][samp+1]-offset;
// 	  if((firstVal>=0 && secondVal<=0)) {  
// 	    //We have a ZC down
// 	    posZcDown[numZcDown]=getZeroCrossingPoint(timeVals[surf][samp],firstVal,timeVals[surf][samp+1],secondVal);
// 	    numZcDown++;
// 	  }
// 	  if((firstVal<=0 && secondVal>=0)) {  
// 	    //We have a ZC up
// 	    posZcUp[numZcUp]=getZeroCrossingPoint(timeVals[surf][samp],firstVal,timeVals[surf][samp+1],secondVal);
// 	    numZcUp++;
// 	  }
// 	}

// 	deltaTFirstRco=0;
// 	if(numZcDown>1) {	       
// 	  for(int i=1;i<numZcDown;i++) {
// 	    if((posZcDown[i]-posZcDown[i-1])>(clockPeriod-0.5) && (posZcDown[i]-posZcDown[i-1])<(clockPeriod+0.5)) {
// 	      deltaTFirstRco+=(posZcDown[i]-posZcDown[i-1]);
// 	      numDeltaTFirstRco++;
// 	    }
// 	  }
// 	}
// 	if(numZcUp>1) {	       
// 	  for(int i=1;i<numZcUp;i++) {
// 	    if((posZcUp[i]-posZcUp[i-1])>(clockPeriod-0.5) && (posZcUp[i]-posZcUp[i-1])<(clockPeriod+0.5)) {
// 	      deltaTFirstRco+=(posZcUp[i]-posZcUp[i-1]);
// 	      numDeltaTFirstRco++;
// 	    }
// 	  }
// 	}
// 	if(numDeltaTFirstRco>0) {
// 	  deltaTFirstRco/=numDeltaTFirstRco;
// 	}
// 	else {
// 	  deltaTFirstRco=0;
// 	}
	    
//       }
	 
//       //Okay lets turn to the magic prognosticator
//       if(numDeltaTFirstRco>0 && numDeltaTSecondRco>0) {
// 	//Now we can see if one is high and one is low
// 	if(deltaTFirstRco>clockPeriod && deltaTSecondRco<clockPeriod) {
// 	  //Wahoo lets celebrate
// 	  fRcoArray[surf]=1; //Arbitrarily define the faster phase as 1
// 	}
// 	else if(deltaTFirstRco<clockPeriod && deltaTFirstRco>clockPeriod) {
// 	  //Wahoo lets celebrate
// 	  fRcoArray[surf]=0; //Arbitrarily define the faster phase as 1
// 	}
// 	else if(numDeltaTFirstRco > numDeltaTSecondRco) {
// 	  //Then well just assume the first one is correct
// 	  if(deltaTFirstRco<clockPeriod)
// 	    fRcoArray[surf]=0;
// 	  else 
// 	    fRcoArray[surf]=1;
// 	}
// 	else if(numDeltaTFirstRco < numDeltaTSecondRco) {
// 	  //Then well just assume the second one is correct
// 	  if(deltaTSecondRco<clockPeriod)
// 	    fRcoArray[surf]=1;
// 	  else 
// 	    fRcoArray[surf]=0;
// 	}
// 	else { 
// 	  if(numDeltaTFirstRco>5) {
// 	    if(deltaTFirstRco>deltaTSecondRco)
// 	      fRcoArray[surf]=1;
// 	    else
// 	      fRcoArray[surf]=0;
// 	  }
// 	  else {
// 	    fRcoArray[surf]=0;
// 	    std::cerr << "Event " << this->eventNumber << " has broken RCO for SURF " << surf << "\t" << numDeltaTFirstRco << "\t" << numDeltaTSecondRco << "\t" << deltaTFirstRco << "\t" << deltaTSecondRco << "\n";
// 	  }
// 	}
//       }
//       else if(numDeltaTFirstRco>0) {
// 	//Then well just assume the first one is correct
// 	if(deltaTFirstRco<clockPeriod)
// 	  fRcoArray[surf]=0;
// 	else 
// 	  fRcoArray[surf]=1;	    
//       }
//       else if(numDeltaTSecondRco>0) {
// 	//Then well just assume the second one is correct
// 	if(deltaTSecondRco<clockPeriod)
// 	  fRcoArray[surf]=1;
// 	else 
// 	  fRcoArray[surf]=0;
//       }
//       else {
// 	fRcoArray[surf]=0;
// 	//We are right up the smelly stuff
// 	std::cerr << "Event " << this->eventNumber << " has broken RCO for SURF " << surf << "\n";
// 	std::cerr << "Event " << this->eventNumber << " has broken RCO for SURF " << surf << "\t" << numDeltaTFirstRco << "\t" << numDeltaTSecondRco << "\t" << deltaTFirstRco << "\t" << deltaTSecondRco << "\n";
//       }  
//       //	 if(surf==1) {
//       //	  std::cout << fRcoArray[surf] << "\t" << deltaTFirstRco << "\t" << deltaTSecondRco << "\n";
//       //	 }
//     }
//     lastEvent=this->eventNumber;
//   }
//   int surf=chanIndex/9;
//   //   std::cout << surf << "\t" << chanIndex << "\t" << fRcoArray[surf] << std::endl;
//   return fRcoArray[surf];
// }
