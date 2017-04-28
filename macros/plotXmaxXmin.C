/////////////////////// macro written by Oindree Banerjee on March 22 2017      /////////////////////////////////
/////////////////////// edited April 24-28 ////////////////////////
/////////////////////   part of effort to find ANITA-4 payload blasts //////////////////////////////////
//////////////////////  plots xMax and xMin for the runs specified by user /////////////////////////////////
//////////////////// to run do root -l plotXmaxXmin.C"(42,367)" for running for runs 42 through 367     ///////////////////////

#include "TH2D.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include <iostream>
#include <TString.h>

using namespace std;

void plotXmaxXmin(int start_run, int end_run);

void plotXmaxXmin(int start_run, int end_run)

{
  TChain c("eventTree");
  for (int run_number = start_run; run_number <= end_run; run_number++) { c.Add(TString::Format("$ANITA_ROOT_DATA/run%i/eventFile%i.root",run_number,run_number)); }

  TCanvas *cc = new TCanvas("cc","cc",1100,1100);

  c.Draw("xMax >> hist_xMax","","");
  cc->SetLogy(1); 
  gStyle->SetStatY(0.9); 
  gStyle->SetStatX(0.89); 
  TString save_png2 = "../plots/anomalous/xMax_run" + TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".png"; 
  TString save_root2 = "../plots/anomalous/xMax_run" +  TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".root"; 
  cc->SaveAs(save_png2.Data());
  cc->SaveAs(save_root2.Data());
  delete cc;
  

  TCanvas *dd = new TCanvas("dd","dd",1100,1100);
 
  c.Draw("xMin >> hist_xMin","","");
  dd->SetLogy(1); 
  gStyle->SetStatY(0.9);
  gStyle->SetStatX(0.325); 
  TString save_png3 = "../plots/anomalous/xMin_run" + TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".png"; 
  TString save_root3 = "../plots/anomalous/xMin_run" +  TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".root"; 
  dd->SaveAs(save_png3.Data());
  dd->SaveAs(save_root3.Data());
  delete dd;

  
} // macro 

