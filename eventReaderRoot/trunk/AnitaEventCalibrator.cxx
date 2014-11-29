//////////////////////////////////////////////////////////////////////////////
/////  AnitaEventCalibrator.cxx        ANITA Event Calibrator            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for calibrating UsefulAnitaEvents               /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include <cstring>

#include <TMath.h>
#include <TGraph.h>

#include "AnitaEventCalibrator.h"
#include "AnitaGeomTool.h"
#include "UsefulAnitaEvent.h"

#ifdef USE_FFT_TOOLS
#include "FFTtools.h"
#endif

//Clock Period Hard Coded
//Double_t clockPeriod=1000/33.;
Double_t clockPeriod=29.9964121338995078;

//Fitting function
Double_t funcSquareWave(Double_t *x, Double_t *par)
{
   Double_t phi=par[0];
   Double_t a=par[1];
   Double_t b=par[2];
   Double_t dtLow=par[3];
   Double_t dtHigh=par[4];
   Double_t sllh=par[5];
   Double_t slhl=par[6];


   Double_t period=dtLow+dtHigh+sllh+slhl;

   Double_t t=x[0]-phi;
   
   Double_t mlh=(a-b)/sllh;
   Double_t mhl=(b-a)/slhl;

   while(t<0) {
      t+=period;
   }
   while(t>period) {
      t-=period;
   }
   if(t<dtLow)
      return b;
   if(t<dtLow+sllh) {
      Double_t t1=t-dtLow;
      return (t1*mlh)+b;
   }
   if(t<dtLow+sllh+dtHigh)
      return a;
   if(t<dtLow+sllh+dtHigh+slhl) {
      Double_t t2=t-(dtLow+sllh+dtHigh);
      return (mhl*t2)+a;
   }
   
      
   return a;

}


Double_t newFuncSquareWave(Double_t *x, Double_t *par)
{
   Double_t phi=par[0];
   Double_t a=par[1];
   Double_t b=par[2];

   Double_t sllh=par[4];
   Double_t slhl=par[4];
   
   Double_t periodLeft=clockPeriod-2*par[4];   
   Double_t dtLow=par[3]*periodLeft;
   Double_t dtHigh=(1-par[3])*periodLeft;


   Double_t t=x[0]-phi;
   
   Double_t mlh=(a-b)/sllh;
   Double_t mhl=(b-a)/slhl;

   while(t<0) {
      t+=clockPeriod;
   }
   while(t>clockPeriod) {
      t-=clockPeriod;
   }
   if(t<dtLow)
      return b;
   if(t<dtLow+sllh) {
      Double_t t1=t-dtLow;
      return (t1*mlh)+b;
   }
   if(t<dtLow+sllh+dtHigh)
      return a;
   if(t<dtLow+sllh+dtHigh+slhl) {
      Double_t t2=t-(dtLow+sllh+dtHigh);
      return (mhl*t2)+a;
   }
   
      
   return a;



}




ClassImp(AnitaEventCalibrator);

AnitaEventCalibrator*  AnitaEventCalibrator::fgInstance = 0;


AnitaEventCalibrator::AnitaEventCalibrator()
   : TObject()
{
   fSquareWave=0;
   fFakeTemp=0;
   fClockUpSampleFactor=16;
   fEpsilonTempScale=1;
   //Default constructor
   std::cout << "AnitaEventCalibrator::AnitaEventCalibrator()" << std::endl;
   loadCalib();
   //   std::cout << "AnitaEventCalibrator::AnitaEventCalibrator() end" << std::endl;
   for(int surf=1;surf<NUM_SURF;surf++) {
     grCorClock[surf-1]=0;
   }
   fLastEventNumber = 0; /* Keep track of rolling events starting from first instance */
   fTempEventInd = 0;
   fNumEventsAveraged = 0;

}

AnitaEventCalibrator::~AnitaEventCalibrator()
{
   //Default destructor
}



//______________________________________________________________________________
AnitaEventCalibrator*  AnitaEventCalibrator::Instance()
{
   //static function
  if(fgInstance)
    return fgInstance;

  fgInstance = new AnitaEventCalibrator();

  return fgInstance;
  //   return (fgInstance) ? (AnitaEventCalibrator*) fgInstance : new AnitaEventCalibrator();
}



int AnitaEventCalibrator::calibrateUsefulEvent(UsefulAnitaEvent *eventPtr, WaveCalType::WaveCalType_t calType)
{  
  if(calType==WaveCalType::kJustTimeNoUnwrap)
    return justBinByBinTimebase(eventPtr);
  
   fApplyClockFudge=0;
   //   std::cout << "AnitaEventCalibrator::calibrateUsefulEvent():" << calType << std::endl;
   if(calType==WaveCalType::kVoltageTime || calType==WaveCalType::kVTLabAG) {
     processEventUnwrapFast(eventPtr);
   }
   else if( calType==WaveCalType::kVTLabAGFastClock || calType==WaveCalType::kVTLabAGCrossCorClock || calType==WaveCalType::kVTFullAGFastClock || calType==WaveCalType::kVTFullAGCrossCorClock || calType==WaveCalType::kVTCalFilePlusSimon ) {
     //processEventAG(eventPtr);
     processEventAG(eventPtr, 1, 0, 0); /* New args tweak timing calibration, 1,0,0 means same as before. */
   }
   else if(calType==WaveCalType::kVTBenS || calType==WaveCalType::kVTBenSNoClockJitterNoZeroMean){
     processEventBS(eventPtr);
   }
   else {
     justBinByBinTimebase(eventPtr);
   }

   
   if(calType>=WaveCalType::kVTInCalibratedFile && eventPtr->fFromCalibratedAnitaEvent==1) {
     applyClockPhiCorrection(eventPtr);
     
  }
   else { 
     //Clock Jitter correction

     if(calType==WaveCalType::kVTLabAGFastClock || calType==WaveCalType::kVTFullAGFastClock) {
       processClockJitterFast(eventPtr);
     }
   
     if( calType==WaveCalType::kVTLabAGCrossCorClock || calType==WaveCalType::kVTFullAGCrossCorClock 
	 || calType==WaveCalType::kVTBenS) {
       processClockJitterCorrelation(eventPtr);
     }
   } 

   //Zero Mean
   if(
      calType==WaveCalType::kVTLabAGCrossCorClock ||
      calType==WaveCalType::kVTFullAGCrossCorClock ||
      calType==WaveCalType::kVTBenS) {

      zeroMean();
   }
    

   if(calType==WaveCalType::kAddPeds) {
     addPedestals();
   }

   

   //   std::cout << "Done processEvent" << std::endl;
   return 0;
}

