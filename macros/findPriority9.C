////////////////////////// macro written by Oindree Banerjee on April 26 2017 while visiting UCL ///////////////////////////////////////////
///////////////////////// to run do findPriority9.C"(42,367,9)" for ANITA-IV runs and priority 9 

#include "TH2D.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include <iostream>
#include <TString.h>


using namespace std;

void findPriority9(int start_run, int end_run, int priority);

void findPriority9(int start_run, int end_run, int priority)

{

   TChain c("headTree");
   for (int run_number = start_run; run_number <= end_run; run_number++) { c.Add(TString::Format("$ANITA_ROOT_DATA/run%i/timedHeadFile%i.root",run_number,run_number)); }

   TCanvas *cc = new TCanvas("cc","cc",1100,1100);
   int nentries = c.Draw("eventNumber:run",Form("(priority & 0x0f) == %i",priority),"colz");

   TString save_png1 = "../plots/anomalous/priority" + TString::Itoa(priority,10) + "run" + TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".png"; 
   TString save_root1 = "../plots/anomalous/priority" + TString::Itoa(priority,10) + "run" + TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".root"; 
   cc->SaveAs(save_png1.Data());
   cc->SaveAs(save_root1.Data());
   delete cc;

   cout << Form("Number of priority %i events is ",priority) << nentries << endl; 

   double *eventNum = c.GetV1();
   double *run = c.GetV2(); 

   ofstream myfile;
   myfile.open (Form("priority%ilist.txt",priority));
   myfile << "#      eventNumber     run \n";

   for (int ientry = 0; ientry < nentries; ientry++)
     { 
       //cout << ientry << " " << Form(" priority %i event number is ",priority) << setprecision(11) << eventNum[ientry] << " " << "and run number is " << run[ientry] << endl; 
       myfile << ientry << "       " << setprecision(11) << eventNum[ientry] << "       " << run[ientry] << "    " <<"\n"; //write to file
     } 


}
