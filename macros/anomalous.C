/////////////////////// macro written by Oindree Banerjee on March 22 2017      /////////////////////////////////
/////////////////////// edited April 24 ////////////////////////
/////////////////////   hoping to find the ANITA-4 anomalous events //////////////////////////////////
//////////////////// to run do root -l anomalous.C"(100,150,40)" for starting with run 100, ending with run 150, for channel 40 ///////////////////////

#include "TH2D.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include <iostream>
#include <TString.h>


using namespace std;

void anomalous(int start_run, int end_run, int channel);

void anomalous(int start_run, int end_run, int channel)

{
  TChain c("eventTree");
  for (int run_number = start_run; run_number <= end_run; run_number++) { c.Add(TString::Format("$ANITA_ROOT_DATA/run%i/eventFile%i.root",run_number,run_number)); }

  TCanvas *cc = new TCanvas("cc","cc",1100,1100);
  cout << channel << endl; 
  
  int nentries = c.Draw(Form("xMax[%i]:eventNumber >> hxMax_eventNum_ch%i",channel,channel),Form("xMax[%i] > 1200",channel),"colz");
  cout << "number of anomalous events is: " << nentries << endl; 

  double *eventNum = c.GetV2();
  double *xMax = c.GetV1(); 

  for (int ientry = 0; ientry < nentries; ientry++)
   {
      cout.setf(ios::fixed); 
      cout << ientry << " " << " anomalous event number is " << setprecision(0) << eventNum[ientry] << " " << "xMax is " << xMax[ientry] << endl; 
   } 

  TString save_png1 = "../plots/anomalous/xMax_eventNum_ch" + TString::Itoa(channel,10) + "_run" + TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".png"; 
  TString save_root1 = "../plots/anomalous/xMax_eventNum_ch" + TString::Itoa(channel,10) + "_run" + TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".root"; 
  cc->SaveAs(save_png1.Data());
  cc->SaveAs(save_root1.Data());
  delete cc;

  /*
  TCanvas *cc = new TCanvas("cc","cc",1100,1100);
  cout << channel << endl; 
  c.Draw(Form("xMax[%i] >> hist_xMax_ch%i",channel,channel),"","");
  cc->SetLogy(1); 
  gStyle->SetStatY(0.9); 
  gStyle->SetStatX(0.89); 
  TString save_png2 = "../plots/anomalous/xMax_ch" + TString::Itoa(channel,10) + "_run" + TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".png"; 
  TString save_root2 = "../plots/anomalous/xMax_ch" + TString::Itoa(channel,10) + "_run" +  TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".root"; 
  cc->SaveAs(save_png2.Data());
  cc->SaveAs(save_root2.Data());
  delete cc;
  

  TCanvas *dd = new TCanvas("dd","dd",1100,1100);
  cout << channel << endl; 
  c.Draw(Form("xMin[%i] >> hist_xMin_ch%i",channel,channel),"","");
  dd->SetLogy(1); 
  gStyle->SetStatY(0.9);
  gStyle->SetStatX(0.325); 
  TString save_png3 = "../plots/anomalous/xMin_ch" + TString::Itoa(channel,10) + "_run" + TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".png"; 
  TString save_root3 = "../plots/anomalous/xMin_ch" + TString::Itoa(channel,10) + "_run" +  TString::Itoa(start_run,10) + "to" + TString::Itoa(end_run,10) + ".root"; 
  dd->SaveAs(save_png3.Data());
  dd->SaveAs(save_root3.Data());
  delete dd;
  */ 
  
} // macro 