void AnitaEventCalibrator::processClockJitter(UsefulAnitaEvent *eventPtr) {
   if(!fSquareWave) {
      fSquareWave = new TF1("fSquareWave",newFuncSquareWave,5,90,5);
      fSquareWave->SetParameters(25,1,-1,0.439,0.33);
      fSquareWave->SetParLimits(0,0,35);
      fSquareWave->SetParLimits(1,1,1);
      fSquareWave->SetParLimits(2,-1,-1);
      fSquareWave->SetParLimits(3,0.439,0.439);
      fSquareWave->SetParLimits(4,0.33,0.33);
      
   }

  Double_t fLowArray[NUM_SAMP];
  Double_t fHighArray[NUM_SAMP];
   Double_t phi0=0;
   Double_t times[NUM_SAMP];
   Double_t volts[NUM_SAMP];


   for(int surf=0;surf<NUM_SURF;surf++) {
      //First fill temp arrays
      Int_t numPoints=numPointsArray[surf][8];
      Int_t numHigh=0;
      Int_t numLow=0;
      for(int samp=0;samp<numPoints;samp++) {
	 if(mvArray[surf][8][samp]>0) {
	    fHighArray[numHigh]=mvArray[surf][8][samp];
	    numHigh++;
	 }
	  else {
	    fLowArray[numLow]=mvArray[surf][8][samp];
	    numLow++;
	  }
      }
      Double_t meanHigh=TMath::Mean(numHigh,fHighArray);
      Double_t meanLow=TMath::Mean(numLow,fLowArray);
       Double_t offset=(meanHigh+meanLow)/2;
       Double_t maxVal=meanHigh-offset;
       //       Double_t minVal=meanLow-offset;

       Int_t gotPhiGuess=0;
       Double_t phiGuess=0;
       for(int i=0;i<numPoints;i++) {
	  times[i]=surfTimeArray[surf][i];
	 Double_t tempV=mvArray[surf][8][i]-offset;	
// 	 if(tempV>maxVal*0.6)
// 	   volts[i]=1;
// 	 else if(tempV<minVal*0.6)
// 	   volts[i]=-1;
// 	 else {
	 volts[i]=tempV/maxVal;
	   //	 }
	 
	 if(!gotPhiGuess) {
	    if(tempV>=0 && (mvArray[surf][8][i+1]-offset)<0) {
	       if(i>3) {
		  phiGuess=times[i];
		  gotPhiGuess=1;
	       }
	    }
	 }
	 
       }

           
      TGraph grTemp(numPoints,times,volts);
      fSquareWave->SetParameter(0,phiGuess);
      grTemp.Fit(fSquareWave,"QR","goff");


      if(surf==0) 
	 phi0=fSquareWave->GetParameter(0);
      
      
      double phi=fSquareWave->GetParameter(0);
      if((phi-phi0)>(clockPeriod/2))
	 phi-=clockPeriod;
      if((phi-phi0)<-1*(clockPeriod/2))
	 phi+=clockPeriod;
      
      Double_t clockCor=phi-phi0;
      clockPhiArray[surf]=(phi-phi0)-clockJitterOffset[surf][fLabChip[surf][8]];
      eventPtr->fClockPhiArray[surf]=clockPhiArray[surf];
      //      std::cout << phi << "\t"  << phi0 << "\t" << clockJitterOffset[surf][fLabChip[surf][8]]
      //		<< std::endl;


      //Now can actually shift times
      // Normal channels are corrected by DeltaPhi - <DeltaPhi>
      // Clock channels are corrected by DeltaPhi (just so the clocks line up)
      for(int chan=0;chan<NUM_CHAN;chan++) {
	 for(int samp=0;samp<numPoints;samp++) {
	    if(chan<8) {
	       timeArray[surf][chan][samp]=surfTimeArray[surf][samp]-clockPhiArray[surf];
	    }
	    else
	       timeArray[surf][chan][samp]=surfTimeArray[surf][samp]-clockCor;
	 }
      }


   }
   
}


void AnitaEventCalibrator::addPedestals() {

  //  int rawArray[NUM_SURF][NUM_CHAN][NUM_SAMP];
  for(int surf=0;surf<NUM_SURF;surf++) {
    for(int chan=0;chan<NUM_CHAN;chan++) {
      int chip=fLabChip[surf][chan];
      for(int samp=0;samp<NUM_SAMP;samp++) {
	rawArray[surf][chan][samp]+=fPedStruct.thePeds[surf][chip][chan][samp];
      }
    }
  }
}


void AnitaEventCalibrator::processClockJitterFast(UsefulAnitaEvent *eventPtr) {
 
  Double_t fLowArray[NUM_SAMP];
  Double_t fHighArray[NUM_SAMP];

  Double_t phi0=0;
  Double_t times[NUM_SAMP];
  Double_t volts[NUM_SAMP];
  
  for(int surf=0;surf<NUM_SURF;surf++) {
    //First fill temp arrays
    Int_t numPoints=numPointsArray[surf][8];
    Int_t numHigh=0;
    Int_t numLow=0;
    for(int samp=0;samp<numPoints;samp++) {
      if(mvArray[surf][8][samp]>0) {
	    fHighArray[numHigh]=mvArray[surf][8][samp];
	    numHigh++;
      }
      else {
	fLowArray[numLow]=mvArray[surf][8][samp];
	numLow++;
      }
    }
    Double_t meanHigh=TMath::Mean(numHigh,fHighArray);
    Double_t meanLow=TMath::Mean(numLow,fLowArray);
    Double_t offset=(meanHigh+meanLow)/2;
    Double_t maxVal=meanHigh-offset;
    //       Double_t minVal=meanLow-offset;
    //       cout << maxVal << "\t" << minVal << endl;
    //       std::cout << offset << "\t" << maxVal << "\t" << minVal << std::endl;
    
    for(int i=0;i<numPoints;i++) {
      times[i]=surfTimeArray[surf][i];
      Double_t tempV=mvArray[surf][8][i]-offset;	
      //	 if(tempV>maxVal*0.9)
      //	   volts[i]=1;
      //	 else if(tempV<minVal*0.9)
      //	   volts[i]=-1;
      //	 else {
      volts[i]=tempV/maxVal;
      //	 }
      
    }
    
    
    Double_t phiGuess=0;
    for(int i=0;i<numPoints-1;i++) {
      if(volts[i]>=0 &&
	 volts[i+1]<0) {
	phiGuess=Get_Interpolation_X(times[i],volts[i],times[i+1],volts[i+1],0);
	//	     	     std::cout << surf << "\t" << 8 << "\t" << times[i] << "\t" << times[i+1] 
	//	     		       << "\t" << volts[i] << "\t" << volts[i+1] << "\t" << phiGuess << std::endl;
	if(i>3)
	  break;
	  }
       }
       
       if(surf==0) 
	  phi0=phiGuess;
       
       double phi=phiGuess;
       if((phi-phi0)>(clockPeriod/2))
	 phi-=clockPeriod;
       if((phi-phi0)<-1*(clockPeriod/2))
	 phi+=clockPeriod;
       
       
       Double_t clockCor=phi-phi0;
       
       while((clockCor-fastClockPeakOffset[surf][fLabChip[surf][8]])>(clockPeriod/2)) {
	 clockCor-=clockPeriod;
       }
        while((clockCor-fastClockPeakOffset[surf][fLabChip[surf][8]])<(-1*clockPeriod/2)) {
	 clockCor+=clockPeriod;
       }

       clockPhiArray[surf]=clockCor;
       eventPtr->fClockPhiArray[surf]=clockPhiArray[surf];
      


       
       //Now can actually shift times
       // Normal channels are corrected by DeltaPhi - <DeltaPhi>
       // Clock channels are corrected by DeltaPhi (just so the clocks line up)
       //       std::cout << "clockCor:\t" << clockCor << "\t" << clockPhiArray[surf] << std::endl;
       for(int chan=0;chan<NUM_CHAN;chan++) {
	  for(int samp=0;samp<numPoints;samp++) {
	     if(chan<8) {
		timeArray[surf][chan][samp]=surfTimeArray[surf][samp]-clockPhiArray[surf];
	     }
	     else
		timeArray[surf][chan][samp]=surfTimeArray[surf][samp]-clockCor;
	  }
       }
       
       
   }
}



