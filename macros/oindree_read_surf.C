/////////// MACRO WRITTEN BY OINDREE BANERJEE  /////////////
//////// to look at slow data from anita 4 /////////
////// to run do root -l oindree_read_surf.C"(42,339)"  ///////   for starting with run 42 and ending with run 339 

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
#include "SurfHk.h" 

using namespace std;

void oindree_read_surf(int start_run,int end_run);  

void oindree_read_surf(int start_run,int end_run)
{  
  TGraph *g1a = new TGraph(0);
  TGraph *g1b = new TGraph(0);
  TGraph *g1c = new TGraph(0);
  
  TGraph *g2a = new TGraph(0);
  TGraph *g2b = new TGraph(0);
  TGraph *g2c = new TGraph(0);
  
  TChain * surfHkTree = new TChain("surfHkTree"); 

  for (int run_number = start_run; run_number < end_run+1; run_number++)
  { 
    char surfHk_filename[1000]; 
    
    sprintf(surfHk_filename,"/Users/oindreebanerjee/OneDrive/telem1617-south/root/run%d/surfHkFile%d.root",run_number,run_number);

    surfHkTree -> Add(surfHk_filename);

   } //run number
     
   SurfHk *surf = NULL;     

   surfHkTree->SetBranchAddress("surf",&surf); 

   int num_entries = surfHkTree->GetEntries();
   cout << "number of entries is " << num_entries << endl; ; 

   for (ientry = 0; ientry < num_entries; ientry++) 
   {
    surfHkTree->GetEntry(ientry); // after doing get entry you can forget about the tree
    //cout << "scaler goals are " << surf->scalerGoals[0] << "  " << surf->scalerGoals[1] << "  " << surf->scalerGoals[2] << "  " << surf->scalerGoals[3] << endl;
    
    if (surf->scalerGoals[0] != surf->scalerGoals[1]) {cout << "0 NE 1" << endl;}
    if (surf->scalerGoals[1] != surf->scalerGoals[2]) {cout << "1 NE 2" << endl;}
    

    g1a->SetPoint(g1a->GetN(),surf->realTime,surf->scalerGoals[0]);
    g1b->SetPoint(g1b->GetN(),surf->realTime,surf->scalerGoals[1]);
    g1c->SetPoint(g1c->GetN(),surf->realTime,surf->scalerGoals[2]);
    
    g2a->SetPoint(g2a->GetN(),surf->run,surf->scalerGoals[0]); 
    g2b->SetPoint(g2b->GetN(),surf->run,surf->scalerGoals[1]); 
    g2c->SetPoint(g2c->GetN(),surf->run,surf->scalerGoals[2]); 
   } 

TCanvas *c1a = new TCanvas("c1a","c1a",850,850); 
g1a->Draw("A*");
g1b->SetMarkerColor(2);
g1b->Draw("same");
g1c->SetMarkerColor(3); 
g1c->Draw("same");
g1a->GetXaxis()->SetTitle("Time");
g1a->GetXaxis()->SetTimeDisplay(1);
g1a->GetXaxis()->SetTimeFormat("%m-%d");
g1a->GetYaxis()->SetTitle("scaler goals");
g1a->SetTitle("ANITA 4 south telemetry data"); 
c1a->SaveAs("surfHk_plots/scaler_goals.png"); 

TCanvas *c1b = new TCanvas("c1b","c1b",850,850); 
g2a->Draw("alp");
g2b->SetLineColor(3);
g2b->Draw("same");
g2c->SetLineColor(2); 
g2c->Draw("same"); 
g2a->GetXaxis()->SetTitle("Run Number");
g2a->GetYaxis()->SetTitle("scaler goals");
g2a->SetTitle("ANITA 4 south telemetry data");
c1b->SaveAs("surfHK_plots/scaler_goals_run.png");

//TCanvas *c1c = new TCanvas("c1c","c1c",1100,1100); 

} //macro


