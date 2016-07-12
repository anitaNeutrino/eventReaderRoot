
//////////////////////////////////////////////////////////////////////////////
/////  exampleDumpHk.cxx        ANITA Hk Dumper                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple program that reads in ROOT hk files and dumps text     /////
/////    values.
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

#include "AnitaConventions.h"
#include "CalibratedAnitaEvent.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaHeader.h"

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"


int main(int argc, char **argv)
{
   if(argc<3) {
      cout << "Usage: " << endl;
      cout << "\t" << argv[0] << " <base dir> <run> <num entries>" << endl << endl;
      return 0;
   }

   char *baseDir = argv[1];
   Int_t run = atoi(argv[2]);

   Long64_t maxEntries=0;
   if(argc==4) {
     maxEntries=atoi(argv[3]);
   }
   
   
   
   RawAnitaEvent *event = 0;
   CalibratedAnitaEvent *calEvent = 0;
   RawAnitaHeader *header =0;

   char eventName[FILENAME_MAX];
   char headerName[FILENAME_MAX];

  sprintf(eventName,"%s/run%d/calEventFile%d*.root",baseDir,run,run);
  sprintf(headerName,"%s/run%d/headFile%d.root",baseDir,run,run);
 
  //Need a TChain for the event files as they are split in to sub files to ensure no single file is over 2GB
  TChain *eventChain = new TChain("eventTree");
  eventChain->Add(eventName);

  Int_t useCalibratedFiles=1;

  //Here we check if there are any entries in the tree of CalibratedAnitaEvent objects
  if(eventChain->GetEntries()>0) {
    //If there are entries we can set the branc address
    eventChain->SetBranchAddress("event",&calEvent);
    useCalibratedFiles=1;
  }
  else {
    //If there aren't any entries we can try raw event files instead
    sprintf(eventName,"%s/run%d/eventFile%d*.root",baseDir,run,run);
    eventChain->Add(eventName);
    eventChain->SetBranchAddress("event",&event);
  }
  
  //Now open the header and GPS files
  TFile *fpHead = TFile::Open(headerName);
  TTree *headTree = (TTree*) fpHead->Get("headTree");
  headTree->SetBranchAddress("header",&header);

  for(Long64_t entry=0;entry<maxEntries;entry++) {
     //Get header
     headTree->GetEntry(entry);
     
     eventChain->GetEntry(entry);

     UsefulAnitaEvent *realEvent = new UsefulAnitaEvent(calEvent);
     //Now realEvent contains event data

    

  }

}