void AnitaEventCalibrator::processClockJitterCorrelation(UsefulAnitaEvent *eventPtr) {
#ifdef USE_FFT_TOOLS
   // This calibration works by first normalising the clock to be between -1 and 1
   // then calculates the cross-correlation between SURF 0 and the other SURFs
   // the peak of the cross-corrleation is taken as the time offset between
   // the channels.

   Double_t fLowArray[NUM_SAMP];
   Double_t fHighArray[NUM_SAMP];
   
   Double_t times[NUM_SURF][NUM_SAMP];
   Double_t volts[NUM_SURF][NUM_SAMP];
   TGraph *grClock[NUM_SURF];
   TGraph *grClockFiltered[NUM_SURF];
   for(int surf=0;surf<NUM_SURF;surf++) {
      //First up we normalise the signals

      //By filling temp arrays
      Int_t numPoints=numPointsArray[surf][8];
      Int_t numHigh=0;
      Int_t numLow=0;
      for(int samp=0;samp<numPoints;samp++) {
	 if(mvArray[surf][8][samp]>0) {
	    fHighArray[numHigh]=mvArray[surf][8][samp];
	    numHigh++;
	 }
	  else {
	    fLowArray[numLow]=mvArray[surf][8][samp];
	    numLow++;
	  }
      }
      //Calculating the min, max and midpoints.
      Double_t meanHigh=TMath::Mean(numHigh,fHighArray);
      Double_t meanLow=TMath::Mean(numLow,fLowArray);
      Double_t offset=(meanHigh+meanLow)/2;
      Double_t maxVal=meanHigh-offset;
      //      Double_t minVal=meanLow-offset;

      //       cout << maxVal << "\t" << minVal << endl;
      //       std::cout << offset << "\t" << maxVal << "\t" << minVal << std::endl;
      
      for(int i=0;i<numPoints;i++) {
	 times[surf][i]=surfTimeArray[surf][i];
	 Double_t tempV=mvArray[surf][8][i]-offset;	
	 // This might be reinstated but I need to test to see if the correlation works better
	 // with or without stamping on the overshoot
// 	 if(tempV>maxVal*0.9)
// 	    volts[surf][i]=1;
// 	 else if(tempV<minVal*0.9)
// 	    volts[surf][i]=-1;
// 	 else {
	 volts[surf][i]=tempV/maxVal;
	    //	 }	 
      }
      grClock[surf] = new TGraph(numPoints,times[surf],volts[surf]);
      if(numPoints>100) {
	TGraph *grTemp = FFTtools::getInterpolatedGraph(grClock[surf],1./2.6);
	grClockFiltered[surf]=FFTtools::simplePassBandFilter(grTemp,0,400);
	delete grTemp;
      }
      else {
	grClockFiltered[surf] = new TGraph(numPoints,times[surf],volts[surf]);
      }
   }

   // At this point we have filled the normalised voltage arrays and created TGraphs
   // we can now correlate  and extract the offsets
   Double_t deltaT=1./(2.6*fClockUpSampleFactor);
   correlateClocks(grClockFiltered,deltaT);


   for(int surf=0;surf<NUM_SURF;surf++) {
      Double_t clockCor=0;
      if(surf>0) {
	TGraph *grCor = grCorClock[surf-1];
	if(grCor) {
	  Int_t dtInt=FFTtools::getPeakBin(grCor);
	  Double_t peakVal,phiDiff;
	  grCor->GetPoint(dtInt,phiDiff,peakVal);
	  clockCor=phiDiff;
	  if(TMath::Abs(clockCor-clockCrossCorr[surf][fLabChip[surf][8]])>clockPeriod/2) {
	    //Need to try again
	    if(clockCor>clockCrossCorr[surf][fLabChip[surf][8]]) {
	      if(dtInt>2*fClockUpSampleFactor) {
		Int_t dt2ndInt=FFTtools::getPeakBin(grCor,0,dtInt-(2*fClockUpSampleFactor));
	       grCor->GetPoint(dt2ndInt,phiDiff,peakVal);
	       clockCor=phiDiff;		 
	      }
	      else {
		std::cerr << "What's going on here then??\n";
	      }
	    }
	    else {
	      if(dtInt<(grCor->GetN()-2*fClockUpSampleFactor)) {
		Int_t dt2ndInt=FFTtools::getPeakBin(grCor,dtInt+(2*fClockUpSampleFactor),grCor->GetN());
		grCor->GetPoint(dt2ndInt,phiDiff,peakVal);
	       clockCor=phiDiff;
	      }
	      else {
		std::cerr << "What's going on here then??\n";
	      }
	    }	     	       	   	    	   
	  }
	}
	else {
	  clockCor=0;
	  std::cout << "Clock is broken for SURF " << surf+1 << "\tevent " 
		    << eventPtr->eventNumber << "\n";
	}
	
	 //	 delete grCor;
      }
	 
      clockPhiArray[surf]=clockCor-fancyClockJitterOffset[surf][fLabChip[surf][8]];
      eventPtr->fClockPhiArray[surf]=clockPhiArray[surf];
      
      //Now can actually shift times
      // Normal channels are corrected by DeltaPhi - <DeltaPhi>
      // Clock channels are corrected by DeltaPhi (just so the clocks line up)
      //       std::cout << "clockCor:\t" << clockCor << "\t" << clockPhiArray[surf] << std::endl;
      Int_t numPoints=numPointsArray[surf][8];
      for(int chan=0;chan<NUM_CHAN;chan++) {
	 for(int samp=0;samp<numPoints;samp++) {
	    if(chan<8) {
	       timeArray[surf][chan][samp]=surfTimeArray[surf][samp]-clockPhiArray[surf];
	    }
	    else
	       timeArray[surf][chan][samp]=surfTimeArray[surf][samp]-clockCor;
	 }
      }
      
      
   }

   for(int surf=0;surf<NUM_SURF;surf++) {
     if(grClock[surf]) delete grClock[surf];
     if(grClockFiltered[surf]) delete grClockFiltered[surf];
   }
#else 
   printf("FFTTools currently disabled\n");
#endif
}


void AnitaEventCalibrator::applyClockPhiCorrection(UsefulAnitaEvent *eventPtr)
{ 
  for(int surf=0;surf<NUM_SURF;surf++) {
    Int_t numPoints=numPointsArray[surf][8];
    for(int chan=0;chan<NUM_CHAN;chan++) {
      for(int samp=0;samp<numPoints;samp++) {
	timeArray[surf][chan][samp]=surfTimeArray[surf][samp]-eventPtr->fClockPhiArray[surf];	
      }
    }   
  }
}


void AnitaEventCalibrator::zeroMean() {
   //Won't do it for the clock channel due to the assymmetry of the clock signal
   for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<8;chan++) {
	 Double_t mean=TMath::Mean(numPointsArray[surf][chan],mvArray[surf][chan]);
	 for(int i=0;i<NUM_SAMP;i++) {
	    mvArray[surf][chan][i]-=mean;
	 }
      }
   }

}

