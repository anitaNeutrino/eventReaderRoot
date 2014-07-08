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
#include "TROOT.h"
#include <iostream>
#include <fstream>

void deltaTest(int run, int startEntry, int numEntries);

void deltaTest()
{
  deltaTest(1028,1000,1);
}
  

void deltaTest(int run, int startEntry, int numEntries) {

  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  sprintf(eventName,"/TBdata/anita/antarctica08/root/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"/TBdata/anita/antarctica08/root/run%d/headFile%d.root",run,run);

  RawAnitaEvent *event = 0;
  RawAnitaHeader *header =0;

  
  TChain *eventChain = new TChain("eventTree");
  eventChain->Add(eventName);
  eventChain->SetBranchAddress("event",&event);

  TFile *fpHead = new TFile(headerName);
  TTree *headTree = (TTree*) fpHead->Get("headTree");
  headTree->SetBranchAddress("header",&header);


  for(int entry=startEntry;entry<startEntry+numEntries;entry++) {
    
    //Stupidly most do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);

       
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTLabAG,header);
    cout << realEvent.eventNumber << " " << header->eventNumber << endl;
    //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
    
    TGraph *gr = realEvent.getGraphFromSurfAndChan(0,0);
    Double_t *timeVals=gr->GetX();
    for(int i=1;i<gr->GetN();i++)
      cout << i << "\t" << timeVals[i]-timeVals[i-1] << "\n";


  }
}

