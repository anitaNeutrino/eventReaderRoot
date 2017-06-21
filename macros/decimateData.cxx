/// Very simple macro to generate decimated root files: i.e. a "10% data set"
/// 100 entries in a row are selected with a Poisson dist'd gap of 900.
/// Seed set to 1234 for same data set to be produced

#include "AnitaConventions.h"
#include "TimedAnitaHeader.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"

#include <iostream>
#include <fstream>

void decimateData()
{
  cout << "Decimating data sets..." << endl;
  
  RawAnitaHeader * header = 0;
  
  const int firstRun = 42;
  const int lastRun = 367;

  TString baseDir = getenv("ANITA4_ROOT_DATA");

  Int_t entriesInARow = 100;
  Int_t passes = 0;
  Int_t gap = 0;
  Int_t gapMean = 900;
  const Int_t seed = 1234;

  Int_t totEntries = 0;
  Int_t totSaved = 0;
 
  for(int run=firstRun; run <= lastRun; run++)
    {
      cout << TString::Format("%% done = %f \r",Double_t(run - firstRun)/Double_t(lastRun - firstRun) * 100) << flush;

      if(run == 60 || run == 61)
	{
	  cout << "Skipping run " << run << ": known missing header files" << endl;
	  continue;
	}

      ///////////////////////////
      /// Full data run
      ////// Head
      TString headFileName = baseDir + TString::Format("/run%d/timedHeadFile%d.root", run, run);
      TFile *headFile = new TFile(headFileName);  
      TTree *headTree = (TTree*)headFile->Get("headTree");

      headTree->SetBranchAddress("header", &header);
      Long64_t maxEntry = headTree->GetEntries();
      
      //////////////////////////
      /// Decimated data run
      ////// Head
      TString decHeadFileName = baseDir + TString::Format("/run%d/decTimedHeadFile%d.root", run, run);
      TFile *decHeadFile = new TFile(decHeadFileName, "recreate");
      TTree *decHeadTree = headTree->CloneTree(0);

      /// Decimate
      for(Long64_t entry=0; entry < maxEntry; entry++)
	{
	  headTree->GetEntry(entry);

	  totEntries++;

	  if(passes < entriesInARow)
	     {
	      decHeadTree->Fill();
	      passes++;
	      totSaved++;
	    }
	  else if(passes == entriesInARow)
	    {
	      gap = 0;
	      TRandom3 *r = new TRandom3(seed+entry); // Want same data set each time, but still Poisson distr'd. So new fixed seed every entry.
	      gap = r->Poisson(gapMean);
	      //cout << gap << endl;
	      delete r;
	      passes++;
	    }
	  else if (passes > entriesInARow && passes < entriesInARow+gap - 1)
	    {
	      passes++;
	    }
	  else
	    {
	      passes = 0;
	    }
	  
	} // end entry
      
      decHeadTree->Write();
      headFile->Close();
      delete headFile;
      decHeadFile->Close();
      delete decHeadFile;
      
    } // end run loop
  
  cout << "Events left after decimation: " <<  Double_t(totSaved)/Double_t(totEntries) * 100 << "%" << endl;
  cout << "Decimated data sets produced" << endl;

  return;

}
