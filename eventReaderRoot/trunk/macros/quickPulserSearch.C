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


void quickPulserSearch(char *baseName,int run, int templateEntry, int startEntry, int numEntries);

void quickPulserSearch()
{
   gStyle->SetStatH(0.25);
   gStyle->SetStatW(0.25);
   quickPulserSearch("root://anita@192.168.2.103//data/anita/palestine08/root",3799,0,1000,19);
}
  

void quickPulserSearch(char *baseName, int run, int templateEntry,int startEntry, int numEntries) {
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

  //  TFile *fpHk = new TFile(hkName);
  //  TTree *prettyHkTree = (TTree*) fpHk->Get("prettyHkTree");
  //  prettyHkTree->SetBranchAddress("hk",&hk);


  //Friends only seem to work with TTree::Draw and similar commands
  //if you are manually calling GetEntry (i.e in a loop) you must call
  //the GetEntry for each tree separately.
  //  eventChain->AddFriend(headTree);
  //  eventChain->AddFriend(prettyHkTree);


  //  char textLabel[180];
  //  char pngName[180];
  //  TGraph *gr[9]={0};

  
  Long64_t nentries=eventChain->GetEntries();
  
  Long64_t maxEntry=startEntry+numEntries;
  if(nentries<maxEntry)
     maxEntry=nentries;

  char outName[180];
  sprintf(outName,"corTree%d.root",run);

  TFile *fpOut = new TFile(outName,"RECREATE");
  Double_t peakVal[4];
  Int_t peakBin[4];
  UInt_t triggerTime;
  UInt_t triggerTimeNs;
  UInt_t eventNumber=0;
  TTree *corTree = new TTree("corTree","Tree of correlations");
  corTree->Branch("peakVal",&peakVal,"peakVal[4]/D");
  corTree->Branch("peakBin",&peakBin,"peakBin[4]/I");
  corTree->Branch("eventNumber",&eventNumber,"eventNumber/i");
  corTree->Branch("triggerTime",&triggerTime,"triggerTime/i");
  corTree->Branch("triggerTimeNs",&triggerTimeNs,"triggerTimeNs/i");


  Double_t deltaT=1./(2.6*8);

  Long64_t starEvery=maxEntry/40;
  if(starEvery==0) starEvery++;

  Int_t antNums[4]={1,9,18,19};
  TGraph *grTemplate[4]={0};

  eventChain->GetEntry(templateEntry);
  UsefulAnitaEvent templateEvent(event,WaveCalType::kVoltageTime);
  for(int ant=0;ant<4;ant++) {
    grTemplate[ant]=templateEvent.getGraph(antNums[ant],AnitaPol::kVertical);
  }
  for(Long64_t entry=startEntry;entry<maxEntry;entry++) {
    
    if(entry%starEvery==0) 
      cerr << "*";

    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    //    prettyHkTree->GetEntry(entry);
   
    
    
    UsefulAnitaEvent realEvent(event,WaveCalType::kVoltageTime);
    //    if((entry-startEntry)%100==0)
    //      cout << realEvent.eventNumber << endl;
    
    
    
    for(int ant=0;ant<4;ant++) {
      TGraph *grEvent=realEvent.getGraph(antNums[ant],AnitaPol::kVertical);
    
      //    TGraph *grCor = FFTtools::getInterpolatedCorrelationGraph(grTemplate,grEvent,deltaT);
      TGraph *grCor = FFTtools::getCorrelationGraph(grTemplate[ant],grEvent);
      peakVal[ant]=FFTtools::getPeakVal(grCor);
      peakBin[ant]=FFTtools::getPeakBin(grCor);
      delete grEvent;
      delete grCor;
    }
    triggerTime=header->triggerTime;
    triggerTimeNs=header->triggerTimeNs;
    corTree->Fill();

  }
  corTree->AutoSave();

}

