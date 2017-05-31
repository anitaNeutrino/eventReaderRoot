#include "UsefulAnitaEvent.h"
#include "AnitaConventions.h"
#include "RawAnitaEvent.h"
#include "TimedAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>

void exampleLoopAllEvents(int run);


void exampleLoopAllEvents()
{
   cout << "Usage: exampleLoopAllEvents(run no.)\n";
   //  exampleLoopAllEvents(1030);
}
  

void exampleLoopAllEvents(int run) {

  char eventName[100];//FILENAME_MAX];
  char headerName[100];//FILENAME_MAX];
  char hkName[100];//FILENAME_MAX];
  sprintf(eventName,"$ANITA_ROOT_DATA/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"$ANITA_ROOT_DATA/run%d/timedHeadFile%d.root",run,run);
  sprintf(hkName,"$ANITA_ROOT_DATA/run%d/prettyHkFile%d.root",run,run);

  RawAnitaEvent *event = 0;
  TimedAnitaHeader *header =0;
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


  UInt_t numEntries=headTree->GetEntries();
  UInt_t numStars=50;
  UInt_t starEvery=numEntries/numStars;
  if(starEvery==0) starEvery++;
  UInt_t count=0;
  
  cout << "There are " << numEntries << " in run " << run << endl;
  cout << "eventChain->GetEntries()\t" << eventChain->GetEntries() << endl;
  cout << "prettyHkTree->GetEntries()\t" << prettyHkTree->GetEntries() << endl;
  //  eventChain->GetEntry(0);
  for(UInt_t entry=0;entry<numEntries;entry++) {
     if(count%starEvery==0) cerr << "*";
    //Stupidly most do this to be perfectly safe  
     eventChain->GetEntry(entry);
     //    headTree->GetEntry(entry);
     //    prettyHkTree->GetEntry(entry);
    
   
    //This step produces a calibrated UsefulAnitaEvent
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlusFastClockZero,hk);
    //    cout << realEvent.eventNumber << " " << header->eventNumber << endl;
    //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
    count++;
  }
  cerr << endl;
  cout << "Processed " << count << " events.\n";
}