void AnitaEventCalibrator::processEventUnwrapFast(UsefulAnitaEvent *eventPtr)
{  

  for(Int_t surf=0;surf<NUM_SURF;surf++) {
      Int_t chanIndex=eventPtr->getChanIndex(surf,8);
      Int_t labChip=eventPtr->getLabChip(chanIndex);
      Int_t rco=eventPtr->getRCO(chanIndex);
      Int_t earliestSample=eventPtr->getEarliestSample(chanIndex);
      Int_t latestSample=eventPtr->getLatestSample(chanIndex);
      Double_t tempFactor=1;
      
      if(earliestSample==0)
	earliestSample++;
      
      if(latestSample==0)
	latestSample=259;

      for(Int_t chan=0;chan<NUM_CHAN;chan++) {
	Int_t chanIndex=eventPtr->getChanIndex(surf,chan);
	//Raw array
	for(Int_t samp=0;samp<NUM_SAMP;samp++) {
	  rawArray[surf][chan][samp]=eventPtr->data[chanIndex][samp];
	}
	
	//Now do the unwrapping
	Int_t index=0;
	Double_t time=0;
	if(latestSample<earliestSample) {
	  //	std::cout << "Two RCO's\t" << surf << "\t" << chan << "\n";
	  //We have two RCOs
	  Int_t nextExtra=256;
	  Double_t extraTime=0;	
	  if(earliestSample<256) {
	    //Lets do the first segment up	
	    for(Int_t samp=earliestSample;samp<256;samp++) {
	      int binRco=1-rco;
	      scaArray[surf][chan][index]=samp;
	      unwrappedArray[surf][chan][index]=rawArray[surf][chan][samp];
	      mvArray[surf][chan][index]=double(rawArray[surf][chan][samp])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
	      timeArray[surf][chan][index]=time;
	      if(samp==255) {
		extraTime=time+(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
		
	      }
	      else {
		time+=(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
	     
	      }
	      index++;
	    }
	    time+=epsilonFromAbby[surf][labChip][rco]*tempFactor*fEpsilonTempScale; ///<This is the time of the first capacitor.
	  }
	  else {
	    //Will just ignore the first couple of samples.
	    nextExtra=260;
	    extraTime=0;
	  }
	  
	  
	  if(latestSample>=1) {
	    //We are going to ignore sample zero for now
	    time+=(justBinByBin[surf][labChip][rco][0])*tempFactor;
	    //	  time+=0.5*(justBinByBin[surf][labChip][rco][0]+justBinByBin[surf][labChip][rco][1])*tempFactor;
	    for(Int_t samp=1;samp<=latestSample;samp++) {
	      int binRco=rco;
	      if(nextExtra<260 && samp==1) {
		if(extraTime<time-0.22) { ///This is Andres's 220ps minimum sample separation
		  //Then insert the next extra capacitor
		  binRco=1-rco;
		  scaArray[surf][chan][index]=nextExtra;
		  unwrappedArray[surf][chan][index]=rawArray[surf][chan][nextExtra];
		  mvArray[surf][chan][index]=double(rawArray[surf][chan][nextExtra])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
		  timeArray[surf][chan][index]=extraTime;
		  if(nextExtra<259) {
		    extraTime+=(justBinByBin[surf][labChip][binRco][nextExtra])*tempFactor;
		    //		extraTime+=0.5*(justBinByBin[surf][labChip][binRco][nextExtra]+justBinByBin[surf][labChip][binRco][nextExtra+1])*tempFactor;
		  }
		  nextExtra++;
		  index++;	 
		  samp--;
		  continue;
		}
	      }
	      scaArray[surf][chan][index]=samp;
	      unwrappedArray[surf][chan][index]=rawArray[surf][chan][samp];
	      mvArray[surf][chan][index]=double(rawArray[surf][chan][samp])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
	      timeArray[surf][chan][index]=time;
	      if(samp<259) {
		time+=(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
		//	      time+=0.5*(justBinByBin[surf][labChip][binRco][samp]+justBinByBin[surf][labChip][binRco][samp+1])*tempFactor;
	      }
	      index++;
	    }
	  }
	}
	else {
	  //	std::cout << "One RCO\t" << surf << "\t" << chan << "\n";
	  //Only one rco
	  time=0;
	  for(Int_t samp=earliestSample;samp<=latestSample;samp++) {
	    int binRco=rco;
	    scaArray[surf][chan][index]=samp;
	    unwrappedArray[surf][chan][index]=rawArray[surf][chan][samp];
	    mvArray[surf][chan][index]=double(rawArray[surf][chan][samp])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
	    timeArray[surf][chan][index]=time;
	    if(samp<259) {
	      time+=(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
	      //	    time+=0.5*(justBinByBin[surf][labChip][binRco][samp]+justBinByBin[surf][labChip][binRco][samp+1])*tempFactor;
	    }
	    index++;
	  }
	}
	numPointsArray[surf][chan]=index;
      }
  
      //And fill in surfTimeArray if we need it for anything
      for(int samp=0;samp<numPointsArray[surf][8];samp++) {
	surfTimeArray[surf][samp]=timeArray[surf][8][samp];
      }    
  }    
}



void AnitaEventCalibrator::processEventBS(UsefulAnitaEvent* eventPtr){
  processEventAG(eventPtr, 0, 0, 1);
  eventPtr->analyseClocksForTempGuessBen();
  //  std::cout << eventPtr->fRollingAverageTempFactor << std::endl;

  /* Then apply bin-by-bin timing correction*/
  for(int surfInd=0; surfInd<NUM_SURF; surfInd++){
    for(int chanInd=0; chanInd<NUM_CHAN; chanInd++){
      for(int samp=0; samp<numPointsArray[surfInd][chanInd]; samp++){
	timeArray[surfInd][chanInd][samp]*=eventPtr->fRollingAverageTempFactor;
      }
    }
  }
}


  /* Don't want to copy and paste this code multiple times...*/
void AnitaEventCalibrator::processEventAG(UsefulAnitaEvent *eventPtr, Int_t fGetTempFactorInProcessEventAG, Int_t fUseRollingTempAverage, Int_t fReadRcoFromFirmware){
  // std::cout << "processEventAG\n";

  //C3PO num is no longer the clock in channel 9
  //  if(eventPtr->fC3poNum) {
  //    clockPeriod=1e9/eventPtr->fC3poNum;
  //  }
  Double_t tempFactor=1;

  if(fGetTempFactorInProcessEventAG && !fUseRollingTempAverage){
    tempFactor=eventPtr->getTempCorrectionFactor();
  }
  else if(fGetTempFactorInProcessEventAG && fUseRollingTempAverage){
    tempFactor=eventPtr->fRollingAverageTempFactor;
  }

  for(Int_t surf=0;surf<NUM_SURF;surf++) {
    for(Int_t chan=0;chan<NUM_CHAN;chan++) {
      int numExtras = 0;

      Int_t chanIndex=eventPtr->getChanIndex(surf,chan);
      Int_t labChip=eventPtr->getLabChip(chanIndex);
      fLabChip[surf][chan]=labChip;
      
      Int_t rco = -1;
      if(fReadRcoFromFirmware){
	rco=eventPtr->getRcoCorrected(chanIndex);
      }
      else{
	rco=eventPtr->guessRco(chanIndex);
      }

      Int_t earliestSample=eventPtr->getEarliestSample(chanIndex);
      Int_t latestSample=eventPtr->getLatestSample(chanIndex);

      if(earliestSample==0)
	earliestSample++;

      if(latestSample==0)
	latestSample=259;
      
      //      if(surf==7) 
      //	std::cout << chan << "\t" << earliestSample << "\t" << latestSample 
      //		  << "\n";

      //Raw array
      for(Int_t samp=0;samp<NUM_SAMP;samp++) {
	rawArray[surf][chan][samp]=eventPtr->data[chanIndex][samp];
      }
      
      //Now do the unwrapping
      Int_t index=0;
      Double_t time=0;
      if(latestSample<earliestSample) {
	//	std::cout << "Two RCO's\t" << surf << "\t" << chan << "\n";
	//We have two RCOs
	Int_t nextExtra=256;
	Double_t extraTime=0;	
	if(earliestSample<256) {
	  //Lets do the first segment up	
	  for(Int_t samp=earliestSample;samp<256;samp++) {
	    int binRco=1-rco;
	    scaArray[surf][chan][index]=samp;
	    unwrappedArray[surf][chan][index]=rawArray[surf][chan][samp];
	    mvArray[surf][chan][index]=double(rawArray[surf][chan][samp])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
	    timeArray[surf][chan][index]=time;
	    if(samp==255) {
	      extraTime=time+(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
	      //	      extraTime=time+0.5*(justBinByBin[surf][labChip][binRco][samp]+justBinByBin[surf][labChip][binRco][samp+1])*tempFactor;
	    }
	    else {
	      time+=(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
	      //	      time+=0.5*(justBinByBin[surf][labChip][binRco][samp]+justBinByBin[surf][labChip][binRco][samp+1])*tempFactor;
	    }
	    index++;
	  }
	  time+=epsilonFromAbby[surf][labChip][rco]*tempFactor*fEpsilonTempScale; ///<This is the time of the first capacitor.
	}
	else {
	  //Will just ignore the first couple of samples.
	  nextExtra=260;
	  extraTime=0;
	}
	
	
	if(latestSample>=1) {
	  //We are going to ignore sample zero for now
	  time+=(justBinByBin[surf][labChip][rco][0])*tempFactor;
	  //	  time+=0.5*(justBinByBin[surf][labChip][rco][0]+justBinByBin[surf][labChip][rco][1])*tempFactor;
	  for(Int_t samp=1;samp<=latestSample;samp++) {
	    int binRco=rco;
	    if(nextExtra<260 && samp==1) {
	      if(extraTime<time-0.22) { ///This is Andres's 220ps minimum sample separation
		//Then insert the next extra capacitor
		binRco=1-rco;
		scaArray[surf][chan][index]=nextExtra;
		unwrappedArray[surf][chan][index]=rawArray[surf][chan][nextExtra];
		mvArray[surf][chan][index]=double(rawArray[surf][chan][nextExtra])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
		timeArray[surf][chan][index]=extraTime;
		if(nextExtra<259) {
		  extraTime+=(justBinByBin[surf][labChip][binRco][nextExtra])*tempFactor;
		  //		extraTime+=0.5*(justBinByBin[surf][labChip][binRco][nextExtra]+justBinByBin[surf][labChip][binRco][nextExtra+1])*tempFactor;
		}
		nextExtra++;
		numExtras++;
		index++;	 
		samp--;
		continue;
	      }
	    }
	    scaArray[surf][chan][index]=samp;
	    unwrappedArray[surf][chan][index]=rawArray[surf][chan][samp];
	    mvArray[surf][chan][index]=double(rawArray[surf][chan][samp])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
	    timeArray[surf][chan][index]=time;
	    if(samp<259) {
	      time+=(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
	      //	      time+=0.5*(justBinByBin[surf][labChip][binRco][samp]+justBinByBin[surf][labChip][binRco][samp+1])*tempFactor;
	    }
	    index++;
	  }
	}
      }
      else {
	//	std::cout << "One RCO\t" << surf << "\t" << chan << "\n";
	//Only one rco
	time=0;
	for(Int_t samp=earliestSample;samp<=latestSample;samp++) {
	  int binRco=rco;
	  scaArray[surf][chan][index]=samp;
	  unwrappedArray[surf][chan][index]=rawArray[surf][chan][samp];
	  mvArray[surf][chan][index]=double(rawArray[surf][chan][samp])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
	  timeArray[surf][chan][index]=time;
	  if(samp<259) {
	    time+=(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
	    //	    time+=0.5*(justBinByBin[surf][labChip][binRco][samp]+justBinByBin[surf][labChip][binRco][samp+1])*tempFactor;
	  }
	  index++;
	}
      }
      numPointsArray[surf][chan]=index;
      // std::cout << "index = " << index << ", numExtras = " << numExtras << std::endl;
    }
    //Okay now add Stephen's check to make sure that all the channels on the SURF have the same number of points.
    for(int chan=0;chan<8;chan++) {
	
       
      if(numPointsArray[surf][chan]<numPointsArray[surf][8]) {

       
	numPointsArray[surf][chan]=numPointsArray[surf][8];
	for(int samp=0;samp<numPointsArray[surf][8];samp++) {
	     
	  timeArray[surf][chan][samp]=timeArray[surf][8][samp];
	  //	     std::cout << "Fix: " << surf << "\t" << chan << "\t" << samp
	  //		       << "\t" << timeArray[surf][chan][samp] << "\n";
	}    
      }

      if(numPointsArray[surf][chan]>numPointsArray[surf][8]) {
	numPointsArray[surf][chan]=numPointsArray[surf][8];
	for(int samp=0;samp<numPointsArray[surf][8];samp++) {
	  //	     std::cout << "Fix: " << surf << "\t" << chan << "\t" << samp
	  //		       << "\t" << timeArray[surf][chan][samp] << "\n";
	  timeArray[surf][chan][samp]=timeArray[surf][8][samp];
	}    	
      }
              
    }
    
    //And fill in surfTimeArray if we need it for anything
    for(int samp=0;samp<numPointsArray[surf][8];samp++) {
      surfTimeArray[surf][samp]=timeArray[surf][8][samp];
    }    
  }	  		  	  
}


//   Double_t tempFactor = 1;
//   if(fGetTempFactorInProcessEventAG && !fUseRollingTempAverage){
//     tempFactor=eventPtr->getTempCorrectionFactor();
//   }
//   else if(fGetTempFactorInProcessEventAG && fUseRollingTempAverage){
//     tempFactor=eventPtr->fRollingAverageTempFactor;
//   }

//   for(Int_t surf=0;surf<NUM_SURF;surf++) {
//     for(Int_t chan=0;chan<NUM_CHAN;chan++) {
//       Int_t chanIndex=eventPtr->getChanIndex(surf,chan);
//       Int_t labChip=eventPtr->getLabChip(chanIndex);
//       fLabChip[surf][chan]=labChip;
//       //      Int_t rco=eventPtr->guessRco(chanIndex);
//       Int_t rco = -1;
//       if(fReadRcoFromFirmware){
// 	rco=eventPtr->getRcoCorrected(chanIndex);
//       }
//       else{
// 	rco=eventPtr->guessRco(chanIndex);
//       }

      

//       Int_t earliestSample=eventPtr->getEarliestSample(chanIndex);
//       Int_t latestSample=eventPtr->getLatestSample(chanIndex);


//       if(earliestSample==0)
// 	earliestSample++;
      
//       if(latestSample==0)
// 	latestSample=259;
//       //      std::cerr << "processEventAG: " <<  chanIndex << "\t" << labChip << "\t" << rco 
//       //      		<< "\t" << earliestSample << "\t" << latestSample << "\n";
//       //Raw array
//       for(Int_t samp=0;samp<NUM_SAMP;samp++) {
// 	rawArray[surf][chan][samp]=eventPtr->data[chanIndex][samp];
//       }
      
//       //Now do the unwrapping
//       Int_t index=0;
//       Double_t time=0;
//       if(latestSample<earliestSample) {
// 	//	std::cout << "Two RCO's\t" << surf << "\t" << chan << "\n";
// 	//We have two RCOs
// 	Int_t nextExtra=256;
// 	Double_t extraTime=0;	
// 	if(earliestSample<256) {
// 	  //Lets do the first segment up	
// 	  for(Int_t samp=earliestSample;samp<256;samp++) {
// 	    int binRco=1-rco;
// 	    scaArray[surf][chan][index]=samp;
// 	    unwrappedArray[surf][chan][index]=rawArray[surf][chan][samp];
// 	    mvArray[surf][chan][index]=double(rawArray[surf][chan][samp])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
// 	    timeArray[surf][chan][index]=time;
// 	    if(samp==255) {
// 	      extraTime=time+(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
	     
// 	    }
// 	    else {
// 	      time+=(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
	     
// 	    }
// 	    index++;
// 	  }
// 	  time+=epsilonFromAbby[surf][labChip][rco]*tempFactor*fEpsilonTempScale; ///<This is the time of the first capacitor.
// 	}
// 	else {
// 	  //Will just ignore the first couple of samples.
// 	  nextExtra=260;
// 	  extraTime=0;
// 	}
	
	
// 	if(latestSample>=1) {
// 	  //We are going to ignore sample zero for now
// 	  time+=(justBinByBin[surf][labChip][rco][0])*tempFactor;
// 	  //	  time+=0.5*(justBinByBin[surf][labChip][rco][0]+justBinByBin[surf][labChip][rco][1])*tempFactor;
// 	  for(Int_t samp=1;samp<=latestSample;samp++) {
// 	    int binRco=rco;
// 	    if(nextExtra<260 && samp==1) {
// 	      if(extraTime<time-0.22) { ///This is Andres's 220ps minimum sample separation
// 		//Then insert the next extra capacitor
// 	      binRco=1-rco;
// 	      scaArray[surf][chan][index]=nextExtra;
// 	      unwrappedArray[surf][chan][index]=rawArray[surf][chan][nextExtra];
// 	      mvArray[surf][chan][index]=double(rawArray[surf][chan][nextExtra])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
// 	      timeArray[surf][chan][index]=extraTime;
// 	      if(nextExtra<259) {
// 		extraTime+=(justBinByBin[surf][labChip][binRco][nextExtra])*tempFactor;
// 		//		extraTime+=0.5*(justBinByBin[surf][labChip][binRco][nextExtra]+justBinByBin[surf][labChip][binRco][nextExtra+1])*tempFactor;
// 	      }
// 	      nextExtra++;
// 	      index++;	 
//    	      samp--;
// 	      continue;
// 	      }
// 	    }
// 	    scaArray[surf][chan][index]=samp;
// 	    unwrappedArray[surf][chan][index]=rawArray[surf][chan][samp];
// 	    mvArray[surf][chan][index]=double(rawArray[surf][chan][samp])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
// 	    timeArray[surf][chan][index]=time;
// 	    if(samp<259) {
// 	      time+=(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
// 	      //	      time+=0.5*(justBinByBin[surf][labChip][binRco][samp]+justBinByBin[surf][labChip][binRco][samp+1])*tempFactor;
// 	    }
// 	    index++;
// 	  }
// 	}
//       }
//       else {
// 	//	std::cout << "One RCO\t" << surf << "\t" << chan << "\n";
// 	//Only one rco
// 	time=0;
// 	for(Int_t samp=earliestSample;samp<=latestSample;samp++) {
// 	  int binRco=rco;
// 	  scaArray[surf][chan][index]=samp;
// 	  unwrappedArray[surf][chan][index]=rawArray[surf][chan][samp];
// 	  mvArray[surf][chan][index]=double(rawArray[surf][chan][samp])*2*mvCalibVals[surf][chan][labChip]; //Need to add mv calibration at some point
// 	  timeArray[surf][chan][index]=time;
// 	  if(samp<259) {
// 	    time+=(justBinByBin[surf][labChip][binRco][samp])*tempFactor;
// 	    //	    time+=0.5*(justBinByBin[surf][labChip][binRco][samp]+justBinByBin[surf][labChip][binRco][samp+1])*tempFactor;
// 	  }
// 	  index++;
// 	}
//       }
//       numPointsArray[surf][chan]=index;
//     }
//     //Okay now add Stephen's check to make sure that all the channels on the SURF have the same number of points.
//     for(int chan=0;chan<8;chan++) {
	
       
//        if(numPointsArray[surf][chan]<numPointsArray[surf][8]) {

       
// 	  numPointsArray[surf][chan]=numPointsArray[surf][8];
// 	  for(int samp=0;samp<numPointsArray[surf][8];samp++) {
	     
// 	     timeArray[surf][chan][samp]=timeArray[surf][8][samp];
// 	     //	     std::cout << "Fix: " << surf << "\t" << chan << "\t" << samp
// 	     //		       << "\t" << timeArray[surf][chan][samp] << "\n";
// 	  }    
//        }

//        if(numPointsArray[surf][chan]>numPointsArray[surf][8]) {
// 	  numPointsArray[surf][chan]=numPointsArray[surf][8];
// 	  for(int samp=0;samp<numPointsArray[surf][8];samp++) {
// 	     //	     std::cout << "Fix: " << surf << "\t" << chan << "\t" << samp
// 	     //		       << "\t" << timeArray[surf][chan][samp] << "\n";
// 	     timeArray[surf][chan][samp]=timeArray[surf][8][samp];
// 	  }    	
//        }
              
//     }
    
//     //And fill in surfTimeArray if we need it for anything
//     for(int samp=0;samp<numPointsArray[surf][8];samp++) {
//       surfTimeArray[surf][samp]=timeArray[surf][8][samp];
//     }    
//   }	  		  	  
// }

Int_t AnitaEventCalibrator::justBinByBinTimebase(UsefulAnitaEvent *eventPtr)
{
//    Int_t refEvNum=31853801;
//    if(!fFakeTemp) {
//       fFakeTemp= new TF1("fFakeTemp","[0] + [1]*exp(-x*[2])",0,100000);
//       fFakeTemp->SetParameters(8.07,0.13,1./30000);
//    }
//    Double_t tempFactor=1;
//    if(doFakeTemp) {
//       tempFactor=fFakeTemp->Eval(100000)/fFakeTemp->Eval(eventPtr->eventNumber-refEvNum);
//    }
  Double_t tempFactor=eventPtr->getTempCorrectionFactor();
        

   for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {	 	 
	 int chanIndex=getChanIndex(surf,chan);
	 int labChip=eventPtr->getLabChip(chanIndex);
	 int rco=eventPtr->guessRco(chanIndex); ///< Is this the right thing to do??
	 
	 Int_t earliestSample=eventPtr->getEarliestSample(chanIndex);
	 Int_t latestSample=eventPtr->getLatestSample(chanIndex);
	 

	 double time=0;
	 for(int samp=0;samp<NUM_SAMP;samp++) {
	   int binRco=rco;
	   rawArray[surf][chan][samp]=eventPtr->data[chanIndex][samp];
	   timeArray[surf][chan][samp]=time;
	   if(latestSample<earliestSample) {
	     //We have two rcos
	     if(samp>=earliestSample)
	       binRco=1-rco;
	     else 
	       binRco=rco;
	   }
	   time+=justBinByBin[surf][labChip][binRco][samp]*tempFactor;
	 }
      }
   }  
  return 0;
}





void AnitaEventCalibrator::loadCalib() {
  char calibDir[FILENAME_MAX];
  char fileName[FILENAME_MAX];
  char *calibEnv=getenv("ANITA_CALIB_DIR");
  if(!calibEnv) {
     char *utilEnv=getenv("ANITA_UTIL_INSTALL_DIR");
     if(!utilEnv)
	sprintf(calibDir,"calib");
     else
	sprintf(calibDir,"%s/share/anitaCalib",utilEnv);
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }

  for(int surf=0;surf<NUM_SURF;surf++) {
     for(int chip=0;chip<NUM_CHIP;chip++) {
	fancyClockJitterOffset[surf][chip]=0;
	clockCrossCorr[surf][chip]=0;
     }
  }
   //Set up some default numbers
    for(int surf=0;surf<NUM_SURF;surf++) {
	for(int chan=0;chan<NUM_CHAN;chan++) {
	  simonsDeltaT[surf][chan]=0;
	    for(int chip=0;chip<NUM_CHIP;chip++) {
		mvCalibVals[surf][chan][chip]=1;
		chipByChipDeltats[surf][chan][chip]=0;
	    }
	}
    }
    
    for(int surf=0;surf<NUM_SURF;surf++) {
       for(int chip=0;chip<NUM_CHIP;chip++) {
	  rcoLatchCalib[surf][chip]=36;
	  for(int rco=0;rco<NUM_RCO;rco++) {
	     timeBaseCalib[surf][chip][rco]=2.6;
	     epsilonCalib[surf][chip][rco]=1.2;
	     epsilonFromAbby[surf][chip][rco]=1.25*(rco+1);
	     for(int samp=0;samp<NUM_SAMP;samp++)
	       justBinByBin[surf][chip][rco][samp]=1./2.6;
	  }
       }
    }
    
   
    int surf,chan,chip,rco,samp;
    int ant;
    char pol;
    double mean,rms,calib;
    int numEnts;
    int icalib;
    //    sprintf(fileName,"%s/rfcmCalibFile.txt",calibDir);
    sprintf(fileName,"%s/mattsFirstGainCalib.dat",calibDir);
    std::ifstream CalibFile(fileName);
    char firstLine[180];
    CalibFile.getline(firstLine,179);
    while(CalibFile >> surf >> chan >> chip >> ant >> pol >> mean >> rms >> calib) {      
       Int_t realAnt;
       AnitaPol::AnitaPol_t realPol;

       AnitaGeomTool::getAntPolFromSurfChan(surf-1,chan-1,realAnt,realPol);
       if(realPol==AnitaPol::kHorizontal) 
	  calib*=-1;

      //The bastards switched the antenna orientation
      Int_t orient=AnitaGeomTool::getAntOrientation(realAnt);
      if(orient==-1) 
	 calib*=-1;
      if(orient==-2 && realPol==AnitaPol::kHorizontal) //Even Orient have never scored -2 goals
	 calib*=-1;
      

      mvCalibVals[surf-1][chan-1][chip-1]=calib;
      //	cout << surf << " " << chan << " " << chip << " " << calib << std::endl;
    }
//    cout << surf << " " << chan << " " << chip << " " << calib << std::endl;
//    exit(0);

    sprintf(fileName,"%s/firstPassDeltaT.dat",calibDir);
    std::ifstream DeltaTCalibFile(fileName);
    DeltaTCalibFile.getline(firstLine,179);
    while(DeltaTCalibFile >> surf >> chan >> chip >> calib >> rms >> numEnts) {      
      chipByChipDeltats[surf][chan][chip]=calib;
      //      std::cout << surf << " " << chan << " " << chip << " " << calib << std::endl;
    }
    
    sprintf(fileName,"%s/timeBaseCalib.dat",calibDir);
    std::ifstream TimeCalibFile(fileName);
    TimeCalibFile.getline(firstLine,179);
    while(TimeCalibFile >> surf >> chip >> rco >> calib) {
	timeBaseCalib[surf][chip][rco]=calib;
	//	std::cout << surf << " " << chip << " " << rco << " " << timeBaseCalib[surf][chip][rco] << std::endl;
    }

    
    sprintf(fileName,"%s/justBinByBin.dat",calibDir);
    std::ifstream BinByBinCalibFile(fileName);
    BinByBinCalibFile.getline(firstLine,179);
    while(BinByBinCalibFile >> surf >> chip >> rco >> samp >> calib) {
	justBinByBin[surf][chip][rco][samp]=calib;
	//	std::cout << surf << " " << chip << " " << rco << " " << samp << " " << justBinByBin[surf][chip][rco][samp] << std::endl;
    }


    //sprintf(fileName,"%s/epsilonFromAbby.dat",calibDir);
    sprintf(fileName,"%s/epsilonFromBenS.dat",calibDir);
    std::ifstream EpsilonAbbyFile(fileName);
    EpsilonAbbyFile.getline(firstLine,179);
    while(EpsilonAbbyFile >> surf >> chip >> rco >> calib) {
      epsilonFromAbby[surf][chip][rco]=calib; 
	//	cout << surf << " " << chan << " " << chip << " " << calib << std::endl;
    }

    sprintf(fileName,"%s/rcoLatchCalibWidth.txt",calibDir);
    std::ifstream RcoCalibFile(fileName);
    RcoCalibFile.getline(firstLine,179);
    while(RcoCalibFile >> surf >> chip >> icalib) {
	rcoLatchCalib[surf][chip]=icalib;
//	cout << surf << " " << chip << " " << icalib << std::endl;
    }
    RcoCalibFile.close();
    
    sprintf(fileName,"%s/epsilonCalib.dat",calibDir);
    std::ifstream EpsilonCalibFile(fileName);
    EpsilonCalibFile.getline(firstLine,179);
    while(EpsilonCalibFile >> surf >> chip >> rco >> calib) {
	epsilonCalib[surf][chip][rco]=calib;
//	cout << surf << " " << chan << " " << chip << " " << calib << std::endl;
    }


    sprintf(fileName,"%s/groupDelayCalibFile.dat",calibDir);
    std::ifstream GroupDelayCalibFile(fileName);
    GroupDelayCalibFile.getline(firstLine,179);
    while(GroupDelayCalibFile >> surf >> chan >> calib) {
       groupDelayCalib[surf][chan]=calib;
       //       std::cout << surf <<  " " << chan << " " << calib << std::endl;
    }

    sprintf(fileName,"%s/rcoLatchDelay.dat",calibDir);
    std::ifstream rcoDelayFile(fileName);
    rcoDelayFile.getline(firstLine,179);
    int latchStart, latchEnd;
    while(rcoDelayFile >> surf >> chip >> latchStart >> latchEnd) {
      rcoLatchStart[surf][chip]=latchStart;
      rcoLatchEnd[surf][chip]=latchEnd;
      // std::cout << surf << " " << chip << " " << latchStart << " " << rcoLatchStart[surf][chip] << " " << latchEnd << " " << rcoLatchEnd[surf][chip] << std::endl;
    }

    sprintf(fileName,"%s/newSlowClockCalibNums.dat",calibDir);
    std::ifstream ClockCalibFile(fileName);
    ClockCalibFile.getline(firstLine,179);
    while(ClockCalibFile >> surf >> chip >> calib) {
      clockJitterOffset[surf][chip]=calib;
      //      clockJitterOffset[surf][chip]=0;  //RJN hack for test
      //      std::cout << "clockJitterOffset:\t" << surf <<  " " << chip << " " << calib << std::endl;
    }

    sprintf(fileName,"%s/fastClocksPeakPhi.dat",calibDir);
    std::ifstream FastClockCalibFile(fileName);
    FastClockCalibFile.getline(firstLine,179);
    while(FastClockCalibFile >> surf >> chip >> calib) {
      fastClockPeakOffset[surf][chip]=calib;
    }

    sprintf(fileName,"%s/newFancyClockCalibNums.dat",calibDir);
    std::ifstream FancyClockCalibFile(fileName);
    FancyClockCalibFile.getline(firstLine,179);
    while(FancyClockCalibFile >> surf >> chip >> calib) {
      //       fancyClockJitterOffset[surf][chip]=calib;
       fancyClockJitterOffset[surf][chip]=0; //RJN hack for test
       //       std::cout << "fancyClockJitterOffset:\t" << surf <<  " " << chip << " " << calib << std::endl;
    }

    sprintf(fileName,"%s/crossCorrClocksPeakPhi.dat",calibDir);
    std::ifstream CrossCorrClockCalibFile(fileName);
    CrossCorrClockCalibFile.getline(firstLine,179);
    while(CrossCorrClockCalibFile >> surf >> chip >> calib) {
      clockCrossCorr[surf][chip]=calib;
    }

    
    
    sprintf(fileName,"%s/rfPowPeds.dat",calibDir);
    std::ifstream GaryRfPowPeds(fileName);
    GaryRfPowPeds.getline(firstLine,179);
    Int_t iant,ipol;
    Double_t rfPed,rfTSys,rfT300,rfSlope;
    while(GaryRfPowPeds >> iant >> ipol >> surf >> chan >> rfT300 >> rfTSys >> rfPed >> rfSlope) {
      rfPowPed[surf][chan]=rfPed;
      garysRfPowT300K[surf][chan]=rfT300;
      garysRfPowTSys[surf][chan]=rfTSys;
      garysRfPowSlope[surf][chan]=rfSlope;
      //      std::cout << surf << "\t" << chan << "\t" << calib << "\n";
    }


    sprintf(fileName,"%s/simonsPositionAndTimingOffsets.dat",calibDir);
    std::ifstream SimonsOffsets(fileName);
    SimonsOffsets.getline(firstLine,179);
    Double_t dt,dr,dphi,dz;
    while(SimonsOffsets >> iant >> dt >> dr >> dphi >> dz) {
      Int_t surf=AnitaGeomTool::getSurfFromAnt(iant);
      Int_t chan=AnitaGeomTool::getChanFromAntPol(iant,AnitaPol::kVertical);
      simonsDeltaT[surf][chan]=dt;     
    }


    sprintf(fileName,"%s/simonsHPolPositionAndTimingOffsets.dat",calibDir);
    std::ifstream SimonsHPolOffsets(fileName);
    SimonsHPolOffsets.getline(firstLine,179);
    while(SimonsHPolOffsets >> iant >> dt >> dr >> dphi >> dz) {
      Int_t surf=AnitaGeomTool::getSurfFromAnt(iant);
      Int_t chan=AnitaGeomTool::getChanFromAntPol(iant,AnitaPol::kHorizontal);
      simonsDeltaT[surf][chan]=dt;     
    }



    sprintf(fileName,"%s/peds.dat",calibDir);
    std::ifstream PedestalFile(fileName,std::ios::in | std::ios::binary);
    PedestalFile.read((char*)&fPedStruct,sizeof(PedestalStruct_t));
    

}


double AnitaEventCalibrator::Get_Interpolation_X(double x1, double y1, double x2, double y2, double y){
  double x=(x2-x1)/(y2-y1)*(y-y1)+x1;
  return x;
}


void AnitaEventCalibrator::correlateClocks(TGraph *grClock[NUM_SURF], Double_t deltaT)
{
#ifdef USE_FFT_TOOLS
  for(int surf=1;surf<NUM_SURF;surf++) {
    if(grCorClock[surf-1])
      delete grCorClock[surf-1];
    grCorClock[surf-1]=0;
  }

//   TGraph *grDeriv[NUM_SURF]={0};
//   TGraph *grInt[NUM_SURF]={0};
//   Int_t maxLength=0;
//   Int_t lengths[10]={0};
//   for(int surf=0;surf<NUM_SURF;surf++) {
//     //    grDeriv[surf]=FFTtools::getDerviative(grClock[surf]);
//         grInt[surf]=FFTtools::getInterpolatedGraph(grClock[surf],deltaT);
//     //    grInt[surf]=FFTtools::getInterpolatedGraph(grDeriv[surf],deltaT);
//     lengths[surf]=grInt[surf]->GetN();
//     if(lengths[surf]>maxLength)
//       maxLength=lengths[surf];

//     //    delete grDeriv[surf];
//   }
  
//   Int_t paddedLength=int(TMath::Power(2,int(TMath::Log2(maxLength))+2));
//   FFTWComplex *theFFT[NUM_SURF]={0};
//   Double_t *oldY[NUM_SURF] = {0};
//   Double_t *corVals[NUM_SURF] = {0};

//   Double_t *outputX[NUM_SURF] = {0};
//   Double_t *outputY[NUM_SURF] = {0};

//   Double_t firstX,firstY;
//   Double_t secondX,secondY;
//   grInt[0]->GetPoint(0,firstX,firstY);
//   grInt[0]->GetPoint(1,secondX,secondY);
//   Double_t actualDt=secondX-firstX;

//   int tempLength=(paddedLength/2)+1;
//   int firstRealSamp=(paddedLength-lengths[0])/2;
//   for(int surf=0;surf<NUM_SURF;surf++) {
//     Double_t thisX,thisY;
//     grInt[surf]->GetPoint(0,thisX,thisY);
//     Double_t waveOffset=firstX-thisX;

//     oldY[surf]= new Double_t [paddedLength];

//     for(int i=0;i<paddedLength;i++) {
//       if(i<firstRealSamp || i>=firstRealSamp+lengths[surf])
// 	thisY=0;
//        else {
// 	  grInt[surf]->GetPoint(i-firstRealSamp,thisX,thisY);
//        }
//        oldY[surf][i]=thisY;
//     }
//     theFFT[surf]=FFTtools::doFFT(paddedLength,oldY[surf]);    

//     //Now do the correlation
//     if(surf>0) {
//       FFTWComplex *tempStep = new FFTWComplex [tempLength];
//       Int_t no2=paddedLength>>1;
//       for(int i=0;i<tempLength;i++) {
// 	double reFFT1=theFFT[surf][i].re;
// 	double imFFT1=theFFT[surf][i].im;
// 	double reFFT2=theFFT[0][i].re;
// 	double imFFT2=theFFT[0][i].im;
// 	//Real part of output 
// 	tempStep[i].re=(reFFT1*reFFT2+imFFT1*imFFT2)/double(no2);
// 	//Imaginary part of output 
// 	tempStep[i].im=(imFFT1*reFFT2-reFFT1*imFFT2)/double(no2);
//       }
//       corVals[surf] = FFTtools::doInvFFT(paddedLength,tempStep);
//       delete [] tempStep;
      
//       outputX[surf] = new Double_t [paddedLength];
//       outputY[surf] = new Double_t [paddedLength];
//       for(int i=0;i<paddedLength;i++) {
// 	if(i<paddedLength/2) {
// 	  //Positive	  
// 	  outputX[surf][i+(paddedLength/2)]=(i*actualDt)+waveOffset;
// 	  outputY[surf][i+(paddedLength/2)]=corVals[surf][i];
// 	}
// 	else {
// 	  //Negative
// 	  outputX[surf][i-(paddedLength/2)]=((i-paddedLength)*actualDt)+waveOffset;
// 	  outputY[surf][i-(paddedLength/2)]=corVals[surf][i];	
// 	}  
//       }
      
//       grCorClock[surf-1]= new TGraph(paddedLength,outputX[surf],outputY[surf]);
//       delete [] outputX[surf];
//       delete [] outputY[surf];
//       delete [] corVals[surf];
//     }
//   }
    
//   for(int surf=0;surf<NUM_SURF;surf++) {
//     delete [] oldY[surf];
//     delete [] theFFT[surf];
//     delete grInt[surf];
//   }
  

  //Use the FFTtools package to interpolate and correlate the clock channels

  //  TGraph *grDeriv[NUM_SURF];
  //  TGraph *grInt[NUM_SURF];
  //  for(int surf=0;surf<NUM_SURF;surf++) {
  //    grInt[surf] = FFTtools::getInterpolatedGraph(grClock[surf],deltaT);
  //    grDeriv[surf] = FFTtools::getDerviative(grInt[surf]);
  //    if(surf>0) {
  //      grCorClock[surf-1] = FFTtools::getCorrelationGraph(grDeriv[surf],grDeriv[0]);
  //    }
  //  }
  //  for(int surf=0;surf<NUM_SURF;surf++) {
  //    delete grDeriv[surf];
  //    delete grInt[surf];
  //  }
  if(grClock[0]->GetN()>100) {
    for(int surf=1;surf<NUM_SURF;surf++) {
      if(grClock[surf]->GetN()>100) {
	grCorClock[surf-1]= FFTtools::getInterpolatedCorrelationGraph(grClock[surf],grClock[0],deltaT);
      }
      else {
	std::cout << "Missing clock for SURF " << surf+1 << "\n";
      }
    }
  }
  else {
    std::cout << "Missing clock for SURF " << 1 << "\n";
  } 
#endif
}

Double_t AnitaEventCalibrator::convertRfPowToKelvin(int surf, int chan, int adc) 
{
  Double_t tMeas=convertRfPowToKelvinMeasured(surf,chan,adc);
  return tMeas-garysRfPowTSys[surf][chan];
}


Double_t AnitaEventCalibrator::convertRfPowToKelvinMeasured(int surf, int chan, int adc) 
{
  //  std::cout << surf << "\t" << chan << "\t" << rfPowPed[surf][chan] << "\n";
  Double_t ped=rfPowPed[surf][chan]; 
  Double_t a=garysRfPowSlope[surf][chan];
  Double_t refTemp=garysRfPowT300K[surf][chan];
  
  Double_t DA=adc-ped;
  Double_t kelvin=refTemp*TMath::Power(10,(a*DA/10.));
  return kelvin;  
}

void AnitaEventCalibrator::updateRollingAverageClockDeltaT(UsefulAnitaEvent* eventPtr, Double_t allSurfMeanUpDt, Int_t allSurfNumUpDt){

  /* Probably sensible to check this really is a rolling average */
  if(eventPtr->eventNumber!=fLastEventNumber+1 && fLastEventNumber!=0){
    std::cerr << "Careful! Processing non-sequential events may screw up rolling average temperature correction!";
    std::cerr << " Last event " << fLastEventNumber << ", this event " << eventPtr->eventNumber << std::endl;
    /* Let the user think about it but don't kill the program... */
  }
  
  fAllSurfAverageClockDeltaTs[fTempEventInd] = allSurfMeanUpDt;
  fAllSurfAverageClockNumDeltaTs[fTempEventInd] = allSurfNumUpDt;

  fTempEventInd++;
  if(fTempEventInd==NUM_EVENTS_TO_AVERAGE_TEMP_OVER){
    fTempEventInd=0;
  }
  if(fNumEventsAveraged<NUM_EVENTS_TO_AVERAGE_TEMP_OVER){
    fNumEventsAveraged++;
  }

  // std::cout << fTempEventInd << " " << fNumEventsAveraged << " " << fLastEventNumber << std::endl;
  /* Sum over events to get rolling average */
  allSurfMeanUpDt /= allSurfNumUpDt;

  Double_t allSurfNEventAverageUpDt = 0;
  Int_t allSurfNEventAverageNumUpDt = 0;

  for(int i=0; i<fNumEventsAveraged; i++){
    allSurfNEventAverageUpDt += fAllSurfAverageClockDeltaTs[i];
    allSurfNEventAverageNumUpDt += fAllSurfAverageClockNumDeltaTs[i];
  }
  allSurfNEventAverageUpDt/=allSurfNEventAverageNumUpDt;

  /* Give the UsefulAnitaEvent it's temperature calibration numbers */
  eventPtr->fTempFactorGuess = clockPeriod/allSurfMeanUpDt;
  eventPtr->fRollingAverageTempFactor = clockPeriod/allSurfNEventAverageUpDt;
  fLastEventNumber = eventPtr->eventNumber;
}
