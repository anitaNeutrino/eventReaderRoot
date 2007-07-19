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
#include <iostream>
#include <fstream>

const float clockPeriod=29.970;

void dontFitSquareWave(int run, int startEntry, int numEntries);
float interpolateX(float x1, float y1, float x2, float y2, float y);

void dontFitSquareWave()
{
   gStyle->SetStatH(0.25);
   gStyle->SetStatW(0.25);
   dontFitSquareWave(1028,0,100);
}
  

void dontFitSquareWave(int run, int startEntry, int numEntries) {


  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  char hkName[FILENAME_MAX];
  sprintf(eventName,"/unix/anita1/webData/firstDay/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"/unix/anita1/webData/firstDay/run%d/headFile%d.root",run,run);
  sprintf(hkName,"/unix/anita1/webData/firstDay/run%d/prettyHkFile%d.root",run,run);

  RawAnitaEvent *event = 0;
  RawAnitaHeader *header =0;
  PrettyAnitaHk *hk = 0;
  
  TChain *eventChain = new TChain("eventTree");
  eventChain->Add(eventName);
  eventChain->SetBranchAddress("event",&event);

  TFile *fpHead = new TFile(headerName);
  TTree *headTree = (TTree*) fpHead->Get("headTree");
  headTree->SetBranchAddress("header",&header);

  TFile *fpHk = new TFile(hkName);
  TTree *prettyHkTree = (TTree*) fpHk->Get("prettyHkTree");
  prettyHkTree->SetBranchAddress("hk",&hk);


  //Friends only seem to work with TTree::Draw and similar commands
  //if you are manually calling GetEntry (i.e in a loop) you must call
  //the GetEntry for each tree separately.
  //  eventChain->AddFriend(headTree);
  //  eventChain->AddFriend(prettyHkTree);


  //  char textLabel[180];
  //  char pngName[180];
  TGraph *gr[9]={0};



  TFile *fp = new TFile("clockPhaseCalibNotFitted.root","RECREATE");
  
  TH1F *histPhiDiff[9];
  TH1F *histPhiDiffChip[9][4];

  char histName[180];
  for(int surf=0;surf<9;surf++) {

     sprintf(histName,"histPhiDiff%d",surf);
     histPhiDiff[surf] = new TH1F(histName,histName,200,-10,10);
     for(int chip=0;chip<4;chip++) {
	sprintf(histName,"histPhiDiffChip%d_%d",surf,chip);
	histPhiDiffChip[surf][chip] = new TH1F(histName,histName,200,-10,10);
     }

  }
  


  Long64_t nentries=eventChain->GetEntries();  
  Long64_t maxEntry=startEntry+numEntries;
  if(nentries<maxEntry)
     maxEntry=nentries;

  TH1F histHigh("histHigh","histHigh",1000,0,1000);
  TH1F histLow("histLow","histLow",1000,-1000,0);
  for(Long64_t entry=startEntry;entry<maxEntry;entry++) {
    
    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    prettyHkTree->GetEntry(entry);
       
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlus,hk);
    if((entry-startEntry)%100==0)
       cout << realEvent.eventNumber << endl;
    
    Float_t phi0=0;
    for(int surf=0;surf<9;surf++) {
       Int_t ci=UsefulAnitaEvent::getChanIndex(surf,8);

       histLow.Reset();
       histHigh.Reset();
       for(int i=0;i<realEvent.fNumPoints[ci];i++) {
	  if(realEvent.fVolts[ci][i]>0) {
	   histHigh.Fill(realEvent.fVolts[ci][i]);
	  }
	  else {
	     histLow.Fill(realEvent.fVolts[ci][i]);
	  }
       }
       //cout << maxVal << "\t" << minVal << endl;
       //       cout << histHigh.GetMean() << "\t" << histLow.GetMean() << endl;
       Double_t offset=(histHigh.GetMean()+histLow.GetMean())/2;
       Double_t maxVal=histHigh.GetMean()-offset;
       Double_t minVal=histLow.GetMean()-offset;
       //       cout << maxVal << "\t" << minVal << endl;
       
       Int_t numPoints=realEvent.fNumPoints[ci];
       Double_t times[260];
       Double_t volts[260];
       Int_t gotPhiGuess=0;
       Float_t phiGuess=0;
       for(int i=0;i<realEvent.fNumPoints[ci];i++) {
	  times[i]=realEvent.fTimes[ci][i];
	 Double_t tempV=realEvent.fVolts[ci][i]-offset;	
	 if(tempV>maxVal*0.9)
	   volts[i]=1;
	 else if(tempV<minVal*0.9)
	   volts[i]=-1;
	 else {
	   volts[i]=tempV/maxVal;
	 }
       }
       
       for(int i=0;i<realEvent.fNumPoints[ci]-1;i++) {
	 if(volts[i]>=0 &&
	    volts[i+1]<0) {
	   phiGuess=interpolateX(times[i],volts[i],times[i+1],volts[i+1],0);
	   if(i>3)
	     break;
	 }
       }
       //       cout << phiGuess << endl; 
       {

	  if(surf==0) 
	     phi0=phiGuess;

	  float phi=phiGuess;
	  if((phi-phi0)>15)
	     phi-=clockPeriod;
	  if((phi-phi0)<-15)
	     phi+=clockPeriod;
	  
	  
	  //       if(surf==4)
	  //	  cout << phi-phi0 << "\t" << phi << "\t" << phi0 << endl;
	  histPhiDiff[surf]->Fill(phi-phi0);

	  Int_t labChip=realEvent.getLabChip(ci);
	  histPhiDiffChip[surf][labChip]->Fill(phi-phi0);


       }
       delete gr[surf];
    }
    
  }


  ofstream CalibNums("clockCalibNumsNotFitted.dat");
  CalibNums << "#SURF" << "\t" << "CHIP" << "\t" << "Calib" << endl;
  TCanvas * canPhiChip = new TCanvas("canPhiChip","canPhiChip");
  canPhiChip->Divide(4,9);
  for(int surf=0;surf<9;surf++) {
     for(int chip=0;chip<4;chip++) {
	int index=(4*surf)+chip+1;
	canPhiChip->cd(index);
	histPhiDiffChip[surf][chip]->Draw();
	float calib=0;
	if(surf>0) {
	   histPhiDiffChip[surf][chip]->Fit("gaus");
	   calib=histPhiDiffChip[surf][chip]->GetFunction("gaus")->GetParameter(1);
	}
	histPhiDiffChip[surf][chip]->Write();
	CalibNums << surf << "\t" << chip << "\t" << calib << endl;
     }
  }
  //  fp->Close();

}

Double_t funcSquareWave(Double_t *x, Double_t *par)
{
   Double_t phi=par[0];
   Double_t a=par[1];
   Double_t b=par[2];
   Double_t dtLow=par[3];
   Double_t dtHigh=par[4];
   Double_t sllh=par[5];
   Double_t slhl=par[6];


   Double_t clockPeriod=dtLow+dtHigh+sllh+slhl;

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

float interpolateX(float x1, float y1, float x2, float y2, float y){
  float x=(x2-x1)/(y2-y1)*(y-y1)+x1;
  return x;
}
