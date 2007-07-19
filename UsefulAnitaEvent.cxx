//////////////////////////////////////////////////////////////////////////////
/////  UsefulAnitaEvent.cxx        ANITA event reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in useful ANITA events and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "UsefulAnitaEvent.h"
#include "AnitaGeomTool.h"
#include <iostream>
#include <fstream>

ClassImp(UsefulAnitaEvent);

UsefulAnitaEvent::UsefulAnitaEvent() 
   : RawAnitaEvent()
{
   gotCalibTemp=0;
   //Default Constructor
}

UsefulAnitaEvent::UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, PrettyAnitaHk *theHk) 
   : RawAnitaEvent(*eventPtr)
{
   if(theHk) {
      gotCalibTemp=1;
      calibTemp=theHk->intTemps[2];
   }
   else 
      gotCalibTemp=0;
      
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
   case WaveCalType::kVoltageTime:
      //Just use the nominal 1mv per count, and 2.6GSamps/sec numbers     
      for(int surf=0;surf<NUM_SURF;surf++) {
	 for(int chan=0;chan<NUM_CHAN;chan++) {
	    int chanIndex=getChanIndex(surf,chan);
	    this->fNumPoints[chanIndex]=fCalibrator->numPointsArray[surf][chan];
	    memset(&(this->fVolts[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	    memset(&(this->fTimes[chanIndex][0]),0,NUM_SAMP*sizeof(double));
	    for(int samp=0;samp<this->fNumPoints[chanIndex];samp++) {
	       this->fVolts[chanIndex][samp]=fCalibrator->unwrappedArray[surf][chan][samp];
	       this->fTimes[chanIndex][samp]=float(samp)/2.6;
	    }
	 }
      }
      break;
   case WaveCalType::kVTLabRG:
      //All my lovely numbers    
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
	       this->fTimes[chanIndex][samp]=fCalibrator->timeArray[surf][chan][samp]+
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
      //All Jiwoo's lovely numbers + cable delay
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


TGraph *UsefulAnitaEvent::getGraph(int chanIndex)
{
   if(chanIndex<0 || chanIndex>=NUM_DIGITZED_CHANNELS)
      return NULL;
   
   TGraph *grPtr=new TGraph(fNumPoints[chanIndex],
				fTimes[chanIndex],
				fVolts[chanIndex]);
   return grPtr;
}

TGraph *UsefulAnitaEvent::getGraph(int ant, AnitaPol::AnitaPol_t pol)
{
   return getGraph(AnitaGeomTool::getChanIndexFromAntPol(ant,pol));

}

TGraph *UsefulAnitaEvent::getGraph(AnitaRing::AnitaRing_t ring,
				   int phi,
				   AnitaPol::AnitaPol_t pol)
{
   return getGraph(AnitaGeomTool::getChanIndexFromRingPhiPol(ring,phi,pol));

}
