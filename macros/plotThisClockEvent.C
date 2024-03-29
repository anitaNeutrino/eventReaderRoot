//#include "AnitaConventions.h"
#include <iostream>

void plotThisClockEvent()
{
   gStyle->SetPadTopMargin(0.05);
   gStyle->SetPadBottomMargin(0.2);
   gStyle->SetPadLeftMargin(0.12);
   gStyle->SetPadRightMargin(0.05);
   gStyle->SetTitleH(0.1);
   gStyle->SetLabelSize(0.1,"xy");
   gStyle->SetTitleSize(0.1,"xy");
   gStyle->SetTitleOffset(0.5,"y");
   gStyle->SetOptTitle(0);
   plotThisClockEvent(1028,255488);
}
  

void plotThisClockEvent(int run, int eventNumber) {

  gSystem->Load("libMathMore.so");
  gSystem->Load("/usr/lib64/libfftw3.so");
  gSystem->Load("libRootFftwWrapper.so");
  gSystem->Load("libAnitaEvent.so");

  char eventName[180];
  char headerName[180];
  char hkName[180];
  sprintf(eventName,"/unix/anita1/webData/firstDay/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"/unix/anita1/webData/firstDay/run%d/timedHeadFile%d.root",run,run);
  sprintf(hkName,"/unix/anita1/webData/firstDay/run%d/prettyHkFile%d.root",run,run);

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


  Int_t startEntry=headTree->GetEntryNumberWithIndex(eventNumber,eventNumber);
  if(startEntry<0) {
    headTree->BuildIndex("eventNumber","eventNumber");
    startEntry=headTree->GetEntryNumberWithIndex(eventNumber,eventNumber);
    if(startEntry<0)
      return;
  }
  //  cout << headTree->GetEntryNumberWithIndex(eventNumber,eventNumber) << endl;

  //Friends only seem to work with TTree::Draw and similar commands
  //if you are manually calling GetEntry (i.e in a loop) you must call
  //the GetEntry for each tree separately.
  eventChain->AddFriend(headTree);
  eventChain->AddFriend(prettyHkTree);


  TCanvas *canClock = (TCanvas*) gROOT->FindObject("canClock");
  if(!canClock) {
    canClock = new TCanvas("canClock","canClock",800,400);
  }
  char textLabel[180];
  char pngName[180];
  TGraph *gr[9]={0};
  //  Int_t startEntry=0;
  Int_t numEntries=1;

  for(int entry=startEntry;entry<startEntry+numEntries;entry++) {
    
    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    prettyHkTree->GetEntry(entry);	
       
    
       
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlusFancyClockZero,hk);
    cout << entry << "\t" <<  realEvent.eventNumber << " " << header->eventNumber << endl;
    //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
    canClock->Clear();
    canClock->SetTopMargin(0);
    TPaveText *topPave = new TPaveText(0.05,0.92,0.95,0.98);
    topPave->SetBorderSize(0);
    
    sprintf(textLabel,"Event %lu",realEvent.eventNumber);
    TText *eventText = topPave->AddText(textLabel);
    eventText->SetTextColor(50);
    topPave->Draw();
    
    TPad *graphPad = new TPad("graphPad","This is pad1",0,0,1,0.9);
    graphPad->SetTopMargin(0);
    graphPad->Draw();
    graphPad->Clear();
    //    graphPad->Clear();
    for(int surf=0;surf<9;surf++) {
      if(gr[surf]) delete gr[surf];
    }
    //    graphPad->Divide(2,5);
    graphPad->cd();
    TH1F *framey = gPad->DrawFrame(-10,-300,110,300);
    for(int surf=0;surf<9;surf++) {
       //      graphPad->cd(surf+1);
      gr[surf] = realEvent.getGraph(UsefulAnitaEvent::getChanIndex(surf,8));
      gr[surf]->SetLineColor(getNiceColour(surf));
      gr[surf]->Draw("l");
	       
    }
    canClock->Update();
    sprintf(pngName,"clocks%lu.png",realEvent.eventNumber);
    //    canClock->Print(pngName);
    //       break;
    //    gSystem->Sleep(1000);
  
  }
}
