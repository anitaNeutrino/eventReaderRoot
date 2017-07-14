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
#include <cstdlib>

void exampleLoopAllEvents(int run);

int main(int argc, char **argv) {
  int runNumber=10367;
  if(argc>1) {
    runNumber=atoi(argv[1]);
  }
  exampleLoopAllEvents(runNumber);
}
    
  



void exampleLoopAllEvents(int run) {

  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  sprintf(eventName,"http://192.168.1.2/antarctica14/root/run%d/eventFile%d.root",run,run);
  sprintf(headerName,"http://192.168.1.2/antarctica14/root/run%d/headFile%d.root",run,run);

  RawAnitaEvent *event = 0;
  RawAnitaHeader *header =0;
  
  TFile  *fpEvent = TFile::Open(eventName);
  TTree *eventTree = (TTree*) fpEvent->Get("eventTree");
  eventTree->SetBranchAddress("event",&event);

  TFile *fpHead = TFile::Open(headerName);
  TTree *headTree = (TTree*) fpHead->Get("headTree");
  headTree->SetBranchAddress("header",&header);


  UInt_t numEntries=headTree->GetEntries();
  UInt_t numStars=50;
  UInt_t starEvery=numEntries/numStars;
  if(starEvery==0) starEvery++;
  UInt_t count=0;
  
  std::cout << "There are " << numEntries << " in run " << run << std::endl;
  std::cout << "eventTree->GetEntries()\t" << eventTree->GetEntries() << std::endl;
  if(numEntries>1000)numEntries=1000;

  //  eventTree->GetEntry(0);
  for(UInt_t entry=0;entry<numEntries;entry++) {
    if(count%starEvery==0) std::cerr << "*";
    //Stupidly most do this to be perfectly safe  
     eventTree->GetEntry(entry);
     headTree->GetEntry(entry);
    
   
    //This step produces a calibrated UsefulAnitaEvent
    UsefulAnitaEvent realEvent(event,WaveCalType::kDefault);
    std::cout << realEvent.eventNumber << " " << header->eventNumber << std::endl;
    //  std::cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << std::endl;
    count++;
  }
  std::cerr << std::endl;
  std::cout << "Processed " << count << " events.\n";
}

