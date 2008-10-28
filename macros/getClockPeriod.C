#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "FFTtools.h"
#include <iostream>
#include <fstream>

const float clockPeriod=29.970;

Double_t getZeroCrossingPoint(Double_t x1, Double_t y1, Double_t x2, Double_t y2);
void getClockPeriod(char *baseName,int run,  int startEntry, int numEntries);

void getClockPeriod()
{
   gStyle->SetStatH(0.25);
   gStyle->SetStatW(0.25);
}
  

void getClockPeriod(char *baseName, int run, int startEntry, int numEntries) {
  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  //  char hkName[FILENAME_MAX];
  sprintf(eventName,"%s/run%d/eventFile%d.root",baseName,run,run);
  sprintf(headerName,"%s/run%d/headFile%d.root",baseName,run,run);
  //  sprintf(hkName,"/unix/anita1/webData/firstDay/run%d/prettyHkFile%d.root",run,run);

  RawAnitaEvent *event = 0;
  RawAnitaHeader *header =0;
  //  PrettyAnitaHk *hk = 0;
  
  TChain *eventChain = new TChain("eventTree");
  eventChain->Add(eventName);
  eventChain->SetBranchAddress("event",&event);

   for(int extra=1;extra<100;extra++) {
     sprintf(eventName,"%s/run%d/eventFile%d_%d.root",baseName,run,run,extra);
    TFile *fpTest = TFile::Open(eventName);
    if(!fpTest) 
      break;
    else {
      delete fpTest;
      eventChain->Add(eventName);
    }
  }

  TFile *fpHead = TFile::Open(headerName);
  TTree *headTree = (TTree*) fpHead->Get("headTree");
  headTree->SetBranchAddress("header",&header);
  
  Long64_t nentries=eventChain->GetEntries();
  
  Long64_t maxEntry=startEntry+numEntries;
  if(nentries<maxEntry)
     maxEntry=nentries;

  char outName[180];
  sprintf(outName,"clockPeriodFile%d.root",run);

  TFile *fpOut = new TFile(outName,"RECREATE");
  TTree *clockTree = new TTree("clockTree","Tree of ZC stuff");
  Double_t deltaTUpPreHitbus;
  Double_t deltaTUpPostHitbus;
  Double_t deltaTDownPreHitbus;
  Double_t deltaTDownPostHitbus;
  Double_t deltaTAvgPreHitbus;
  Double_t deltaTAvgPostHitbus;
  Int_t sampPreHitbus,sampPostHitbus;
  Int_t earliestSample,latestSample;
  Int_t surfNum,chipNum,rcoNum;
  clockTree->Branch("sampPreHitbus",&sampPreHitbus,"sampPreHitbus/I");
  clockTree->Branch("sampPostHitbus",&sampPostHitbus,"sampPostHitbus/I");
  clockTree->Branch("deltaTUpPreHitbus",&deltaTUpPreHitbus,"deltaTUpPreHitbus/D");
  clockTree->Branch("deltaTUpPostHitbus",&deltaTUpPostHitbus,"deltaTUpPostHitbus/D");
  clockTree->Branch("deltaTDownPreHitbus",&deltaTDownPreHitbus,"deltaTDownPreHitbus/D");
  clockTree->Branch("deltaTDownPostHitbus",&deltaTDownPostHitbus,"deltaTDownPostHitbus/D");
  clockTree->Branch("deltaTAvgPreHitbus",&deltaTAvgPreHitbus,"deltaTAvgPreHitbus/D");
  clockTree->Branch("deltaTAvgPostHitbus",&deltaTAvgPostHitbus,"deltaTAvgPostHitbus/D");
  clockTree->Branch("earliestSample",&earliestSample,"earliestSample/I");
  clockTree->Branch("latestSample",&latestSample,"latestSample/I");
  clockTree->Branch("surf",&surfNum,"surf/I");
  clockTree->Branch("chip",&chipNum,"chip/I");
  clockTree->Branch("rco",&rcoNum,"rco/I");

  TTree *tempTree = new TTree("tempTree","Tree of clock temp stuff");
  Double_t avgDeltaT;
  Double_t rmsDeltaT;
  Int_t numDeltaT;
  tempTree->Branch("avgDeltaT",&avgDeltaT,"avgDeltaT/D");
  tempTree->Branch("rmsDeltaT",&rmsDeltaT,"rmsDeltaT/D");
  tempTree->Branch("numDeltaT",&numDeltaT,"numDeltaT/I");

  Long64_t starEvery=maxEntry/40;
  if(starEvery==0) starEvery++;

  Int_t chan=8;

  for(Long64_t entry=startEntry;entry<maxEntry;entry++) {
    
    if(entry%starEvery==0) 
      cerr << "*";

    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    //    prettyHkTree->GetEntry(entry);
   
    UsefulAnitaEvent realEvent(event,WaveCalType::kJustTimeNoUnwrap);
    numDeltaT=0;
    avgDeltaT=0;
    rmsDeltaT=0;
    Double_t avgDeltaTSq=0;
    for(int surf=0;surf<10;surf++) {
       TGraph *gr = realEvent.getGraphFromSurfAndChan(surf,chan);
       Int_t labChip=realEvent.getLabChip(surf*9 + chan);
       earliestSample=realEvent.getEarliestSample(surf*9 + chan);
       //       earliestSample+=1; //To avoid HITBUS crosstalk
       latestSample=realEvent.getLatestSample(surf*9 + chan);
       //       latestSample-=1; // To avoid HITBUS crosstalk
       Double_t offset=gr->GetMean(2);
       Double_t *adcs=gr->GetY();
       Double_t *times=gr->GetX();
       
       //       cout << times[0] << "\t" << times[1] << endl;
       surfNum=surf;
       chipNum=labChip;
       rcoNum=realEvent.guessRco(surf*9 + chan);


       if(earliestSample>259) 
	  earliestSample=0;
       if(latestSample<0) 
	  latestSample=259;

       sampPostHitbus=0;
       sampPreHitbus=0;
       deltaTAvgPostHitbus=0;
       deltaTAvgPreHitbus=0;

       //    cout << earliestSample << "\t" << latestSample << endl;
       if(latestSample<earliestSample) {
	  //We have two RCO's
	 {
	   Double_t posZcUp[100]={0};
	   Double_t posZcDown[100]={0};
	   Int_t numZcUp=0;
	   Int_t numZcDown=0;
	   for(int samp=earliestSample;samp<259;samp++) {
	     Double_t firstVal=adcs[samp]-offset;
	     Double_t secondVal=adcs[samp+1]-offset;
	     if((firstVal>=0 && secondVal<=0)) {  
	       //We have a ZC down
	       posZcDown[numZcDown]=getZeroCrossingPoint(times[samp],firstVal,times[samp+1],secondVal);
	       numZcDown++;
	     }
	     if((firstVal<=0 && secondVal>=0)) {  
	       //We have a ZC up
	       posZcUp[numZcUp]=getZeroCrossingPoint(times[samp],firstVal,times[samp+1],secondVal);
	       numZcUp++;
	     }
	   }
	   
	   sampPostHitbus=260-earliestSample;
	   deltaTDownPostHitbus=0;
	   deltaTAvgPostHitbus=0;
	   Int_t countDowns=0;
	   Int_t countAll=0;
	   if(numZcDown>1) {
	     for(int i=1;i<numZcDown;i++) {
	       if( (posZcDown[i]-posZcDown[i-1])>7.5 && (posZcDown[i]-posZcDown[i-1])<8.5) {
		 deltaTDownPostHitbus+=(posZcDown[i]-posZcDown[i-1]);
		 countDowns++;
		 avgDeltaT+=(posZcDown[i]-posZcDown[i-1]);
		 avgDeltaTSq+=(posZcDown[i]-posZcDown[i-1])*(posZcDown[i]-posZcDown[i-1]);
		 numDeltaT++;
	       }
	     }
	     if(countDowns>0) { 
	       deltaTAvgPostHitbus=deltaTDownPostHitbus;
	       countAll=countDowns;
	       deltaTDownPostHitbus/=countDowns;
	     }
	   }
	   deltaTUpPostHitbus=0;
	   Int_t countUps=0;
	   if(numZcUp>1) {
	     for(int i=1;i<numZcUp;i++) {
	       if( (posZcUp[i]-posZcUp[i-1])>7.5 && (posZcUp[i]-posZcUp[i-1])<8.5) {
		 deltaTUpPostHitbus+=(posZcUp[i]-posZcUp[i-1]);
		 countUps++;
		 avgDeltaT+=(posZcUp[i]-posZcUp[i-1]);
		 avgDeltaTSq+=(posZcUp[i]-posZcUp[i-1])*(posZcUp[i]-posZcUp[i-1]);
		 numDeltaT++;
	       }
	     }
	     if(countUps>0) {
	       deltaTAvgPostHitbus+=deltaTUpPostHitbus;
	       countAll+=countUps;
	       deltaTUpPostHitbus/=countUps;
	     }
	   }
	   if(countAll>0)
	     deltaTAvgPostHitbus/=countAll;
	 }

	 {
	   Double_t posZcUp[100]={0};
	   Double_t posZcDown[100]={0};
	   Int_t numZcUp=0;
	   Int_t numZcDown=0;
	   if(latestSample>0) {
	     for(int samp=0;samp<latestSample;samp++) {

	       Double_t firstVal=adcs[samp]-offset;
	       Double_t secondVal=adcs[samp+1]-offset;
	       if((firstVal>=0 && secondVal<=0)) {  
		 //We have a ZC down
		 posZcDown[numZcDown]=getZeroCrossingPoint(times[samp],firstVal,times[samp+1],secondVal);
		 numZcDown++;
	       }
	       if((firstVal<=0 && secondVal>=0)) {  
		 //We have a ZC up
		 posZcUp[numZcUp]=getZeroCrossingPoint(times[samp],firstVal,times[samp+1],secondVal);
		 numZcUp++;
	       }
	     }
	     sampPreHitbus=latestSample+1;
	     deltaTDownPreHitbus=0;
	     deltaTAvgPreHitbus=0;
	     Int_t countDowns=0;
	     Int_t countAll=0;
	     if(numZcDown>1) {
	       for(int i=1;i<numZcDown;i++) {
		 if( (posZcDown[i]-posZcDown[i-1])>7.5 && (posZcDown[i]-posZcDown[i-1])<8.5) {
		   deltaTDownPreHitbus+=(posZcDown[i]-posZcDown[i-1]);
		   countDowns++;
		   avgDeltaT+=(posZcDown[i]-posZcDown[i-1]);
		   avgDeltaTSq+=(posZcDown[i]-posZcDown[i-1])*(posZcDown[i]-posZcDown[i-1]);
		   numDeltaT++;
		 }
	       }
	       if(countDowns>0) {
		 deltaTAvgPreHitbus=deltaTDownPreHitbus;
		 countAll=countDowns;
		 deltaTDownPreHitbus/=countDowns;
	       }
	     }
	     deltaTUpPreHitbus=0;
	     Int_t countUps=0;
	     if(numZcUp>1) {
	       for(int i=1;i<numZcUp;i++) {
		 if( (posZcUp[i]-posZcUp[i-1])>7.5 && (posZcUp[i]-posZcUp[i-1])<8.5) {
		   deltaTUpPreHitbus+=(posZcUp[i]-posZcUp[i-1]);
		   countUps++;
		   avgDeltaT+=(posZcUp[i]-posZcUp[i-1]);
		   avgDeltaTSq+=(posZcUp[i]-posZcUp[i-1])*(posZcUp[i]-posZcUp[i-1]);
		   numDeltaT++;
		 }
	       }
	       if(countUps>0) {
		 deltaTAvgPreHitbus+=deltaTUpPreHitbus;
		 countAll+=countUps;
		 deltaTUpPreHitbus/=countUps;
	       }
	     }
	     if(countAll>0)	       
	       deltaTAvgPreHitbus/=countAll;
	   }
	 }	  	  
       }
       else {
	  //Only one RCO
	   Double_t posZcUp[100]={0};
	   Double_t posZcDown[100]={0};
	   Int_t numZcUp=0;
	   Int_t numZcDown=0;
	   for(int samp=earliestSample;samp<latestSample;samp++) {
	     Double_t firstVal=adcs[samp]-offset;
	     Double_t secondVal=adcs[samp+1]-offset;
	     if((firstVal>=0 && secondVal<=0)) {  
	       //We have a ZC down
	       posZcDown[numZcDown]=getZeroCrossingPoint(times[samp],firstVal,times[samp+1],secondVal);
	       numZcDown++;
	     }
	     if((firstVal<=0 && secondVal>=0)) {  
	       //We have a ZC up
	       posZcUp[numZcUp]=getZeroCrossingPoint(times[samp],firstVal,times[samp+1],secondVal);
	       numZcUp++;
	     }
	   }
	   sampPostHitbus=(latestSample+1)-earliestSample;
	   sampPreHitbus=0;
	   deltaTDownPreHitbus=0;
	   deltaTDownPostHitbus=0;
	   deltaTAvgPreHitbus=0;
	   deltaTAvgPostHitbus=0;
	   Int_t countDowns=0;
	   Int_t countAll=0;
	   if(numZcDown>1) {
	     for(int i=1;i<numZcDown;i++) {
	       if( (posZcDown[i]-posZcDown[i-1])>7.5 && (posZcDown[i]-posZcDown[i-1])<8.5) {
		 deltaTDownPostHitbus+=(posZcDown[i]-posZcDown[i-1]);
		 countDowns++;
		 avgDeltaT+=(posZcDown[i]-posZcDown[i-1]);
		 avgDeltaTSq+=(posZcDown[i]-posZcDown[i-1])*(posZcDown[i]-posZcDown[i-1]);
		 numDeltaT++;
	       }
	     }
	     if(countDowns>0) {
	       deltaTAvgPostHitbus=deltaTDownPostHitbus;
	       countAll=countDowns;
	       deltaTDownPostHitbus/=countDowns;
	     }
	   }
	   deltaTUpPreHitbus=0;
	   deltaTUpPostHitbus=0;
	   Int_t countUps=0;
	   if(numZcUp>1) {
	     for(int i=1;i<numZcUp;i++) {
	       if( (posZcUp[i]-posZcUp[i-1])>7.5 && (posZcUp[i]-posZcUp[i-1])<8.5) {
		 deltaTUpPostHitbus+=(posZcUp[i]-posZcUp[i-1]);
		 countUps++;
		 avgDeltaT+=(posZcUp[i]-posZcUp[i-1]);
		 avgDeltaTSq+=(posZcUp[i]-posZcUp[i-1])*(posZcUp[i]-posZcUp[i-1]);
		 numDeltaT++;
	       }
	     }
	     if(countUps>0) {
	       deltaTAvgPostHitbus+=deltaTUpPostHitbus;
	       countAll+=countUps;
	       deltaTUpPostHitbus/=countUps;
	     }
	   }
	   if(countAll>0)
	     deltaTAvgPostHitbus/=countAll;
       }
       clockTree->Fill();
       delete gr;
    }
    avgDeltaT/=numDeltaT;
    avgDeltaTSq/=numDeltaT;
    rmsDeltaT=avgDeltaTSq-avgDeltaT*avgDeltaT;
    tempTree->Fill();

    
    //    if((entry-startEntry)%100==0)
    //      cout << realEvent.eventNumber << endl;    
  }
  clockTree->AutoSave();
  tempTree->AutoSave();
  fpOut->Write();

}


Double_t getZeroCrossingPoint(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
  Double_t m=(y1-y2)/(x1-x2);
  Double_t c=y1-m*x1;
  return (-1*c/m);
}
