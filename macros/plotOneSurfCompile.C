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
#include "TSystem.h"
#include <iostream>
#include <fstream>

void plotOneSurf(int run, int startEntry, int numEntries,int surf);

void plotOneSurf()
{
  plotOneSurf(1028,1000,1,1);
}
  

void plotOneSurf(int run, int startEntry, int numEntries,int surf) {

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
    
    
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlusFastClockZero,hk);
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

