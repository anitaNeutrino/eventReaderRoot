/////////// MACRO WRITTEN BY OINDREE BANERJEE  /////////////
//////// to look at slow data from anita 4 /////////
////// to run do root -l oindree_read_mon.C"(42,339)"  ///////   for starting with run 42 and ending with run 339 

//#include "FFTtools.h" 
//#include "AnitaConventions.h" // for WaveCalType
#include "TH2D.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
//#include <stdio.h>
#include <iostream>
#include <TString.h>
//#include "SlowRate.h" 

using namespace std;

void oindree_read_mon(int start_run,int end_run);  

void oindree_read_mon(int start_run,int end_run)
{  
  //signed char b = 1; 
  TGraph *g1 = new TGraph(0);
  TGraph *g2 = new TGraph(0);
  TGraph *g3 = new TGraph(0);
  TGraph *g4 = new TGraph(0);
  //TH2F *h1 = new TH2F("",";latitude (degrees); longitude (degrees)",100,-90,-70,100,-200,200); 
  
  TChain * slowTree = new TChain("slowTree"); 

  for (int run_number = start_run; run_number < end_run+1; run_number++)
  { 
    //cout << "inside for loop over run number and the run number is " << endl; 
    //cout << run_number << endl; 
    
    char slow_filename[1000]; 
    
    sprintf(slow_filename,"/Users/oindreebanerjee/OneDrive/telem1617-south/root/run%d/slowFile%d.root",run_number,run_number);

    slowTree -> Add(slow_filename);

   } //run number
     
   SlowRate *mon = NULL;     

   slowTree->SetBranchAddress("mon",&mon); 

   int num_entries = slowTree->GetEntries();
   cout << "number of entries is " << num_entries << endl; ; 

   for (ientry = 0; ientry < num_entries; ientry++) 
   {
    slowTree->GetEntry(ientry); // after doing get entry you can forget about the tree
    
    g1->SetPoint(g1->GetN(),mon->realTime,mon->SlowRate::getEventRate10Min()); 
    g2->SetPoint(g2->GetN(),mon->realTime,mon->SlowRate::getEventRate1Min());
    
    g3->SetPoint(g3->GetN(),mon->run,mon->SlowRate::getEventRate10Min());
    g4->SetPoint(g4->GetN(),mon->run,mon->SlowRate::getEventRate1Min()); 

    //g1->SetPoint(g1->GetN(),mon->realTime,((mon->eventRate10Min) | b));  // because event rate is unsigned char 
    //g2->SetPoint(g2->GetN(),mon->realTime,((mon->eventRate1Min) | b)); 
    //g3->SetPoint(g3->GetN(),mon->run,((mon->eventRate10Min) | b));
    //g4->SetPoint(g4->GetN(),mon->run,((mon->eventRate1Min) | b)); 
    //cout << "run number is " << mon->run << endl; 
    //cout << "eventRate10Min is " << ((mon->eventRate10Min) | b) << endl;      
   } 

TCanvas *c1a = new TCanvas("c1a","c1a",850,850); 
g1->Draw("A*"); 
g1->SetTitle("ANITA 3 data"); 
g1->GetXaxis()->SetTitle("Time");
g1->GetXaxis()->SetTimeDisplay(1); 
g1->GetXaxis()->SetTimeFormat("%m-%d"); 
g1->GetYaxis()->SetTitle("Event Rate (10 min) (Hz)"); 
c1a->SaveAs("slow_plots/eventRate10Min.png"); 

TCanvas *c1b = new TCanvas("c1b","c1b",850,850); 
g2->Draw("A*"); 
g2->SetTitle("ANITA 3 data"); 
g2->GetXaxis()->SetTitle("Time");
g2->GetXaxis()->SetTimeDisplay(1); 
g1->GetXaxis()->SetTimeFormat("%m-%d"); 
g2->GetYaxis()->SetTitle("Event Rate (1 min) (Hz)"); 
c1b->SaveAs("slow_plots/eventRate1min.png"); 

TCanvas *c1c = new TCanvas("c1c","c1c",1100,1100); 
g3->Draw("A*"); 
g3->SetTitle("ANITA 3 data"); 
g3->GetXaxis()->SetTitle("Run Number");
g3->GetYaxis()->SetTitle("Event Rate (10 min) (Hz)"); 
c1c->SaveAs("slow_plots/eventRate10Min_run.png"); 

TCanvas *c1d = new TCanvas("c1d","c1d",1100,1100); 
g4->Draw("A*"); 
g4->SetTitle("ANITA 3 data"); 
g4->GetXaxis()->SetTitle("Run Number");
g4->GetYaxis()->SetTitle("Event Rate (1 min) (Hz)"); 
c1d->SaveAs("slow_plots/eventRate1Min_run.png"); 


delete c1a;
delete c1b;
delete c1c;
delete c1d; 

} //macro


