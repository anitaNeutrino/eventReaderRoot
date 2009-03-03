//////////////////////////////////////////////////////////////////////////////
/////  AnitaEventCalibrator.cxx        ANITA Event Calibrator            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for calibrating UsefulAnitaEvents               /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include "AnitaEventCalibrator.h"
#include "UsefulAnitaEvent.h"

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

















ClassImp(AnitaEventCalibrator);

AnitaEventCalibrator*  AnitaEventCalibrator::fgInstance = 0;


AnitaEventCalibrator::AnitaEventCalibrator()
   : TObject()
{
   fSquareWave=0;
   //Default constructor
   std::cout << "AnitaEventCalibrator::AnitaEventCalibrator()" << std::endl;
   loadCalib();
   //   std::cout << "AnitaEventCalibrator::AnitaEventCalibrator() end" << std::endl;

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
 
   //   std::cout << "AnitaEventCalibrator::calibrateUsefulEvent():" << calType << std::endl;
   if(calType==WaveCalType::kVTFullJW || calType==WaveCalType::kVTLabJW ||
      calType==WaveCalType::kVTFullJWPlus || calType==WaveCalType::kVTLabJWPlus ||
      calType==WaveCalType::kVTFullJWPlusClock || calType==WaveCalType::kVTLabJWPlusClock ||
      calType==WaveCalType::kVTLabJWPlusClockZero || calType==WaveCalType::kVTFullJWPlusClockZero) {
      //Do nothing
      if(eventPtr->gotCalibTemp)
	 processEventJW(eventPtr,eventPtr->calibTemp);
      else
	 processEventJW(eventPtr,33.046); //For now using fixed temp
       // until I get the alligned trees set up.
   }
   else {
      processEventRG(eventPtr);
   }


   //Clock Jitter correction
   if(calType==WaveCalType::kVTLabJWPlusClock || calType==WaveCalType::kVTFullJWPlusClock ||
      calType==WaveCalType::kVTLabClockRG || calType==WaveCalType::kVTFullClockRG ||
      calType==WaveCalType::kVTLabJWPlusClockZero || calType==WaveCalType::kVTFullJWPlusClockZero) {
      processClockJitter();
   }

   //Zero Mean
   if(calType==WaveCalType::kVTLabClockZeroRG || calType==WaveCalType::kVTFullClockZeroRG ||
      calType==WaveCalType::kVTLabJWPlusClockZero || calType==WaveCalType::kVTFullJWPlusClockZero) {

      zeroMean();
   }
    
   //   std::cout << "Done processEvent" << std::endl;
   return 0;
}



