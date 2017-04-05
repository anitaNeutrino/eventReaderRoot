// Quick macro for finding Usable time
// As some Hk data is unavailable, rendering pat information unfindable, I'm using this to determine the Usable time instead of say, event data

// Identifies missing realTime periods, and looks for suspicious gapso
// Successfully finds runs w/ gps outages, missing events etc

#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "AnitaGeomTool.h"
#include "RawAnitaHeader.h"
#include "TimedAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "FFTtools.h"
#include "TurfRate.h"
#include "Adu5Pat.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH2.h"
#include "TH1.h"
#include "TColor.h"
#include "TImage.h"
#include "TMath.h"
#include "TStyle.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>
#include "TMarker.h"
#include "TLatex.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TPaveText.h"

void findUsableTime(int firstRun, int lastRun);

void findUsableTime(int firstRun, int lastRun)
{

  FileStat_t staty;
  char prettyName[FILENAME_MAX];
  TChain *prettyChain = new TChain("prettyHkTree");
  
  for (int run=firstRun; run<lastRun+1; run++)
    { 
      sprintf(prettyName,"/unix/anita4/flight2016/root/run%d/prettyHkFile%d.root",run,run);
      if(gSystem->GetPathInfo(prettyName,staty))
	{
	  continue;
	}
      prettyChain->Add(prettyName);
    }
  
  PrettyAnitaHk *hk = 0;
  prettyChain->SetBranchAddress("hk",&hk); 
  int maxEntries = prettyChain->GetEntries();
  cout << "There are " << maxEntries << " entries in prettyHk Chain " << endl;

  UInt_t startTime = 0;
  UInt_t finalTime = 0;
  UInt_t realTimeC = 0; // current realTime entry
  UInt_t realTimeL = 0; // last realTime entry

  UInt_t typicalMaxRunTransitionTime = 29; // max realTime difference between runs
  UInt_t currentSuspiciousRun = firstRun;
  UInt_t lastSuspiciousRun = firstRun;
  UInt_t suspiciousRuns = 0;
  
  UInt_t missingTimePeriod = 0;
  UInt_t totalTime = 0;
  UInt_t totalTimeMissing = 0;
  UInt_t UsableTime = 0;
  Double_t UsableTimePerc = 0;
  Double_t fullUsableTimePerc = 0;

  for (int entry = 0; entry < maxEntries; entry++) 
   {
     
    prettyChain->GetEntry(entry);
    
    realTimeC = hk->realTime;

    if(entry == 0)
      {
	realTimeL = hk->realTime;

	startTime = hk->realTime;
      }

    else
      {
	prettyChain->GetEntry(entry-1);
	realTimeL = hk->realTime;
      }
    
    if( (realTimeC != (realTimeL + 1))  &&  (realTimeC != realTimeL) )
      {

	if(realTimeC > realTimeL) // weird feature with adu5A outage where current real time < last real time...
	  {
	    std::cout << "Run is: " << hk->run << std::endl;
	    
	    std::cout << "realTimeC = " << realTimeC << std::endl;
	    std::cout << "realTimeL = " << realTimeL << std::endl;
	  }
	
	missingTimePeriod = 0;

	missingTimePeriod = realTimeC - realTimeL;
	
	if( (realTimeC - realTimeL) > 4294967290) // bug where we get max int or similar
	  {
	    missingTimePeriod = 0;
	  }

	if(missingTimePeriod > typicalMaxRunTransitionTime * 2) // sometimes the run resets twice, find values higher than these
	  {
	    currentSuspiciousRun = hk->run;

	    if(currentSuspiciousRun != lastSuspiciousRun)
	      {
		std::cout << "SUSPICIOUS RUN DETECTED: run " << hk->run << std::endl;
		suspiciousRuns++;
	      }
	   
	    lastSuspiciousRun = hk->run;
	  }

	if(missingTimePeriod != 0) // don't print the corrected error
	  {
	    std::cout << "Transition in real time:" << missingTimePeriod << std::endl;
	  }
	
	totalTimeMissing += missingTimePeriod;

      }

    if(entry == maxEntries - 1)
      {

	std::cout << "|||||||||||||||| Total flight stats |||||||||||||||" << std::endl;
	
	finalTime = hk->realTime;

	totalTime = finalTime - startTime;
	
	UsableTime = totalTime - totalTimeMissing;

	std::cout << "Total missing time = " << totalTimeMissing << " seconds" << std::endl;

	std::cout << " = " << Double_t(totalTimeMissing)/Double_t(60*60) << " hours " << std::endl;
	
	std::cout << "Total Usable time = " << UsableTime << " seconds" << std::endl;

	std::cout << "= " << Double_t(UsableTime)/(Double_t(60*60*24)) << " days " << std::endl;
	
	UsableTimePerc = Double_t(totalTime - totalTimeMissing)/Double_t(totalTime) * 100;
	std:: cout << "% Usable time = " << UsableTimePerc << std::endl;

	fullUsableTimePerc = Double_t(totalTime - totalTimeMissing)/Double_t(2.394e+06) * 100;

	std::cout << "% total Usable time = " << fullUsableTimePerc << std::endl;

	std::cout << "Total # suspicious runs = " <<  suspiciousRuns << std::endl;

	std::cout << "||||||||||||||||                    |||||||||||||||" << std::endl;

	
	
      }

   } // end entry loop

  return;
  
}
