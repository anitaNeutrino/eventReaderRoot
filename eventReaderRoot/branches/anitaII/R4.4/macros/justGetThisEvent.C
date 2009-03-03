#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
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

void justGetThisEvent(int run, int eventNumber);


void justGetThisEvent(int run, int eventNumber) {

  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  char hkName[FILENAME_MAX];
  sprintf(eventName,"/unix/anita1/newRootData/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"/unix/anita1/newRootData/run%d/timedHeadFile%d.root",run,run);
  sprintf(hkName,"/unix/anita1/newRootData/run%d/prettyHkFile%d.root",run,run);

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
  UInt_t count=0;
  
  headTree->BuildIndex("eventNumber");
  Long64_t entry=headTree->GetEntryNumberWithIndex(eventNumber);
  if(entry<0) {
    cout << "Event: " << eventNumber << " not in run: " << run << endl;
    return;
  }
  cout << "There are " << numEntries << " in run " << run << endl;
  cout << "eventChain->GetEntries()\t" << eventChain->GetEntries() << endl;
  cout << "prettyHkTree->GetEntries()\t" << prettyHkTree->GetEntries() << endl;
  //  eventChain->GetEntry(0);

  eventChain->GetEntry(entry);
  headTree->GetEntry(entry);
  prettyHkTree->GetEntry(entry);
  
    //This step produces a calibrated UsefulAnitaEvent
  UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlusFastClockZero,hk);
  //  UsefulAnitaEvent realEvent(event,WaveCalType::kNoCalib,hk);
  cout << realEvent.eventNumber << " " << header->eventNumber << endl;
  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
  
  for(int chan=0;chan<81;chan++) {
     cout << chan << "\t" << realEvent.fNumPoints[chan] << "\t" << (int)realEvent.firstHitbus[chan] << "\t" << (int)realEvent.lastHitbus[chan] << endl;
    //    for(int samp=0;samp<260;samp++) {
    //       cout << realEvent.data[chan][samp] << endl;
    //    }
  }
}

