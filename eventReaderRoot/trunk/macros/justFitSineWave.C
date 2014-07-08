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
void fitSineWaves(char *baseName,int run,  int startEntry, int numEntries);
int getNiceColour(int index);

Double_t mySineWave(Double_t *t, Double_t *par) ;

void fitSineWaves()
{
  gStyle->SetStatH(0.25);
  gStyle->SetStatW(0.25);
}
  

void fitSineWaves(char *baseName, int run, int startEntry, int numEntries) {
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

  Double_t sineFreq=0.6;
  Int_t surfNum,chipNum,rcoNum;
  Int_t earliestSample=0;
  Int_t latestSample=0;
  Double_t t255,t0;
  Double_t chiSquared[2]={0};
  Int_t    ndf[2]={0};
  Double_t lag[2]={0};
  Double_t epsilon;

  Int_t chan=0;

   
  TF1 *fitSine = new TF1("fitSine",mySineWave,0,200,3);
  fitSine->SetNpx(1000);
  fitSine->SetParameters(55,sineFreq,0.8);
  fitSine->SetParLimits(0,20,80);
  fitSine->SetParLimits(1,sineFreq,sineFreq);
  fitSine->SetParLimits(2,0,1/sineFreq);



  Long64_t starEvery=maxEntry/40;
  if(starEvery==0) starEvery++;

  
  for(Long64_t entry=startEntry;entry<maxEntry;entry++) {

    if(entry%starEvery==0) 
      cerr << "*";

    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    //    prettyHkTree->GetEntry(entry);

    TGraph *grEv[10]={0};
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTLabAGFastClock,header);
    for(int surf=0;surf<10;surf++) {
      Int_t chanIndex=realEvent.getChanIndex(surf,chan);
      TGraph *gr = realEvent.getGraphFromSurfAndChan(surf,chan);
      Int_t labChip=realEvent.getLabChip(chanIndex);

      rcoNum=realEvent.guessRco(chanIndex);
    
      Double_t mean=gr->GetMean(2);
      Double_t *adcs=gr->GetY();
      Double_t *times=gr->GetX();
      for(int i=0;i<gr->GetN();i++) 
	adcs[i]-=mean;

      grEv[surf]= new TGraph(gr->GetN(),times,adcs);
      fitSine->SetParameters(55,sineFreq,0.8);
      grEv[surf]->Fit("fitSine","");
      delete gr;
      delete grEv[surf];
    }
  }
}


Double_t getZeroCrossingPoint(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
  Double_t m=(y1-y2)/(x1-x2);
  Double_t c=y1-m*x1;
  return (-1*c/m);
}

int getNiceColour(int index)
{
  if(index>10) return index;
  Int_t niceColours[11]={50,38,30,9,8,44,24,12,40,20,41};
  return niceColours[index];
}

Double_t mySineWave(Double_t *t, Double_t *par) 
{
  Double_t amp=par[0]; //In mV
  Double_t freq=par[1]; //In GHz
  Double_t lag=par[2]; //In ns

  return amp*TMath::Sin(TMath::TwoPi()*freq*(t[0]+lag));

}