void AnitaEventCalibrator::processClockJitter() {
   if(!fSquareWave) {
      fSquareWave = new TF1("fSquareWave",funcSquareWave,5,90,7);
      fSquareWave->SetParameters(25,200,-200,12.881,16.428,0.33,0.33);
      fSquareWave->SetParLimits(0,0,35);
      fSquareWave->SetParLimits(1,50,350);
      fSquareWave->SetParLimits(2,-400,-50);
      //  fSquareWave->SetParLimits(3,8,16);
      //  fSquareWave->SetParLimits(4,10,20);
      fSquareWave->SetParLimits(3,12.881,12.881);
      fSquareWave->SetParLimits(4,16.428,16.428);
      fSquareWave->SetParLimits(5,0.33,0.33);
      fSquareWave->SetParLimits(6,0.33,0.33);
   }

   Float_t phi0=0;
   Double_t times[NUM_SAMP];
   Double_t volts[NUM_SAMP];

   for(int surf=0;surf<NUM_SURF;surf++) {
      //First fill temp arrays
      Int_t numPoints=numPointsArray[surf][8];
      for(int samp=0;samp<numPoints-1;samp++) {
	 times[samp]=surfTimeArray[surf][samp];
	 volts[samp]=mvArray[surf][8][samp];
      }

      //Next need to guess what phi is
      Float_t phiGuess=0;
      for(int i=0;i<numPoints-1;i++) {
	 if(volts[i]>=0 &&
	     volts[i+1]<0) {
	    phiGuess=times[i];
	    if(i>3)
	       break;
	 }
      }
           
      TGraph grTemp(numPoints,times,volts);
      fSquareWave->SetParameters(phiGuess,200,-200,12.881,16.428,0.33,0.33);
      grTemp.Fit(fSquareWave,"QR","goff");


      if(surf==0) 
	 phi0=fSquareWave->GetParameter(0);
      
      float period=fSquareWave->GetParameter(3)+fSquareWave->GetParameter(4)+
	 fSquareWave->GetParameter(5)+fSquareWave->GetParameter(6);
      
      float phi=fSquareWave->GetParameter(0);
      if((phi-phi0)>15)
	 phi-=period;
      if((phi-phi0)<-15)
	 phi+=period;
      
      Double_t clockCor=phi-phi0;
      clockPhiArray[surf]=(phi-phi0)-clockJitterOffset[surf][fLabChip[surf][8]];
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



void AnitaEventCalibrator::processEventRG(UsefulAnitaEvent *eventPtr) {
   //   std::cout << "processEventRG" << std::endl;
   //Now we'll actually try and process the data
   for(int surf=0;surf<NUM_SURF;surf++) {
      for(int chan=0;chan<NUM_CHAN;chan++) {	 
	    int goodPoints=0;
	    int chanIndex=getChanIndex(surf,chan);
	    int firstHitbus=eventPtr->firstHitbus[chanIndex];
	    int lastHitbus=eventPtr->lastHitbus[chanIndex];
	    int wrappedHitbus=((eventPtr->chipIdFlag[chanIndex])&0x8)>>3;
	       	    
	    int labChip=(eventPtr->chipIdFlag[chanIndex])&0x3;
	    fLabChip[surf][chan]=labChip;
	    int rcoBit=((eventPtr->chipIdFlag[chanIndex])&0x4)>>2;

	    
	    for(int samp=0;samp<NUM_SAMP;samp++) {
	       rawArray[surf][chan][samp]=eventPtr->data[chanIndex][samp];
	    }
	    
	    //Hack for sample zero weirdness
	    //This should go away at some point
	    if(chan==0) {
	       rawArray[surf][chan][0]=rawArray[surf][chan][259];
	    }
    
	    if(!wrappedHitbus) {
		int numHitBus=1+lastHitbus-firstHitbus;
		goodPoints=NUM_EFF_SAMP-numHitBus;
	    }
	    else {
		goodPoints=lastHitbus-(firstHitbus+1);
	    }
	    
	    int firstSamp,lastSamp;
	    if(!wrappedHitbus) {
		firstSamp=lastHitbus+1;
		lastSamp=NUM_SAMP+lastHitbus;
	    }
	    else {
		firstSamp=firstHitbus+1;
		lastSamp=lastHitbus;	    
	    }
	    numPointsArray[surf][chan]=goodPoints;


	    //Timebase calib
	    float timeVal=0;

	    //First we have to work out which phase we are in
	    int startRco=rcoBit;
	    if(!wrappedHitbus) 
		startRco=1-startRco;
	    if(firstSamp<rcoLatchCalib[surf][labChip] && !wrappedHitbus) 
	       startRco=1-startRco;


	    //Now we do the calibration
	    float time255=0;
	    for(int samp=firstSamp;samp<lastSamp;samp++) {
		int currentRco=startRco;
		int index=samp;
		int subtractOffset=0;
		if (index>=NUM_SAMP-1) {
		    index-=NUM_SAMP-1;	
		    subtractOffset=1;
		    currentRco=1-startRco;
		}
		
		unwrappedArray[surf][chan][samp-firstSamp]=rawArray[surf][chan][index];	       
		mvArray[surf][chan][samp-firstSamp]=double(rawArray[surf][chan][index])*mvCalibVals[surf][chan][labChip]*2;

		if(chan==8) {
		   if(index==0) {
		      //I think this works as we have switched rco's
		      if(time255>0) 
			 timeVal=time255+epsilonCalib[surf][labChip][currentRco];
		   }		
		   surfTimeArray[surf][samp-firstSamp]=timeVal;
		   //		if(surf==0 && chan==8)
		   //		   cout << samp-firstSamp << " " << timeVal << " " << timeBaseCalib[surf][labChip][currentRco] << " " << surf << " " << labChip << " " << currentRco << endl;
		   
		   timeVal+=1.0/timeBaseCalib[surf][labChip][currentRco];
		   if(index==255) {
		      time255=timeVal;
		   }

		   //		if(surf==0 && chan==8)
		   //		   cout << samp-firstSamp << " " << timeVal << endl;
		}
	    }
	    
      }      	           
   }
     
}


void AnitaEventCalibrator::processEventJW(UsefulAnitaEvent *eventPtr,float temp)
{  
   //   std::cout << "processEventJW" << std::endl;
   //  int word;
  int chanIndex=0;
  int labChip=0;    

  float temp_scale=29.938/(31.7225-0.054*temp);

  for (int surf=0; surf<NUM_SURF; surf++){
    for (int chan=0; chan<NUM_CHAN; chan++){ 
      int goodPoints=0;
      chanIndex=getChanIndex(surf,chan);
      int firstHitbus=eventPtr->firstHitbus[chanIndex];
      int lastHitbus=eventPtr->lastHitbus[chanIndex];
      int wrappedHitbus=((eventPtr->chipIdFlag[chanIndex])&0x8)>>3;
	

      labChip=(eventPtr->chipIdFlag[chanIndex])&0x3;
      fLabChip[surf][chan]=labChip;       	    
      int rcoBit=((eventPtr->chipIdFlag[chanIndex])&0x4)>>2;

      
      for(int samp=0;samp<NUM_SAMP;samp++) {
	 rawArray[surf][chan][samp]=eventPtr->data[chanIndex][samp];
      }
      
 

      if(!wrappedHitbus) {
	int numHitBus=1+lastHitbus-firstHitbus;
	goodPoints=NUM_SAMP-numHitBus;
      }
      else {
	goodPoints=lastHitbus-(firstHitbus+1);
      }


      int firstSamp,lastSamp;
      if(!wrappedHitbus) {
	firstSamp=lastHitbus+1;
	//	lastSamp=(NUM_SAMP-1)+lastHitbus;//Ryan's error?
	lastSamp=NUM_SAMP+firstHitbus;//My fix
      }
      else {
	firstSamp=firstHitbus+1;
	lastSamp=lastHitbus;
      }

      int startRco=rcoBit;
      if(!wrappedHitbus) 
	startRco=1-startRco;

      //switch RCO info for RCO delay
      if(firstHitbus<=tcalRcoDelayBin[surf][labChip][startRco] && !wrappedHitbus) startRco=1-startRco;

      int ibin=0;
      for(int samp=firstSamp;samp<lastSamp;samp++) {
	int index=samp;
	int irco=startRco;
	if (index>=NUM_SAMP) { 
	  index-=(NUM_SAMP);
	  irco=1-startRco;
	}

	if (index==0) { //temp. fix to skip sca=0 where unexpected voltage apears
	  goodPoints--;
	  continue;
	}
	
	unwrappedArray[surf][chan][ibin]=rawArray[surf][chan][index];
	mvArray[surf][chan][ibin]=double(rawArray[surf][chan][index])*mvCalibVals[surf][chan][labChip]*2;
	scaArray[surf][chan][ibin]=index; 
	rcobit[surf][chan][ibin]=irco;

	if (chan==8){//timing calibraion
	  float dt_bin=tcalTBin[surf][labChip][irco][index]*temp_scale;	  
	  int index_prev=index-1;
	  if (index_prev==-1) index_prev=259;
//	  float dt_bin_prev=tcalTBin[surf][labChip][irco][index_prev];

	  if (ibin==0) surfTimeArray[surf][ibin]=dt_bin;       
	  else surfTimeArray[surf][ibin]=surfTimeArray[surf][ibin-1]+dt_bin;	

	  if (index==1) {	  
	    float epsilon_eff=tcalEpsilon[surf][labChip][irco];
	    surfTimeArray[surf][ibin]=surfTimeArray[surf][ibin]-epsilon_eff;
	  }
	}
	ibin++;	
      }
      
      numPointsArray[surf][chan]=goodPoints;

      
      //2nd correction for RCO phase info. delay, RCO is determined by measuring clock period. 
      //to day CPU time, this method is used only if around the boundary of tcalRcoDelayBin.
      if (chan==8){
	if (firstHitbus>tcalRcoDelayBin[surf][labChip][startRco] && 
	    firstHitbus<=tcalRcoDelayBin[surf][labChip][startRco]+2 && !wrappedHitbus){
	
	float t_LE[3];
	float t_TE[3];
	int LE_count=0;
	int TE_count=0;
	int ibin=0;
	for (ibin=0;ibin<goodPoints-1;ibin++){
	  float mv1=unwrappedArray[surf][chan][ibin];
	  float mv2=unwrappedArray[surf][chan][ibin+1];
	  if (LE_count<3 && mv1<0 && mv2>=0){
	    float t1=surfTimeArray[surf][ibin];
	    float t2=surfTimeArray[surf][ibin+1];
	    t_LE[LE_count]=Get_Interpolation_X(t1, mv1, t2, mv2, 0);
	    LE_count++;
	  }	    
	  if (TE_count<3 && mv1>0 && mv2<=0){
	    float t1=surfTimeArray[surf][ibin];
	    float t2=surfTimeArray[surf][ibin+1];
	    t_TE[TE_count]=Get_Interpolation_X(t1, mv1, t2, mv2, 0);
	    TE_count++;
	  }	    
	}	  
	
	if (LE_count>2 && TE_count>2){	    

	  float clock_pulse_width_LE=0;
	  if (LE_count==2) clock_pulse_width_LE=t_LE[1]-t_LE[0];
	  if (LE_count==3) clock_pulse_width_LE=(t_LE[2]-t_LE[0])/2.;
	  float clock_pulse_width_TE=0;
	  if (TE_count==2) clock_pulse_width_TE=t_TE[1]-t_TE[0];
	  if (TE_count==3) clock_pulse_width_TE=(t_TE[2]-t_TE[0])/2.;
	  float clock_pulse_width=(clock_pulse_width_LE+clock_pulse_width_TE)/2.;

	  if (clock_pulse_width<29.75 || clock_pulse_width>30.2){
	      for (int ibin=0;ibin<goodPoints;ibin++){
		
		  for (int ichan=0; ichan<NUM_CHAN; ichan++) {
		      rcobit[surf][ichan][ibin]=1-rcobit[surf][ichan][ibin];
		  }
		  //may be one rcobit array per board might be enough, need to modify later./jwnam
		int irco=rcobit[surf][chan][ibin];
		int index=scaArray[surf][chan][ibin];
		float dt_bin=tcalTBin[surf][labChip][irco][index]*temp_scale;	  
		if (ibin==0) surfTimeArray[surf][ibin]=dt_bin;       
		else surfTimeArray[surf][ibin]=surfTimeArray[surf][ibin-1]+dt_bin;	
		if (index==1) {	  
		  float epsilon_eff=tcalEpsilon[surf][labChip][irco];
		  surfTimeArray[surf][ibin]=surfTimeArray[surf][ibin]-epsilon_eff;
		}
		
	      }
	    }
	  }
	}
      }
    }        
  }
}





void AnitaEventCalibrator::loadCalib() {
  char calibDir[FILENAME_MAX];
  char fileName[FILENAME_MAX];
  char *calibEnv=getenv("ANITA_CALIB_DIR");
  if(!calibEnv) {
    sprintf(calibDir,"calib");
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }


   //Set up some default numbers
    for(int surf=0;surf<NUM_SURF;surf++) {
	for(int chan=0;chan<NUM_CHAN;chan++) {
	    for(int chip=0;chip<NUM_CHIP;chip++) {
		mvCalibVals[surf][chan][chip]=1;
	    }
	}
    }
    
    for(int surf=0;surf<NUM_SURF;surf++) {
       for(int chip=0;chip<NUM_CHIP;chip++) {
	  rcoLatchCalib[surf][chip]=36;
	  for(int rco=0;rco<NUM_RCO;rco++) {
	     timeBaseCalib[surf][chip][rco]=2.6;
	     epsilonCalib[surf][chip][rco]=1.2;
	  }
       }
    }
    
   
    int surf,chan,chip,rco;
    int ant;
    char pol;
    float mean,rms,calib;
    int icalib;
    sprintf(fileName,"%s/rfcmCalibFile.txt",calibDir);
    std::ifstream CalibFile(fileName);
    char firstLine[180];
    CalibFile.getline(firstLine,179);
    while(CalibFile >> surf >> chan >> chip >> ant >> pol >> mean >> rms >> calib) {
	mvCalibVals[surf][chan][chip]=calib;
//	cout << surf << " " << chan << " " << chip << " " << calib << std::endl;
    }
//    cout << surf << " " << chan << " " << chip << " " << calib << std::endl;
//    exit(0);
    
    sprintf(fileName,"%s/timeBaseCalib.dat",calibDir);
    std::ifstream TimeCalibFile(fileName);
    TimeCalibFile.getline(firstLine,179);
    while(TimeCalibFile >> surf >> chip >> rco >> calib) {
	timeBaseCalib[surf][chip][rco]=calib;
	//	std::cout << surf << " " << chip << " " << rco << " " << timeBaseCalib[surf][chip][rco] << std::endl;
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


    sprintf(fileName,"%s/clockCalibNums.dat",calibDir);
    std::ifstream ClockCalibFile(fileName);
    ClockCalibFile.getline(firstLine,179);
    while(ClockCalibFile >> surf >> chip >> calib) {
       clockJitterOffset[surf][chip]=calib;
       //       std::cout << surf <<  " " << chip << " " << calib << std::endl;
    }


    //Load Jiwoo calibrations
    sprintf(fileName,"%s/jiwoo_timecode/anita_surf_time_constant_epsilon.txt",calibDir);
    std::ifstream JiwooEpsilonCalibFile(fileName);
    while(JiwooEpsilonCalibFile >> surf >> chip >> rco >> calib) {
	int tmpRco=1-rco;
	tcalEpsilon[surf][chip][tmpRco]=calib;
	//	cout << surf << " " << chip << " " << rco << "\t" << calib << endl;
	   
    }
    sprintf(fileName,"%s/jiwoo_timecode/anita_surf_time_constant_differeniial.txt",calibDir);
    std::ifstream JiwooDifferentialCalibFile(fileName);
    while(JiwooDifferentialCalibFile >> surf >> chip >> rco) {
	for(int samp=0;samp<NUM_SAMP;samp++) {
	    JiwooDifferentialCalibFile >> calib;
	    tcalTBin[surf][chip][rco][samp]=calib;
	}
    }
    sprintf(fileName,"%s/jiwoo_timecode/anita_surf_time_constant_rco_delay.txt",calibDir);
    std::ifstream JiwooRcoDelayCalibFile(fileName);
    while(JiwooRcoDelayCalibFile>> surf >> chip >> rco >> calib) {
	tcalRcoDelayBin[surf][chip][rco]=calib;
    }
    
}


float AnitaEventCalibrator::Get_Interpolation_X(float x1, float y1, float x2, float y2, float y){
  float x=(x2-x1)/(y2-y1)*(y-y1)+x1;
  return x;
}