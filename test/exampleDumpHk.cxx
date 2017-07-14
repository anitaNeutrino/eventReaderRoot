//////////////////////////////////////////////////////////////////////////////
/////  exampleDumpHk.cxx        ANITA Hk Dumper                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple program that reads in ROOT hk files and dumps text     /////
/////    values.
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;

#include "AnitaConventions.h"
#include "CalibratedHk.h"

#include "TFile.h"
#include "TTree.h"


int main(int argc, char **argv)
{
   if(argc<2) {
      cout << "Usage: " << endl;
      cout << "\t" << argv[0] << " <hk root file> <num entries>" << endl << endl;
      return 0;
   }

   Long64_t maxEntries=0;
   if(argc==3) {
     maxEntries=atoi(argv[2]);
   }
   
   
   
   CalibratedHk *calHk=0;
   char fileName[FILENAME_MAX];
   strcpy(fileName,argv[1]);

   TFile *fp = new TFile(fileName);
   if(!fp) {
     std::cerr << "Couldn't open: " << fileName << "\n";
     return -1;
   }
   TTree *hkTree = (TTree*) fp->Get("hkTree");
   if(!hkTree) {
     std::cerr << "Couldn't get hkTree from " << fileName << "\n";
     return -1;
   }
   hkTree->SetBranchAddress("hk",&calHk);
   
   Long64_t numEntries=hkTree->GetEntries();
   if(numEntries<maxEntries) 
     maxEntries=numEntries;

   ofstream VoltFile("voltTexts.txt");
   

   for(Long64_t i=0;i<maxEntries;i++) {
     hkTree->GetEntry(i);
     if(i==0) {
       VoltFile << "#unixTime";
       for(int voltInd=0;voltInd<NUM_VOLTAGES;voltInd++) {
	 VoltFile << "\t" << calHk->getVoltageName(voltInd);
       }
       VoltFile << "\n";
     }
     VoltFile << calHk->realTime;
     for(int voltInd=0;voltInd<NUM_VOLTAGES;voltInd++) {
       VoltFile << "\t" << calHk->getVoltage(voltInd);
     }
     VoltFile << "\n";
   }  

}
