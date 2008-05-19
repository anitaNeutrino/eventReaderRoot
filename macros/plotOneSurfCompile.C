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
#include "TROOT.h"
#include <iostream>
#include <fstream>

void plotOneSurf(int run, int startEntry, int numEntries,int surf);
void plotOneSurfByEventNumber(int run, int eventNumber, int surf);  

void plotOneSurf()
{
  plotOneSurf(1028,1000,1,1);
}

void plotOneSurfByEventNumber(int run, int eventNumber, int surf) {
   char headerName[FILENAME_MAX];
   sprintf(headerName,"/unix/anita1/newRootData/run%d/timedHeadFile%d.root",run,run);
   TFile *fpHead = new TFile(headerName);
   if(!fpHead) {
      cout << "No fpHead" << endl;
      return;
   }
   TTree *headTree = (TTree*) fpHead->Get("headTree");
   if(!headTree) {
      cout << "No headTree" << endl;
   }
   cout << "eventNumber: " << eventNumber << endl;
   Int_t startEntry=headTree->GetEntryNumberWithIndex(eventNumber,eventNumber);
   if(startEntry<0) {
      headTree->BuildIndex("eventNumber","eventNumber");
      startEntry=headTree->GetEntryNumberWithIndex(eventNumber,eventNumber);
      if(startEntry<0) {
	 cout << "startEntry: " << startEntry << endl;
	 return;
      }
   }
   cout << eventNumber << "\t" << startEntry << endl;
   fpHead->Close();
   plotOneSurf(run,startEntry,1,surf);
}

void plotOneSurf(int run, int startEntry, int numEntries,int surf) {

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


  //Friends only seem to work with TTree::Draw and similar commands
  //if you are manually calling GetEntry (i.e in a loop) you must call
  //the GetEntry for each tree separately.
  //  eventChain->AddFriend(headTree);
  //  eventChain->AddFriend(prettyHkTree);


  TCanvas *canSurf = (TCanvas*) gROOT->FindObject("canSurf");
  if(!canSurf) {
    canSurf = new TCanvas("canSurf","canSurf",800,600);
  }
  TGraph *gr[9]={0};

  for(int entry=startEntry;entry<startEntry+numEntries;entry++) {
    
    //Stupidly most do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    prettyHkTree->GetEntry(entry);
    
    
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlusClockZero,hk);
    //    UsefulAnitaEvent realEvent(event,WaveCalType::kJustUnwrap,hk);
    cout << realEvent.eventNumber << " " << header->eventNumber << endl;
    //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
    
    canSurf->Clear();
    //    canSurf->Clear();
    for(int chan=0;chan<9;chan++) {
      if(gr[chan]) delete gr[chan];
    }
    canSurf->Divide(2,5);

    for(int chan=0;chan<9;chan++) {
      canSurf->cd(chan+1);
      gr[chan]=realEvent.getGraph(realEvent.getChanIndex(surf,chan));
      gr[chan]->Draw("al");
    }
    canSurf->Update();
    gSystem->Sleep(1000);
  }
  //  canSurf->cd(10);
  //  sqaurey->Draw();
}

