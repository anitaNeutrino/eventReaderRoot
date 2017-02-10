//////////// MACRO WRITTEN BY OINDREE BANERJEE  /////////////
//////////// read gpu root file to check if notch moved correctly when tuning notch during flight  ///////////////
/////// modified for looking at gpu spectra after flight to see tuff performance //////////////
//// to run //// 
//// do ////
//// root -l oindree_tuff_performance"(102,105,1,4,2,2)"
//// root -l oindree_tuff_performance"(102,105,1,16,4,4)"
//// root -l oindree_tuff_performance"(102,105,1,2,1,1)"


//#include "TFile.h"
//#include "TTree.h"
//#include <stdio.h>
#include <iostream>
#include <TString.h>
#include "GpuPowerSpectra.h" 

using namespace std;

void oindree_tuff_performance(int start_run,int end_run,int start_phi,int end_phi,int canvas_divide_col,int canvas_divide_row);  

void oindree_tuff_performance(int start_run,int end_run,int start_phi,int end_phi,int canvas_divide_col,int canvas_divide_row)
{
  int num_run = end_run - start_run; 
  TString save = "tuff_performance_plots/gpu_run_" + TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + "phi" + TString::Itoa(start_phi,10) + "to" + TString::Itoa(end_phi,10) + ".png"; 
  TString g_title; 
 // leg = new TLegend(0.1,0.2,0.8,0.8); 
 // leg->SetHeader("Run Number"); 

  TChain *gpuTree = new TChain("gpuTree");
  
  for (int run_number = start_run; run_number < end_run+1; run_number++){
   
   char gpu_filename[1000]; 

   sprintf(gpu_filename,"/Users/oindreebanerjee/OneDrive/telem1617-south/root/run%d/gpuFile%d.root",run_number,run_number);
   
   gpuTree->Add(gpu_filename); 
   
   } // run_number

  GpuPowerSpectra *gpu = NULL;     

  gpuTree->SetBranchAddress("gpu",&gpu); 

  int num_entries = gpuTree->GetEntries();

  cout << "number of entries is " << num_entries << endl; 
     
  TCanvas *c1 = new TCanvas("c1","c1",1100,1100); 
  c1->Divide(canvas_divide_col,canvas_divide_row); 
  c1->SetGridx(); 
  c1->SetGridy(); 
  
  for (int ientry = 0; ientry < num_entries; ientry++) 
   {
     gpuTree->GetEntry(ientry); 
     for (int phi_sector = start_phi-1; phi_sector < end_phi; phi_sector++)
      {
       c1->cd(phi_sector+1); 
       TGraph *g = gpu->GpuPowerSpectra::getGraph(phi_sector);
       g->GetXaxis()->SetRangeUser(200.,1200.);  
       g->GetXaxis()->SetTitle("Frequency (MHz)"); 
       g->GetYaxis()->SetRangeUser(0.,50.);
       g->GetYaxis()->SetTitle("Power (dB)");
       if (start_run != end_run) 
        {
         g_title = "Averaged Spectra from Runs " + TString::Itoa(start_run,10) + " to " + TString::Itoa(end_run,10) + " Phi Sector " + TString::Itoa(phi_sector+1,10); 
        }       
       else
        { 
         g_title = "Averaged Spectra from Run " + TString::Itoa(start_run,10) + " Phi Sector " + TString::Itoa(phi_sector+1,10); 
        }
       g->SetTitle(g_title.Data());
       g->GetXaxis()->SetLabelSize(0.05);
       g->GetYaxis()->SetLabelSize(0.05); 
       g->GetXaxis()->SetTitleSize(0.05);
       g->GetYaxis()->SetTitleSize(0.05); 

       for (int i = 0; i < num_run+1; i++){
        if (gpu->run == start_run + (i%(num_run+1))){
         g->SetLineColor(i+1);
         g->SetMarkerColor(i+1); 
         //leg->AddEntry(g,TString::Itoa((start_run+i),10),"l"); 
         //g->SetMarkerStyle(i+1);
         }}        
    
       g->Draw(ientry == 0 ? "ALP" : "LP");
       //leg->Draw(); 
        
      } // for loop over phi sector  
   } // for loop over entry

c1->SaveAs(save.Data()); 
//delete c1; 
} //macro






