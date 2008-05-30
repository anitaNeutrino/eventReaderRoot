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


void searchForMissingEvs()
{

  FileStat_t staty;
  char headerName[FILENAME_MAX];
  TimedAnitaHeader *header =0;
  
  TChain *headChain = new TChain("headTree");
  for(int run=1030;run<=1239;run++) {
    sprintf(headerName,"/unix/anita1/newRootData/run%d/timedHeadFile%d.root",run,run);
    //    cout << headerName << endl;
    if(!gSystem->GetPathInfo(headerName,staty))
      headChain->Add(headerName);
  }
  headChain->SetBranchAddress("header",&header);


  Int_t numEntries=headChain->GetEntries();
  cout << "Entries:\t" << numEntries << endl;

  Int_t lastEventNum=0;
  Int_t lastRun=0;
  Int_t wasGood=0;

  for(int entry=0;entry<numEntries;entry++) {
    headChain->GetEntry(entry);
    
    if(header->eventNumber==lastEventNum+1)
      wasGood=1;
    else if(wasGood) {
      cout << lastRun << "\t" << lastEventNum << "\t" << header->eventNumber << endl;
      wasGood=0;
    }    
    lastEventNum=header->eventNumber;
    lastRun=header->run;
  }
  //  canSurf->cd(10);
  //  sqaurey->Draw();
}

