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
  char fileName[180];
  sprintf(fileName,"epsilonTree%d.root",run);
  TFile *fpOut = new TFile(fileName,"RECREATE");
  Int_t surfNum,chipNum,rcoNum;
  Int_t earliestSample=0;
  Int_t latestSample=0;
  Double_t t255,t0;
  Double_t chiSquared[2]={0};
  Int_t    ndf[2]={0};
  Double_t lag[2]={0};
  Double_t epsilon;
  TTree *epsilonTree = new TTree("epsilonTree","Tree of Epsilon Stuff");
  epsilonTree->Branch("surf",&surfNum,"surf/I");
  epsilonTree->Branch("chip",&chipNum,"chip/I");
  epsilonTree->Branch("rco",&rcoNum,"rco/I");
  epsilonTree->Branch("t0",&t0,"t0/D");
  epsilonTree->Branch("t255",&t255,"t255/D");
  epsilonTree->Branch("epsilon",&epsilon,"epsilon/D");
  epsilonTree->Branch("chiSquared",chiSquared,"chiSquared[2]/D");
  epsilonTree->Branch("lag",lag,"lag[2]/D");
  epsilonTree->Branch("ndf",ndf,"ndf[2]/I");
  epsilonTree->Branch("earliestSample",&earliestSample,"earliestSample/I");
  epsilonTree->Branch("latestSample",&latestSample,"latestSample/I");

  Int_t chan=1;

   
  TF1 *fitSine = new TF1("fitSine",mySineWave,0,200,3);
  fitSine->SetNpx(1000);
  fitSine->SetParameters(55,sineFreq,0.8);
  fitSine->SetParLimits(0,45,65);
  fitSine->SetParLimits(1,sineFreq,sineFreq);
  fitSine->SetParLimits(2,0,1/sineFreq);

  TF1 *fitSine2 = new TF1("fitSine2",mySineWave,0,200,3);
  fitSine2->SetNpx(1000);
  fitSine2->SetParameters(55,sineFreq,0.8);
  fitSine2->SetParLimits(0,45,65);
  fitSine2->SetParLimits(1,sineFreq,sineFreq);
  fitSine2->SetParLimits(2,0,1/sineFreq);



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
    UsefulAnitaEvent realEvent(event,WaveCalType::kJustTimeNoUnwrap,header);
    for(int surf=0;surf<10;surf++) {
      Int_t chanIndex=realEvent.getChanIndex(surf,chan);
      TGraph *gr = realEvent.getGraphFromSurfAndChan(surf,chan);
      Int_t labChip=realEvent.getLabChip(chanIndex);
      earliestSample=realEvent.getEarliestSample(chanIndex);
      latestSample=realEvent.getLatestSample(chanIndex);
    
      surfNum=surf;
      chipNum=labChip;
      rcoNum=realEvent.guessRco(chanIndex);
    
      Double_t mean=gr->GetMean(2);
      Double_t *adcs=gr->GetY();
      Double_t *times=gr->GetX();
      for(int i=0;i<gr->GetN();i++) 
	adcs[i]-=mean;
      grEv[surf]= new TGraph(gr->GetN(),times,adcs);
      Double_t ampGuess=TMath::MaxElement(grEv[surf]->GetN(),adcs);

      //  canSine->cd(surf+1);
      //      new TCanvas();
      grEv[surf]->SetLineColor(getNiceColour(surf));
      grEv[surf]->SetMarkerColor(getNiceColour(surf));
      //      grEv[surf]->Draw("alp");


      if(latestSample<earliestSample) {
	//First part
	Int_t good=0;
	if(earliestSample<210) {
	  Double_t firstZc=0;
	  for(int i=earliestSample+1;i<255;i++) {
	    if(adcs[i]>=0 && adcs[i-1]<=0) {
	      //Then we have a rising edge
	      firstZc=getZeroCrossingPoint(times[i-1],adcs[i-1],times[i],adcs[i]);
	      break;	  
	    }
	  }
	  Double_t sinePeriod=1/sineFreq;
	  while(firstZc>sinePeriod)
	    firstZc-=sinePeriod;
	  
	  while(firstZc<0)
	    firstZc+=sinePeriod;
	  fitSine->SetParameters(ampGuess,sineFreq,firstZc);
	  fitSine->SetRange(times[earliestSample],times[255]);
	  grEv[surf]->Fit("fitSine","QR");
	  good++;
	}

	if(good==1 && latestSample>45) {
	  t255=(times[255]+fitSine->GetParameter(2)); //adjusted t255 time
	  Double_t period=1/sineFreq;
	  Int_t numPeriods=t255/(period);
	  Double_t sub255=(t255-numPeriods*(period));
	  t0=1.5+sub255;
	  if(t0>period) t0-=period;
	  
	  Double_t firstZc=0;
	  for(int i=2;i<latestSample;i++) {
	    if(adcs[i]>=0 && adcs[i-1]<=0) {
	      //Then we have a rising edge
	      firstZc=getZeroCrossingPoint(times[i-1],adcs[i-1],times[i],adcs[i]);
	      break;	  
	    }
	  }
	  Double_t sinePeriod=1/sineFreq;
	  while(firstZc>sinePeriod)
	    firstZc-=sinePeriod;
	  
	  while(firstZc<0)
	    firstZc+=sinePeriod;

	  fitSine2->SetParameters(ampGuess,sineFreq,firstZc);
	  fitSine2->SetRange(times[1],times[latestSample]);
	  grEv[surf]->Fit("fitSine2","QR+");
	  good++;
	} 	

	if(good==2) {
	  t255=(times[255]-fitSine->GetParameter(2)); //adjusted t255 time
	  t0=times[0]-fitSine2->GetParameter(2);
	  
	  chiSquared[0]=fitSine->GetChisquare();
	  chiSquared[1]=fitSine2->GetChisquare();
	  lag[0]=fitSine->GetParameter(2);
	  lag[1]=fitSine2->GetParameter(2);
	  ndf[0]=fitSine->GetNDF();
	  ndf[1]=fitSine2->GetNDF();

	  Double_t period=1/sineFreq;
	  Int_t numPeriods=t255/(period);
	  Double_t sub255=(t255-numPeriods*(period));
	  Double_t diff=t0-sub255;
	  if(diff<0.3) diff+=period;
	  epsilon=diff;

	  //	  cout << surf << "\t" << sub255 << "\t" << t0 << "\t" << diff << "\n";
	  epsilonTree->Fill();
	}

      }
      delete gr;
      delete grEv[surf];
    }
  }
  epsilonTree->AutoSave();
  
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

  return amp*TMath::Sin(TMath::TwoPi()*freq*(t[0]-lag));

}
