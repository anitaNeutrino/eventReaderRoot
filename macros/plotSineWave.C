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
void plotSineWave(char *baseName,int run,  int startEntry);
int getNiceColour(int index);

Double_t mySineWave(Double_t *t, Double_t *par) ;

void plotSineWave()
{
   gStyle->SetStatH(0.25);
   gStyle->SetStatW(0.25);
}
  

void plotSineWave(char *baseName, int run, int startEntry) {
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
 


  Int_t chan=0;

    //Stupidly must do this to be perfectly safe  
  eventChain->GetEntry(startEntry);
  headTree->GetEntry(startEntry);
    //    prettyHkTree->GetEntry(entry);
   
  TF1 *fitSine = new TF1("fitSine",mySineWave,0,200,3);
  fitSine->SetNpx(1000);
  fitSine->SetParameters(50,0.6,0);
  fitSine->SetParLimits(0,40,60);
  fitSine->SetParLimits(1,0.6,0.6);
  fitSine->SetParLimits(2,0,1/0.6);
  TF1 *fitSine2 = new TF1("fitSine2",mySineWave,0,200,3);
  fitSine2->SetNpx(1000);
  fitSine2->SetParameters(50,0.6,0);
  fitSine2->SetParLimits(0,40,60);
  fitSine2->SetParLimits(1,0.6,0.6);
  fitSine2->SetParLimits(2,0,1/0.6);

  //  TCanvas *canSine = new TCanvas("canSine","canSine");
  //  canSine->Divide(2,5);

  TGraph *grEv[10]={0};
  UsefulAnitaEvent realEvent(event,WaveCalType::kJustTimeNoUnwrap,header);
  for(int surf=0;surf<10;surf++) {
      TGraph *gr = realEvent.getGraphFromSurfAndChan(surf,chan);
      Int_t labChip=realEvent.getLabChip(surf*9 + chan);
      Int_t earliestSample=realEvent.getEarliestSample(surf*9 + chan);
      Int_t latestSample=realEvent.getLatestSample(surf*9 + chan);

      Double_t mean=gr->GetMean(2);
      Double_t *adcs=gr->GetY();
      Double_t *times=gr->GetX();
      for(int i=0;i<gr->GetN();i++) 
	adcs[i]-=mean;
      grEv[surf]= new TGraph(gr->GetN(),times,adcs);

      //  canSine->cd(surf+1);
      new TCanvas();
      grEv[surf]->SetLineColor(getNiceColour(surf));
      grEv[surf]->SetMarkerColor(getNiceColour(surf));
      grEv[surf]->Draw("alp");


      if(latestSample<earliestSample) {
	//First part
	Int_t good=0;
	if(earliestSample<220) {
	  fitSine->SetRange(times[earliestSample],times[255]);
	  grEv[surf]->Fit("fitSine","QR");
	  good++;
	}

	if(latestSample>30) {
	  fitSine2->SetRange(times[0],times[latestSample]);
	  grEv[surf]->Fit("fitSine2","QR+");
	  good++;
	} 	

	if(good==2) {
	  Double_t t255=(times[255]+fitSine->GetParameter(2)); //adjusted t255 time
	  Double_t t0=times[0]+fitSine2->GetParameter(2);
	  
	  Double_t period=1/0.6;

	  Int_t numPeriods=t255/(period);
	  Double_t sub255=(t255-numPeriods*(period));
	  Double_t diff=t0-sub255;
	  if(diff<0) diff+=period;
	  cout << surf << "\t" << sub255 << "\t" << t0 << "\t" << diff << "\n";
	}

      }
      delete gr;
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
