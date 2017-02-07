/////////// MACRO WRITTEN BY OINDREE BANERJEE  /////////////
//////// to look at how much we phi masked in anita 4 /////////
////// to run do root -l oindree_read_header.C"(42,339)"  ///////   for starting with run 42 

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
//#include "GpuPowerSpectra.h" 
//#include <time.h> 

using namespace std;

void oindree_read_header(int start_run,int end_run);  
int get_num_phi_mask(unsigned short bitmask);

void oindree_read_header(int start_run,int end_run)
{ 
  TGraph *g1 = new TGraph(0);
  TGraph *g2 = new TGraph(0);
  TGraph *g3 = new TGraph(0); 
  TGraph *g4 = new TGraph(0);   
  TGraph *g5 = new TGraph(0); 
  TH1I *h1 = new TH1I("",";deadtime; Number of Entries",100,-1,2); 
  
  TChain * headTree = new TChain("headTree"); 

  for (int run_number = start_run; run_number < end_run+1; run_number++)
  { 
    //cout << "inside for loop over run number and the run number is " << endl; 
    //cout << run_number << endl; 
    
    char head_filename[1000]; 
    
    sprintf(head_filename,"/Users/oindreebanerjee/OneDrive/telem1617-south/root/run%d/headFile%d.root",run_number,run_number);

    headTree -> Add(head_filename);

   } //run number
     
   RawAnitaHeader *header = NULL;     

   headTree->SetBranchAddress("header",&header); 

   int num_entries = headTree->GetEntries();
   cout << "number of entries is " << num_entries << endl; ; 

   for (ientry = 0; ientry < num_entries; ientry++) 
   {
    headTree->GetEntry(ientry); // after doing get entry you can forget about the tree 

    unsigned short bitmask_phi = header->phiTrigMask;
    unsigned short bitmask_L2 = header->l2TrigMask;
    unsigned short bitmask_phiH = header->phiTrigMaskH;
  
    int num_phi_mask = get_num_phi_mask(bitmask_phi); 
    int num_L2_mask = get_num_phi_mask(bitmask_L2); 
    int num_phi_maskH = get_num_phi_mask(bitmask_phiH); 

    if (num_phi_mask != 16) 
    {
    g1->SetPoint(g1->GetN(),header->realTime,num_phi_mask); 
    g2->SetPoint(g2->GetN(),header->run,num_phi_mask); 
    }
    
    g3->SetPoint(g3->GetN(),header->realTime,num_phi_maskH);
 
    if (num_L2_mask != 16) {g4->SetPoint(g4->GetN(),header->realTime,num_L2_mask);}
    //g4->SetPoint(g4->GetN(),header->realTime,num_L2_mask);
    //cout << "deadtime is " << header -> deadTime << endl; 
    
    g5->SetPoint(g5->GetN(),header->realTime,header->run); 


    h1->Fill(header->deadTime);   
   } 

TCanvas *c1 = new TCanvas("c1","c1",850,850); 
g1->Draw("A*"); 
g1->SetTitle("ANITA 4 south telemetry data"); 
g1->GetXaxis()->SetTitle("Time");
g1->GetXaxis()->SetTimeDisplay(1); 
g1->GetXaxis()->SetTimeFormat("%m-%d");
g1->GetYaxis()->SetTitle("Number of phi sectors masked"); 
c1->SaveAs("header_plots/phiTrigMask_vs_realTime.png"); 

TCanvas *c2 = new TCanvas("c2","c2",850,850); 
g2->Draw("A*"); 
g2->SetTitle("ANITA 4 south telemetry data"); 
g2->GetXaxis()->SetTitle("Run Number");
g2->GetYaxis()->SetTitle("Number of phi sectors masked"); 
c2->SaveAs("header_plots/phiTrigMask_vs_run_number.png"); 

TCanvas *c3 = new TCanvas("c3","c3",850,850); 
g3->Draw("alp"); 
g3->SetTitle("phiTrigMaskH vs realTime"); 
g3->GetXaxis()->SetTitle("realTime");
g3->GetYaxis()->SetTitle("Number of phi sectors H masked"); 
c3->SaveAs("header_plots/phiTrigMaskH_vs_realTime.png"); 

TCanvas *c4 = new TCanvas("c4","c4",850,850); 
g4->Draw("A*"); 
g4->SetTitle("ANITA 4 south telemetry data"); 
g4->GetXaxis()->SetTitle("Time");
g4->GetXaxis()->SetTimeDisplay(1); 
g4->GetXaxis()->SetTimeFormat("%m-%d");
g4->GetYaxis()->SetTitle("Number of phi sectors L2 masked"); 
c4->SaveAs("header_plots/L2TrigMask_vs_realTime.png"); 

TCanvas *c5 = new TCanvas("c5","c5",850,850); 
g5->Draw("A*"); 
g5->SetTitle("ANITA 4 south telemetry data"); 
g5->GetXaxis()->SetTitle("Time");
g5->GetYaxis()->SetTitle("Run Number"); 
g5->GetXaxis()->SetTimeDisplay(1); 
c5->SaveAs("header_plots/realtime_run.png"); 

} //macro

int get_num_phi_mask(unsigned short bitmask) {
    int nmask;
    nmask = 0;
    for (int i=0;i<16;i++) if (bitmask & (1<<i)) nmask++;
    return nmask;
}


